/*
    goahead.c -- Main program for GoAhead

    Usage: goahead [options] [documents] [IP][:port] ...
        Options:
        --auth authFile        # User and role configuration
        --background           # Run as a Linux daemon
        --home directory       # Change to directory to run
        --log logFile:level    # Log to file file at verbosity level
        --route routeFile      # Route configuration file
        --verbose              # Same as --log stdout:2
        --version              # Output version information

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "goahead.h"
#include    "js.h"

/********************************* Defines ************************************/

static int finished = 0;

#undef ME_GOAHEAD_LISTEN
/*
    These must match TOP.es.set
 */
#if TEST_IPV6
#if ME_COM_SSL
    #define ME_GOAHEAD_LISTEN "http://127.0.0.1:18080, https://127.0.0.1:14443, http://[::1]:18090, https://[::1]:14453"
#else
    #define ME_GOAHEAD_LISTEN "http://127.0.0.1:18080, http://[::1]:18090"
#endif
#else
#if ME_COM_SSL
    #define ME_GOAHEAD_LISTEN "http://192.168.1.124:80, https://192.168.1.124:443"
#else
    #define ME_GOAHEAD_LISTEN "http://192.168.1.124:80"
#endif
#endif


/********************************* Forwards ***********************************/

static void initPlatform(void);
static void logHeader(void);
static void usage(void);

#if WINDOWS
static void windowsClose();
static int windowsInit();
static LRESULT CALLBACK websWindProc(HWND hwnd, UINT msg, UINT wp, LPARAM lp);
#endif

static bool testHandler(Webs *wp);
#if ME_GOAHEAD_JAVASCRIPT
static int aspTest(int eid, Webs *wp, int argc, char **argv);
static int bigTest(int eid, Webs *wp, int argc, char **argv);
#endif
static void actionTest(Webs *wp);
static void sessionTest(Webs *wp);
static void showTest(Webs *wp);
// 测试action函数
static void on_led_set(Webs *wp);
static void on_buzzer_set(Webs *wp);

#if ME_GOAHEAD_UPLOAD && !ME_ROM
static void uploadTest(Webs *wp);
#endif
#if ME_GOAHEAD_LEGACY
static int legacyTest(Webs *wp, char *prefix, char *dir, int flags);
#endif

#if ME_UNIX_LIKE
static void sigHandler(int signo);
#endif

static void exitProc(void *data, int id);

/*********************************** Code *************************************/

