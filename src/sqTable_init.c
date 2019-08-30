/**@file    	sqTable_init.c
 * @brief   	数据库初始化、数据库处理线程及数据库操作函数
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "public.h"

const char *dbPathName = "/home/fsu.db";

DeviceCFG_t g_DeviceCfg;
RS485CFG_t *g_RS485Cfg;


static void getDeviceCfg(void);
static void getRS485Cfg(void);
uint8_t g_RS485CfgNum;

/**
 * @brief 数据库表初始化
 */
void sqTable_init(void)
{
	getDeviceCfg();	//从数据库获取设备配置信息
	getRS485Cfg();	//从数据库获取RS485配置
}

/**
 * @brief 从数据库中获取设备信息
 */
static void getDeviceCfg(void)
{
	sqlite3 *pdb = NULL;
	char **pResult;	//查表数据
	int nRow, nCol, nResult;
	char *errmsg;
	nResult = sqlite3_open_v2(dbPathName, &pdb, SQLITE_OPEN_READWRITE, NULL);
	if(nResult != SQLITE_OK)
	{
		DebugPrint("can't open database:%s, then exit the process\n", sqlite3_errmsg(pdb));
		sqlite3_close(pdb);
		exit(EXIT_FAILURE);
	}

	//开始查找设备信息表
	//获取设备ID
	char *strSql = "select content from CfgTab_devInfo where cfgName = 'DeviceID'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.DeviceID, pResult[nCol], DEVICEID_LEN);
		DebugPrint("设备ID--> %s %d %d\n", g_DeviceCfg.devConf.DeviceID, nRow, nCol);
	}
	//获取设备名称
	strSql = "select content from CfgTab_devInfo where cfgName = 'DeviceName'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.DeviceName, pResult[nCol], NAME_LENGTH);
		DebugPrint("设备名称--> %s\n", g_DeviceCfg.devConf.DeviceName);
	}
	//获取机房名称
	strSql = "select content from CfgTab_devInfo where cfgName = 'RoomName'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.RoomName, pResult[nCol], NAME_LENGTH);
		DebugPrint("机房名称--> %s\n", g_DeviceCfg.devConf.RoomName);
	}
	//获取设备型号
	strSql = "select content from CfgTab_devInfo where cfgName = 'DeviceModel'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.Model, pResult[nCol], NAME_LENGTH);
		DebugPrint("设备型号--> %s\n", g_DeviceCfg.devConf.Model);
	}
	//获取设备品牌
	strSql = "select content from CfgTab_devInfo where cfgName = 'DeviceBrand'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.Brand, pResult[nCol], NAME_LENGTH);
		DebugPrint("设备品牌--> %s\n", g_DeviceCfg.devConf.Brand);
	}
	//获取设备序列号
	strSql = "select content from CfgTab_devInfo where cfgName = 'DeviceSN'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.DeviceSN, pResult[nCol], FSUID_LEN);
		DebugPrint("设备序列号--> %s\n", g_DeviceCfg.DeviceSN);
	}
	//获取版本
	strSql = "select content from CfgTab_devInfo where cfgName = 'Version'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.Version, pResult[nCol], VER_LENGTH);
		DebugPrint("版本--> %s\n", g_DeviceCfg.devConf.Version);
	}
	//获取启用时间
	strSql = "select content from CfgTab_devInfo where cfgName = 'BeginRunTime'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.BeginRunTime, pResult[nCol], TIME_LEN);
		DebugPrint("启用时间--> %s\n", g_DeviceCfg.devConf.BeginRunTime);
	}
	//获取设备描述信息
	strSql = "select content from CfgTab_devInfo where cfgName = 'DevDescribe'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.devConf.DevDescribe, pResult[nCol], DES_LENGTH);
		DebugPrint("设备描述信息--> %s\n", g_DeviceCfg.devConf.DevDescribe);
	}
	//获取B接口远程服务器地址
	strSql = "select content from CfgTab_devInfo where cfgName = 'bRemoteServer'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.bIfServer, pResult[nCol], ADDR_LENGTH);
		DebugPrint("B接口远程服务器地址--> %s\n", g_DeviceCfg.bIfServer);
	}
	//获取本机web浏览器访问地址
	strSql = "select content from CfgTab_devInfo where cfgName = 'webAccessAddr'";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if ((nResult != SQLITE_OK) || (pResult[nCol] == NULL))
	{
		goto end;
	}
	else
	{
		strncpy(g_DeviceCfg.webIf, pResult[nCol], ADDR_LENGTH);
		DebugPrint("本机web浏览器访问地址--> %s\n", g_DeviceCfg.webIf);
	}

	//查看全表
	strSql = "select * from CfgTab_devInfo";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if (nResult != SQLITE_OK)
	{
		goto end;
	}
	int i,j;
	int index = nCol;
	for( i = 0; i < nRow ; i++ )
	{
			for( j = 0 ; j < nCol; j++ )
			{
				DebugPrint( "%s ",pResult[index++]);
			}
			printf("\n");
	}

