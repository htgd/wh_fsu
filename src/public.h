/**@file    	public.h
 * @brief   	动环监控公共头文件
 * @details
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

#ifndef __PUBLIC_H
#define	__PUBLIC_H

#include <pthread.h>
#include <stdint.h>
#include "bIf_protocol.h"

#define ADDR_LENGTH		50	///< 配置地址长度

/**
 * @brief 设备配置信息结构体
 */
typedef struct
{
	TDevConf devConf;				///< 设备信息
	char DeviceSN[FSUID_LEN];		///< 设备序列号
	char bIfServer[ADDR_LENGTH];	///< B接口远端服务器地址
	char webIf[ADDR_LENGTH];		///< web浏览地址
} DeviceCFG_t;

/**@brief RS485端口配置结构体 */
typedef struct
{
	char name[10];
	int  speed;
	int  databits;
	int  stopbits;
	int  parity;
	char option;
} RS485CFG_t;

/**@brief 数字输入IO配置结构体 */
typedef struct
{
	uint8_t option;	//IO配置（0：上拉，1：下拉，2：开漏）
} IO_DI_CFG_t;

/**@brief 数字输出IO配置结构体 */
typedef struct
{
	uint8_t option;	//IO配置（0：推挽输出，1：开漏）
} IO_DO_CFG_t;

/**@brief 模拟输入IO配置结构体 */
typedef struct
{
	uint8_t option;	//IO配置（0：上拉，1：下拉，2：开漏）
} IO_AI_CFG_t;


typedef struct
{
	uint8_t devAddr;
	uint8_t cmd;
	uint16_t starMbAddr;
	uint16_t byteSum;
	uint16_t crc;
} CONTENT_RECORD;



extern DeviceCFG_t g_DeviceCfg;	//设备配置信息
extern RS485CFG_t *g_RS485Cfg;	//RS485接口配置信息
extern uint8_t g_RS485CfgNum;	//RS485配置端口数（通过数据库获取）


extern void DebugPrint(char *fmt, ...);




#endif	/* __PUBLIC_H */

