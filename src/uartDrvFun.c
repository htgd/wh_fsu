/**@file  	    uartDrvFun.c
 * @brief   	串口驱动接口
 * @details
 * @author  	wanghuan  any question please send mail to 371463817@qq.com
 * @date    	2019-06-17
 * @version 	V1.0
 * @copyright	Copyright (c) 2019-2022  江苏亨通光网科技有限公司
 **********************************************************************************
 * @attention
 * 硬件平台:iMX6ULL \n
 * 内核版本：L4.1.15
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2019/06/17  <td>1.0      <td>wanghuan  <td>创建初始版本
 * </table>
 **********************************************************************************
 */

/* 包含的头文件 */
#include <stdio.h>		//标准输入输出,如printf、scanf以及文件操作
#include <stdlib.h>		//标准库头文件，定义了五种类型、一些宏和通用工具函数
#include <unistd.h>		//定义 read write close lseek 等Unix标准函数
#include <fcntl.h>		//文件控制定义

#include <termios.h>	//终端I/O
#include <string.h>		//字符串操作
#include <time.h>		//时间，-lrt
#include <sys/select.h>	//select函数，用于查询文件变化
#include "public.h"

//static const char *serialPath[] = {"/dev/ttymxc5", "/dev/ttymxc2", "/dev/ttymxc3", "/dev/ttymxc1", "/dev/ttymxc4", "/dev/ttymxc6"};
//static const char *rs485[] = {"RS485-1", "RS485-2", "RS485-3", "RS485-4", "RS485-5", "RS485-6"};

//宏定义
#define UART_FALSE 	-1
#define UART_TRUE 	0

/**
 * @brief 打开串口并返回串口设备文件描述符
 * @param[in]  *port 	串口设备（"/dev/ttymxc2"）
 * @return 	返回串口设备文件描述符
 * - fd 	串口设备文件描述符
 * - UART_FALSE  打开失败
 */
int UART_Open(char *port)
{
	int fd;
	// 打开串口
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd < 0)
	{
		perror(port);
		return UART_FALSE;
	}
	// 恢复设置串口为阻塞状态， 0：阻塞， FNDELAY：非阻塞
	if (fcntl(fd, F_SETFL, 0) < 0)	//阻塞，即使前面在open串口设备时设置的是非阻塞的
	{
		DebugPrint("fcntl failed!\n");
		return UART_FALSE;
	}
	else
	{
		DebugPrint("fcntl=%d\n", fcntl(fd, F_SETFL, 0));
	}
	// 测试是否为终端设备
	if (isatty(fd) == 0)
	{
		DebugPrint("standard input is not a terminal device\n");
		close(fd);
		return UART_FALSE;
	}
	else
	{
		DebugPrint("is a tty success!\n");
	}
	DebugPrint("fd->open=%d ok\n", fd);
	return fd;
}

/**
 * @brief 关闭串口
 * @param[in]  fd 	串口设备描述符
 */
void UART_Close(int fd)
{
	close(fd);
}

/**
 * @brief 设置串口参数：波特率，数据位，停止位和效验位
 * @param[in]  fd 	        串口文件描述符
 * @param[in]  speed        串口波特率(支持300~230400)
 * @param[in]  flow_ctrl 	数据流控（0：无流控，1：硬件流控，2：软件流控）
 * @param[in]  databits     数据位，取值为7或者8
 * @param[in]  stopbits     停止位，取值为1或者2
 * @param[in]  parity 	    效验类型，取值为N,E,O,,S
 * @return 正确返回为0，错误返回为-1
 */
int UART_Set(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{

	int i;
	int status;
	int speed_arr[] = {B230400,B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300};
	int name_arr[] = {230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300};

	struct termios options;

	/*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.
    */
	if (tcgetattr(fd, &options) != 0)
	{
		perror("SetupSerial 1");
		return (UART_FALSE);
	}
	memset(&options, 0, sizeof(options));	//配置清零

	// 设置串口输入波特率和输出波特率
	for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
	{
		if (speed == name_arr[i])
		{
			cfsetispeed(&options, speed_arr[i]);
			cfsetospeed(&options, speed_arr[i]);
		}
	}

	//修改控制模式，保证程序不会占用串口
	options.c_cflag |= CLOCAL;
	//修改控制模式，使得能够从串口中读取输入数据
	options.c_cflag |= CREAD;

	//设置数据流控制
	switch (flow_ctrl)
	{

	case 0: //不使用流控制
		options.c_cflag &= ~CRTSCTS;
		break;

	case 1: //使用硬件流控制
		options.c_cflag |= CRTSCTS;
		break;
	case 2: //使用软件流控制
		options.c_cflag |= IXON | IXOFF | IXANY;
		break;
	}
	//设置数据位
	options.c_cflag &= ~CSIZE;	//屏蔽其他标志位
	switch (databits)
	{
	case 5:
		options.c_cflag |= CS5;
		break;
	case 6:
		options.c_cflag |= CS6;
		break;
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		options.c_cflag |= CS8;
		break;
	}
	//设置校验位
	switch (parity)
	{
	case 'n':
	case 'N': //无奇偶校验位。
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~INPCK;
		break;
	case 'o':
	case 'O': //设置为奇校验
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= (INPCK | ISTRIP);	//INPCK：启用输入校验检测，ISTRIP：从接收字符串中去掉奇偶校验位
		break;
	case 'e':
	case 'E': //设置为偶校验
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		options.c_iflag |= (INPCK | ISTRIP);
		break;
	case 's':
	case 'S': //设置为空格
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default: //无奇偶校验位。
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~INPCK;
		break;
	}
	// 设置停止位
	switch (stopbits)
	{
	case 1:	//1个停止位
		options.c_cflag &= ~CSTOPB;
		break;
	case 2: //2个停止位
		options.c_cflag |= CSTOPB;
		break;
	default:
		options.c_cflag &= ~CSTOPB;
		break;
	}

	//如果前面termios结构意清零，则下面无需特意设置
	//修改输出模式，原始数据输出
	options.c_oflag &= ~OPOST;
	//修改输入模式，原始数据输入
	options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	//设置等待时间和最小接收字符
	options.c_cc[VTIME] = 30; 	// 读取一个字符等待30*(1/10)s
	options.c_cc[VMIN] = 0;  	// 读取字符的最少个数为1

	//如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
	tcflush(fd, TCIOFLUSH);
	//激活配置 (将修改后的termios数据设置到串口中）
	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
		perror("com set error!\n");
		return (UART_FALSE);
	}
	return (UART_TRUE);
}

/**
 * @brief 串口发送
 * @param[in]  fd 	        串口文件描述符
 * @param[in]  *send_buf    串口发送数据指针
 * @param[in]  data_len 	数据长度
 * @return 成功发送的字节数，错误返回为-1
 */
int UART_Send(int fd, char *send_buf, int data_len)
{
	int len = 0;

	len = write(fd, send_buf, data_len);
	if (len == data_len)
	{
		return len;
	}
	else
	{
		tcflush(fd, TCOFLUSH);
		return UART_FALSE;
	}
}






































