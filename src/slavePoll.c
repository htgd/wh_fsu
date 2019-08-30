/**@file  	    slavePoll.c
 * @brief   	动换监控RS485接口监测轮询
 * @details
 * @author  	wanghuan  any question please send mail to 371463817@qq.com
 * @date    	2019-08-07
 * @version 	V1.0
 * @copyright	Copyright (c) 2019-2022  江苏亨通光网科技有限公司
 **********************************************************************************
 * @attention
 * 硬件平台:iMX6ULL \n
 * 内核版本：L4.1.15
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2019/08/07  <td>1.0      <td>wanghuan  <td>创建初始版本
 * </table>
 **********************************************************************************
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "public.h"
#include "modbus.h"

static void pollThreadcreate(void);
static const int idx[7]={0,1,2,3,4,5,6};

extern void non_mbPollUartTreat(uint8_t idx);
extern void mbPollUartTreat(uint8_t idx);

/**
 * @brief 东环监控从设备轮询线程
 */
void * slavePollThread(void *arg)
{
	pollThreadcreate();
	sleep(1);
	return 0;
}

/**
 * @brief 非标设备轮询线程
 */
void * non_mbPollThread(void *arg)
{
    int num=*(int *)arg;
	DebugPrint("Non-std PORT %d CREATE\n",num);
	non_mbPollUartTreat(num);

	pthread_exit((void *)0);
}

/**
 * @brief 标准modbus接口轮询线程
 */
void * mbPollThread(void *arg)
{
	uint8_t num=*(uint8_t *)arg;

	DebugPrint("RS485 PORT %d CREATE \n",num);
	if(num<7)
	{
		mbPollUartTreat(num);	//modbus串口处理
	}
	DebugPrint("no data create to poll ,so RS485 PORT %d thread EXIT  \n",num);
	pthread_exit((void *)0);
}

/**
 * @brief 为每个RS485接口创建轮询线程 \n
 * 其中分配（1-6）为标准modbus接口，7为非标设备接口
 */
static void pollThreadcreate(void)
{
	int i;
	char rc;
    pthread_t thr[7];
	if(pthread_create(&thr[6], NULL, non_mbPollThread, (void *)&idx[6])!=0)
	{
		DebugPrint("create thread 6 failed1\n");
		return;
	}
     for(i=0;i<6;i++)	//6个modbus接口轮询线程
     {
		if(pthread_create(&thr[i], NULL, mbPollThread, (void *)&idx[i])!=0)
		{
			DebugPrint("create thread %d failed!\n",i);
			return;
		}
     }
}


void non_mbPollUartTreat(uint8_t idx)
{
	while(1)
	{
		sleep(20);
		DebugPrint("Non-std poll...\n");
	}
}

/**
 * @brief modbus设备轮询及数据处理
 * @param[in]  idx 	串口设备（"/dev/ttymxc2"）
 */
void mbPollUartTreat(uint8_t idx)
{
	while(1)
	{
		sleep(10);
		DebugPrint("mb port %d poll...\n", idx);
	}



//	modbus_t *ctx = NULL;
//
//	// 以串口的方式创建libmobus实例,并设置参数
//	ctx = modbus_new_rtu(MODBUS_DEV_NAME, 115200, 'N', 8, 1);
//	if (ctx == NULL)                //使用UART4,对应的设备描述符为ttymxc3
//	{
//		fprintf(stderr, "Unable to allocate libmodbus contex\n");
//		return -1;
//	}
//	// 使用RS485时需考虑设置串口模式、RTS引脚等
////	modbus_rtu_set_serial_mode(MODBUS_RTU_RS485);	//设置串口模式
//
//	modbus_set_debug(ctx, 1);      //设置1可看到调试信息
//	modbus_set_slave(ctx, 1);      //设置slave ID
//
//	if (modbus_connect(ctx) == -1) //等待连接设备
//	{
//		fprintf(stderr, "Connection failed:%s\n", modbus_strerror(errno));
//		return -1;
//	}
//
//	int i,rc;
//	uint16_t tab_reg[64] = {0}; //定义存放数据的数组
//	while (1)
//	{
//		printf("\n----------------\n");
//		//读取保持寄存器的值，可读取多个连续输入保持寄存器
//		rc = modbus_read_registers(ctx, 0, 10, tab_reg);
//		if (rc == -1)
//		{
//			fprintf(stderr,"%s\n", modbus_strerror(errno));
//			return -1;
//		}
//		for (i=0; i<10; i++)
//		{
//			printf("reg[%d] = %d(0x%x)\n", i, tab_reg[i], tab_reg[i]);
//		}
//
//		usleep(5000000);
//	}
//	modbus_close(ctx);  //关闭modbus连接
//	modbus_free(ctx);   //释放modbus资源，使用完libmodbus需要释放掉
}