end:
	sqlite3_close(pdb);
	sqlite3_free(errmsg);
}

/**
 * @brief 从数据库获取RS485配置
 */
void getRS485Cfg(void)
{
	sqlite3 *pdb = NULL;
	char **pResult;	//查表数据
	int nRow, nCol, nResult;
	char *errmsg;
	//前面已验证
	nResult = sqlite3_open_v2(dbPathName, &pdb, SQLITE_OPEN_READWRITE, NULL);
	//查表CfgTab_RS485
	char *strSql = "select * from CfgTab_RS485";
	nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);
	if (nResult != SQLITE_OK || nRow == 0)
	{
		DebugPrint("sqlite3_get_table err: %s\n", errmsg);
		goto end;
	}

	g_RS485CfgNum = nRow;	//RS485配置端口数（即CfgTab_RS485表行数）
	//为全局RS485配置存储分配内存
	if ((g_RS485Cfg = (RS485CFG_t *)malloc((g_RS485CfgNum) * sizeof(RS485CFG_t))) == NULL)
	{
		DebugPrint("malloc g_RS485Cfg error\n");
		exit(EXIT_FAILURE);	//内存出错退出进程
	}
	int nIndex = nCol;
	int i;
	for (i = 0; i < g_RS485CfgNum; i++)
	{
		if(strstr(pResult[nIndex], "RS485") == NULL)
			continue;
		strncpy(g_RS485Cfg[i].name, pResult[nIndex], 10);
		g_RS485Cfg[i].speed = atoi(pResult[nIndex + 1]);
		g_RS485Cfg[i].databits = atoi(pResult[nIndex + 2]);
		g_RS485Cfg[i].stopbits = atoi(pResult[nIndex + 3]);
		g_RS485Cfg[i].parity = atoi(pResult[nIndex + 4]);
		nIndex = nIndex + nCol;
		DebugPrint("%s %d %d %d %d\n", g_RS485Cfg[i].name, g_RS485Cfg[i].speed, g_RS485Cfg[i].databits, g_RS485Cfg[i].stopbits, g_RS485Cfg[i].parity);
	}

end:
	sqlite3_close(pdb);
	sqlite3_free(errmsg);
}
























/**
 * @brief sqlite3通用语句执行
 * @param[in] *strSql  sql操作语句字符串指针
 */
int sql_general_set_interface(char *strSql)
{
	char **pResult;
	int nResult;
	char *errmsg;

	DebugPrint("\nstrSql=%s\n", strSql);
	sqlite3 *pdb = NULL;
	nResult = sqlite3_open_v2(dbPathName, &pdb, SQLITE_OPEN_READWRITE, NULL);
    if (nResult != SQLITE_OK)
    {
    	DebugPrint("can't open database:%s\n", sqlite3_errmsg(pdb));
    	goto end;
    }
	nResult = sqlite3_exec(pdb, strSql, 0, 0, &errmsg);		//执行sql语句
	if (nResult != SQLITE_OK)
	{
		DebugPrint("sql_general_set_interface err\n");
		goto end;
	}
	else
	{
		DebugPrint("sql_general_set_interface ok\n");
	}
end:
	sqlite3_close(pdb);
	sqlite3_free(errmsg);
	return nResult;
}

/**
 * @brief sqlite3通用语句读表
 * @param[in] *strSql  sql操作语句字符串指针
 */
int sql_general_get_interface(char *strSql)
{
	char **pResult;
	int nRow, nCol, nResult;
	char *errmsg;

	DebugPrint("\nstrSql=%s\n", strSql);
	sqlite3 *pdb = NULL;
	nResult = sqlite3_open_v2(dbPathName, &pdb, SQLITE_OPEN_READWRITE, NULL);
    if (nResult != SQLITE_OK)
    {
    	DebugPrint("can't open database:%s\n", sqlite3_errmsg(pdb));
    	goto end;
    }
    nResult = sqlite3_get_table(pdb, strSql, &pResult, &nRow, &nCol, &errmsg);	//sql查表
	if (nResult != SQLITE_OK)
	{
		DebugPrint("sql_general_get_interface err %s\n", errmsg);
		goto end;
	}

end:
	sqlite3_close(pdb);
	sqlite3_free(errmsg);
	return nResult;
}







