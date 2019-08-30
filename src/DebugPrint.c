/**@file    	DebugPrint.c
 * @brief   	调试打印接口
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
#include <string.h>
#include <stdarg.h>

#define DEBUG_STRING_MAX_LEN 1024	///< 调试打印最大字节数

/**@struct S_DebugMsg
 * @brief 调试打印消息结构体
 */
typedef struct
{
	unsigned int ModuleId;
	unsigned int length;
	unsigned char data[DEBUG_STRING_MAX_LEN];
} S_DebugMsg;

/**@enum debugWhere
 * @brief 调试打印输出口枚举
 */
enum
{
	noDebug = 0,
	serialDebug,
	udpDebug,
	mqttDebug
} debugWhere;

/** 调试打印输出口定义 */
char g_debugWhere = serialDebug;

/**
 * @brief 调试打印
 */
void DebugPrint(char *fmt, ...)
{
	S_DebugMsg debugMsg;
	int printStrLen = 0;

	if (noDebug == g_debugWhere)
		return;
	va_list ap;
	va_start(ap, fmt);
	printStrLen = vsnprintf((char *)&debugMsg.data[0], DEBUG_STRING_MAX_LEN, fmt, ap);
	va_end(ap);

	if (g_debugWhere == serialDebug)
	{
		printf("%s", debugMsg.data);
		return;
	}
	if (g_debugWhere == udpDebug)
	{
//		debugMsg.length = htons(printStrLen);
//		sendto(g_sockfd, &debugMsg.data, printStrLen, 0, (struct sockaddr *)&g_debugAddr, sizeof(struct sockaddr_in));
	}
	if (g_debugWhere == mqttDebug)
	{
//		debugMsg.length = htons(printStrLen);
//
//		pthread_mutex_lock(&comBuff0.lock);
//		if (printStrLen > 2)
//		{
//			AP_circleBuff_WritePacket((INT8U *)debugMsg.data, printStrLen, DTU2MQTPC);
//		}
//		pthread_mutex_unlock(&comBuff0.lock);
		//sendto(g_sockfd, &debugMsg.data, printStrLen, 0,(struct sockaddr *)&g_debugAddr, sizeof(struct sockaddr_in));
	}
}