MAIN(goahead, int argc, char **argv, char **envp)
{
    char    *argp, *home, *documents, *endpoints, *endpoint, *route, *auth, *tok, *lspec;
    int     argind, duration;

#if WINDOWS
    if (windowsInit() < 0) {
        return 0;
    }
#endif
    route = "route.txt";
    auth = "auth.txt";
    duration = 0;

    // 命令行解析
    for (argind = 1; argind < argc; argind++) {
        argp = argv[argind];
        if (*argp != '-') {
            break;

        } else if (smatch(argp, "--auth") || smatch(argp, "-a")) {
            if (argind >= argc) usage();
            auth = argv[++argind];

#if ME_UNIX_LIKE && !MACOSX
        } else if (smatch(argp, "--background") || smatch(argp, "-b")) {
            websSetBackground(1);
#endif

        } else if (smatch(argp, "--debugger") || smatch(argp, "-d") || smatch(argp, "-D")) {
            websSetDebug(1);

        } else if (smatch(argp, "--duration")) {
                    if (argind >= argc) usage();
                    duration = atoi(argv[++argind]);

        } else if (smatch(argp, "--home")) {
            if (argind >= argc) usage();
            home = argv[++argind];
            if (chdir(home) < 0) {
                error("Cannot change directory to %s", home);
                exit(-1);
            }
        } else if (smatch(argp, "--log") || smatch(argp, "-l")) {
            if (argind >= argc) usage();
            logSetPath(argv[++argind]);

        } else if (smatch(argp, "--verbose") || smatch(argp, "-v")) {
            logSetPath("stdout:2");

        } else if (smatch(argp, "--route") || smatch(argp, "-r")) {
            route = argv[++argind];

        } else if (smatch(argp, "--version") || smatch(argp, "-V")) {
            printf("%s\n", ME_VERSION);
            exit(0);

        } else if (*argp == '-' && isdigit((uchar) argp[1])) {
            lspec = sfmt("stdout:%s", &argp[1]);
            logSetPath(lspec);
            wfree(lspec);

        } else {
            usage();
        }
    }
    documents = ME_GOAHEAD_DOCUMENTS;
    if (argc > argind) {
        documents = argv[argind++];
    }
    initPlatform();		// 平台初始化，注册信号处理函数
    if (websOpen(documents, route) < 0) // 初始化服务器
    {
        error("Cannot initialize server. Exiting.");
        return -1;
    }
#if ME_GOAHEAD_AUTH		// 加载路径和鉴权配置文件
    if (websLoad(auth) < 0)
    {
        error("Cannot load %s", auth);
        return -1;
    }
#endif
    logHeader();		// 打印web服务器基本信息
    if (argind < argc) {
        while (argind < argc) {
            endpoint = argv[argind++];	// 参数中指定了服务器的endpoint 如./goahead -v ./web/  192.168.10.111:80
            // WEB端口监听
            if (websListen(endpoint) < 0) {
                return -1;
            }
        }
    } else {
        endpoints = sclone(ME_GOAHEAD_LISTEN);
        for (endpoint = stok(endpoints, ", \t", &tok); endpoint; endpoint = stok(NULL, ", \t,", &tok)) {
#if !ME_COM_SSL
            if (strstr(endpoint, "https")) continue;
#endif
            if (websListen(endpoint) < 0) {
                wfree(endpoints);
                return -1;
            }
        }
        wfree(endpoints);
    }

#if ME_ROM && KEEP	// 采用web文件ROM化
    /*
        If not using a route/auth config files, then manually create the routes like this:
        If custom matching is required, use websSetRouteMatch. If authentication is required, use websSetRouteAuth.
     */
    websAddRoute("/", "file", 0);
#endif
#ifdef GOAHEAD_INIT
    /*
        Define your init function in main.me goahead.init, or
        configure with DFLAGS=GOAHEAD_INIT=myInitFunction
     */
    {
        extern int GOAHEAD_INIT();

        if (GOAHEAD_INIT() < 0) {
            exit(1);
        }
    }
#endif

    websDefineHandler("test", testHandler, 0, 0, 0);	// 定义一个请求处理程序
    websAddRoute("/test", "test", 0);
#if ME_GOAHEAD_LEGACY
	// 表示对 /goform 的请求都交给 websFormHandler 函数处理。函数的参数列表如下
    websUrlHandlerDefine("/legacy/", 0, 0, legacyTest, 0);	// 设置form方式调用时候的文件位置
//    websFormDefine(T("odbc_form_web_login"), odbc_form_web_login);	// 定义form方式调用接口函数字符对应的函数名称
#endif
#if ME_GOAHEAD_JAVASCRIPT
    websDefineJst("aspTest", aspTest);		// 定义一个js本地函数
    websDefineJst("bigTest", bigTest);
#endif
    websDefineAction("test", actionTest);	// goAction定义，在asp文件中调用C函数
    websDefineAction("sessionTest", sessionTest);
    websDefineAction("showTest", showTest);

    websDefineAction("led", on_led_set);
    websDefineAction("buzzer", on_buzzer_set);

#if ME_GOAHEAD_UPLOAD && !ME_ROM
    websDefineAction("uploadTest", uploadTest);
#endif


#if ME_UNIX_LIKE && !MACOSX
    /*
        Service events till terminated
     */
    if (websGetBackground()) {
        if (daemon(0, 0) < 0) {
            error("Cannot run as daemon");
            return -1;
        }
    }
#endif

    if (duration) {
        printf("Running for %d secs\n", duration);
        websStartEvent(duration * 1000, (WebsEventProc) exitProc, 0);
    }
    websServiceEvents(&finished);
    logmsg(1, "Instructed to exit\n");
    websClose();

#if WINDOWS
    windowsClose();
#endif
    return 0;
}


static void exitProc(void *data, int id)
{
    websStopEvent(id);
    finished = 1;
}

static void logHeader(void)
{
    char    home[ME_GOAHEAD_LIMIT_STRING];

    getcwd(home, sizeof(home));
    logmsg(2, "Configuration for %s", ME_TITLE);
    logmsg(2, "---------------------------------------------");
    logmsg(2, "Version:            %s", ME_VERSION);
    logmsg(2, "BuildType:          %s", ME_DEBUG ? "Debug" : "Release");
    logmsg(2, "CPU:                %s", ME_CPU);
    logmsg(2, "OS:                 %s", ME_OS);
    logmsg(2, "Host:               %s", websGetServer());
    logmsg(2, "Directory:          %s", home);
    logmsg(2, "Documents:          %s", websGetDocuments());
    logmsg(2, "Configure:          %s", ME_CONFIG_CMD);
    logmsg(2, "---------------------------------------------");
}


