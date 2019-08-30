# 动环监控主机程序说明  

----------

## 一、简述  

动环监控主机采用ARM架构，主机功耗低，杜绝网页病毒干扰，提高设备安全性。配合动环软件可以实现UPS监控，配电监控、空调监控、视频监控、门禁监控、温湿度监控、电气火灾检测、防盗监控等。支持数据采集处理、联动控制、报表浏览等功能，内嵌WEB服务器支持。用户通过IE浏览器查看监控场地环境、设备参数、告警、报表等信息，代替人工巡检，实现无人值守，提高维护效率，降低管理成本。  

特性如下：  

 - 支持B接口通信；  
 - 支持web浏览；  
 - 支持非标Modbus设备；  
 - 支持标准Modbus设备。  

### 1.1、文件结构  

|源文件                                        |描述   |
|:------------------------------               |:----- |
|\src\monitor.c                        |主进程入口，创建功能子线程|
|\src\DebugPrint.c                      |调试打印接口，支持重定向到UDP|
|\src\ipc_client.c |IPC摄像头soap客户端程序|
|\src\slavePoll.c             |监测设备轮询|
|\src\bIf_protocol.c     |B接口协议实现（包含客户端与服务器）|
|\src\sqTable_init.c   |数据库操作初始化及操作接口|
|\src\uartDrvFun.c      |UART驱动操作接口|
|\src\public.h    |公共头文件|
|\bin |目标可执行文件 monitor、fsu_web|
|\bin\web |嵌入式web网页实现|
|\build   |编译、调试文件夹|
|\lib\sqlite3 |sqlite3库及头文件，数据库|
|\lib\libmodbus |modbus库及头文件|
|\lib\goahead |goahead库及头文件,嵌入式web实现|
|\soap_sc |B接口soap支持源码|
|\soap_ipc |ipc摄像头soap客户端支持源码|
|\web_src |嵌入式web底层源码（子进程）|
|\CMakeLists.txt |动环监控工程cmake文件|
|\config.h.in |动环监控工程cmake编译配置文件|
|\cross.cmake |动环监控工程交叉编译配置文件|
|\fsu.db |动环监控工程数据库文件|

> 注：其他功能逐步调试添加中  

----------

### 1.2、工具编译安装方法  
#### 1.2.1 sqllite3  
**下载**  
```bash
wget https://www.sqlite.org/2019/sqlite-autoconf-3290000.tar.gz
```
**解压、编译、安装**  
```bash
tar -xzvf sqlite-autoconf-3290000.tar.gz
cd sqlite-autoconf-3290000
mkdir install
chmod 777 install
./configure --prefix=$(pwd)/install --host=arm-linux CC=arm-linux-gnueabihf-gcc
make
make install
```
install 文件夹下就是需要的库（lib）和头文件（include）  

-------

#### 1.2.2 libmodbus  
**下载**
```bash
wget http://libmodbus.org/releases/libmodbus-3.1.4.tar.gz
```
**解压、编译、安装**  
```bash
tar -xzvf libmodbus-3.1.4.tar.gz
cd libmodbus-3.1.4
mkdir install
chmod 777 install
./configure --prefix=$(pwd)/install --host=arm-linux --enable-static ac_cv_func_malloc_0_nonnull=yes CC=arm-linux-gnueabihf-gcc
make
make install
```

---------

#### 1.2.3 gsoap-fsu  

---------

### 1.3、程序源码编译安装方法  

确保宿主机安装有cmake  
在cross.cmake文件中配置自己的交叉编译环境  
在build文件夹中运行终端依次执行如下：   

```bash
cmake ..
make
```
**目标机配置**  
数据库位置： /home/fsu.db  
动态库安装位置： /usr/lib/  

> 注：程序所依赖驱动及动态库需配置到目标主机。  
>   

---------

### 1.4、程序安装包安装方法  




## 二、数据库设计  

### 2.1、  

