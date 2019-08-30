/**@file    	monitor.c
 * @brief   	动环监控主进程入口
 * @details  	支持设置调试输出口
 * @author  	wanghuan  any question please send mail to 371463817@qq.com
 * @date    	2019-08-06
 * @version 	V1.0
 * @copyright	Copyright (c) 2019-2022  江苏亨通光网科技有限公司
 **********************************************************************************
 * @attention
 * OS:  L4.1.15
 * gcc: gcc-linaro-4.9
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2018/08/17  <td>1.0      <td>wanghuan  <td>创建初始版本
 * </table>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "public.h"

extern void * bIfClientThread(void *arg);
extern void * bIfServerThread(void *arg);
extern void * slavePollThread(void *arg);

extern void sqTable_init(void);

int main()
{
	pthread_t threadId[10];
	void *retval;

	DebugPrint("wangh linux monitor process\n");
	DebugPrint("\n");
	DebugPrint("compile time is: %s %s\n", __DATE__, __TIME__);
	DebugPrint("\n");

	sqTable_init();	//数据库表初始化

	pthread_create(&threadId[4], NULL, &bIfClientThread, 0);	//B接口客户端线程创建
	pthread_create(&threadId[5], NULL, &bIfServerThread, 0);	//B接口服务器线程创建
	pthread_create(&threadId[6], NULL, &slavePollThread, 0);	//监测设备轮询线程创建

	//等待线程
	pthread_join(threadId[5], &retval);
	pthread_join(threadId[6], &retval);

	return 0;
}


