static void usage(void) {
    fprintf(stderr, "\n%s Usage:\n\n"
        "  %s [options] [documents] [[IPaddress][:port] ...]\n\n"
        "  Options:\n"
#if ME_GOAHEAD_AUTH
        "    --auth authFile        # User and role configuration\n"
#endif
#if ME_UNIX_LIKE && !MACOSX
        "    --background           # Run as a Unix daemon\n"
#endif
        "    --debugger             # Run in debug mode\n"
        "    --home directory       # Change to directory to run\n"
        "    --log logFile:level    # Log to file file at verbosity level\n"
        "    --route routeFile      # Route configuration file\n"
        "    --verbose              # Same as --log stdout:2\n"
        "    --version              # Output version information\n\n",
        ME_TITLE, ME_NAME);
    exit(-1);
}


static void initPlatform(void)
{
#if ME_UNIX_LIKE
	signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGKILL, sigHandler);	// 增加退出信号处理
    #ifdef SIGPIPE
        signal(SIGPIPE, SIG_IGN);
    #endif
#elif ME_WIN_LIKE
    _fmode=_O_BINARY;
#endif
}


#if ME_UNIX_LIKE
static void sigHandler(int signo)
{
    finished = 1;
}
#endif


/*
    Simple handler and route test
    Note: Accesses to "/" are normally remapped automatically to /index.html
 */
static bool testHandler(Webs *wp)
{
    if (smatch(wp->path, "/")) {
        websRewriteRequest(wp, "/home.html");
        /* Fall through */
    }
    return 0;
}


#if ME_GOAHEAD_JAVASCRIPT
/*
    Parse the form variables: name, address and echo back
 */
static int aspTest(int eid, Webs *wp, int argc, char **argv)
{
	char	*name, *address;

	if (jsArgs(argc, argv, "%s %s", &name, &address) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	return (int) websWrite(wp, "Name: %s, Address %s", name, address);
}


/*
    Generate a large response
 */
static int bigTest(int eid, Webs *wp, int argc, char **argv)
{
    int     i;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html>\n");
    for (i = 0; i < 800; i++) {
        websWrite(wp, " Line: %05d %s", i, "aaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccccddddddd<br/>\r\n");
    }
    websWrite(wp, "</html>\n");
    websDone(wp);
    return 0;
}
#endif


/*
    Implement /action/actionTest. Parse the form variables: name, address and echo back.
 */
static void actionTest(Webs *wp)
{
	cchar	*name, *address;

	name = websGetVar(wp, "name", NULL);
	address = websGetVar(wp, "address", NULL);
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
	websWrite(wp, "<html><body><h2>name: %s, address: %s</h2></body></html>\n", name, address);
    websFlush(wp, 0);
	websDone(wp);
}


static void sessionTest(Webs *wp)
{
	cchar	*number;

    if (scaselessmatch(wp->method, "POST")) {
        number = websGetVar(wp, "number", 0);
        websSetSessionVar(wp, "number", number);
    } else {
        number = websGetSessionVar(wp, "number", 0);
    }
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html><body><p>Number %s</p></body></html>\n", number);
    websDone(wp);
}


static void showTest(Webs *wp)
{
    WebsKey     *s;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "<html><body><pre>\n");
    for (s = hashFirst(wp->vars); s; s = hashNext(wp->vars, s)) {
        websWrite(wp, "%s=%s\n", s->name.value.string, s->content.value.string);
    }
    websWrite(wp, "</pre></body></html>\n");
    websDone(wp);
}

// led控制
static void on_led_set(Webs *wp)
{
	// get the input value in query stream
//	char *io_val;
//
//	io_val = websGetVar(wp, "val_led", NULL);
//	if(io_val)
//		system("echo 1 > /sys/class/leds/led-err/brightness");
//	else
//		system("echo 0 > /sys/class/leds/led-err/brightness");
}

// 蜂鸣器控制
static void on_buzzer_set(Webs *wp)
{
//	char *io_val;
//
//	io_val = websGetVar(wp, "val_buz", NULL);
//	if(io_val)
//		system("echo 1 > /sys/class/leds/beep/brightness");
//	else
//		system("echo 0 > /sys/class/leds/beep/brightness");
}


#if ME_GOAHEAD_UPLOAD && !ME_ROM
/*
    Dump the file upload details. Don't actually do anything with the uploaded file.
 */
static void uploadTest(Webs *wp)
{
    WebsKey         *s;
    WebsUpload      *up;
    char            *upfile;

    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");
    websWriteEndHeaders(wp);
    if (scaselessmatch(wp->method, "POST")) {
        for (s = hashFirst(wp->files); s; s = hashNext(wp->files, s)) {
            up = s->content.value.symbol;
            websWrite(wp, "FILE: %s\r\n", s->name.value.string);
            websWrite(wp, "FILENAME=%s\r\n", up->filename);
            websWrite(wp, "CLIENT=%s\r\n", up->clientFilename);
            websWrite(wp, "TYPE=%s\r\n", up->contentType);
            websWrite(wp, "SIZE=%d\r\n", up->size);
            upfile = sfmt("%s/tmp/%s", websGetDocuments(), up->clientFilename);
            if (rename(up->filename, upfile) < 0) {
                error("Cannot rename uploaded file: %s to %s, errno %d", up->filename, upfile, errno);
            }
            wfree(upfile);
        }
        websWrite(wp, "\r\nVARS:\r\n");
        for (s = hashFirst(wp->vars); s; s = hashNext(wp->vars, s)) {
            websWrite(wp, "%s=%s\r\n", s->name.value.string, s->content.value.string);
        }
    }
    websDone(wp);
}
#endif


#if ME_GOAHEAD_LEGACY
/*
    Legacy handler with old parameter sequence
 */
static int legacyTest(Webs *wp, char *prefix, char *dir, int flags)
{
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteHeader(wp, "Content-Type", "text/plain");
    websWriteEndHeaders(wp);
    websWrite(wp, "Hello Legacy World\n");
    websDone(wp);
    return 1;
}

#endif



#if WINDOWS
/*
    Create a taskbar entry. Register the window class and create a window
 */
static int windowsInit()
{
    HINSTANCE   inst;
    WNDCLASS    wc;                     /* Window class */
    HMENU       hSysMenu;
    HWND        hwnd;

    inst = websGetInst();
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = inst;
    wc.hIcon         = NULL;
    wc.lpfnWndProc   = (WNDPROC) websWindProc;
    wc.lpszMenuName  = wc.lpszClassName = ME_NAME;
    if (! RegisterClass(&wc)) {
        return -1;
    }
    /*
        Create a window just so we can have a taskbar to close this web server
     */
    hwnd = CreateWindow(ME_NAME, ME_TITLE, WS_MINIMIZE | WS_POPUPWINDOW, CW_USEDEFAULT,
        0, 0, 0, NULL, NULL, inst, NULL);
    if (hwnd == NULL) {
        return -1;
    }

    /*
        Add the about box menu item to the system menu
     */
    hSysMenu = GetSystemMenu(hwnd, FALSE);
    if (hSysMenu != NULL) {
        AppendMenu(hSysMenu, MF_SEPARATOR, 0, NULL);
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    return 0;
}


static void windowsClose()
{
    HINSTANCE   inst;

    inst = websGetInst();
    UnregisterClass(ME_NAME, inst);
}


/*
    Main menu window message handler.
 */
static LRESULT CALLBACK websWindProc(HWND hwnd, UINT msg, UINT wp, LPARAM lp)
{
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            finished++;
            return 0;

        case WM_SYSCOMMAND:
            break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}


/*
    Check for Windows Messages
 */
WPARAM checkWindowsMsgLoop()
{
    MSG     msg;

    if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        if (!GetMessage(&msg, NULL, 0, 0) || msg.message == WM_QUIT) {
            return msg.wParam;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}


/*
    Windows message handler
 */
static LRESULT CALLBACK websAboutProc(HWND hwndDlg, uint msg, uint wp, long lp)
{
    LRESULT    lResult;

    lResult = DefWindowProc(hwndDlg, msg, wp, lp);

    switch (msg) {
        case WM_CREATE:
            break;
        case WM_DESTROY:
            break;
        case WM_COMMAND:
            break;
    }
    return lResult;
}

#endif

/*
    Copyright (c) Embedthis Software. All Rights Reserved.
    This software is distributed under commercial and open source licenses.
    You may use the Embedthis GoAhead open source license or you may acquire
    a commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.
 */
