/**@file    	sqlite_api.c
 * @brief   	数据库操作封装接口
 * @details
 * @author  	wanghuan  any question please send mail to 371463817@qq.com
 * @date    	2019-08-13
 * @version 	V1.0
 * @copyright	Copyright (c) 2019-2022  江苏亨通光网科技有限公司
 **********************************************************************************
 * @attention
 * OS:  L4.1.15
 * gcc: gcc-linaro-4.9
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2018/08/13  <td>1.0      <td>wanghuan  <td>创建初始版本
 * </table>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "sqlite_api.h"
#include "public.h"

/**
 * @brief 打开数据库文件
 * @param[in]  *pDbPath  数据库文件路径
 * @param[out] *pDb      数据库句柄
 * @return 	正确返回SQ_OK，错误返回SQ_ERROR
 */
int Open_DB(sqlite3 *pDb, const char *pDbPath)
{
	if (NULL == pDbPath)
	{
		return SQ_ERROR;
	}
	int nResult = sqlite3_open_v2(pDbPath, &pDb, SQLITE_OPEN_READWRITE, NULL);
	if(SQLITE_OK != nResult)
	{
		return SQ_ERROR;
	}
	return SQ_OK;
}

/**
 * @brief 关闭数据库
 * @param[in] *pDb      数据库句柄
 * @return 	正确返回SQ_OK，错误返回SQ_ERROR
 */
int Close_DB(sqlite3 *pDb)
{
	if (NULL == pDb)
	{
		return SQ_ERROR;
	}
	int nResult = sqlite3_close(pDb);
	if(SQLITE_OK != nResult)
	{
		return SQ_ERROR;
	}
	pDb = NULL;
	return SQ_OK;
}

/**
 * @brief 执行数据库字符串指令
 * @param[in] *pDb      数据库句柄
 * @param[in] *pStrSql  数据库指令字符串
 * @return 	正确返回SQ_OK，错误返回SQ_ERROR
 */
int ExecSQL(sqlite3 *pDb, char *pStrSql)
{
	if (NULL == pStrSql || strlen(pStrSql) <= 0 || NULL == pDb)
	{
		return SQ_ERROR;
	}
	char *pErrMsg = NULL;
	int nResult = sqlite3_exec(pDb, pStrSql, NULL, NULL, &pErrMsg);
	if(SQLITE_OK != nResult)
	{
		DebugPrint("sqlite3_exec err: %s\n", pErrMsg);
		sqlite3_free(pErrMsg);
		return SQ_ERROR;
	}
	return SQ_OK;
}

int GetTable(sqlite3 *pDb, char *pStrSql, char ***pTableResult, int *pRow, int *pColumn)
{
	if (NULL == pStrSql || strlen(pStrSql) <= 0 || NULL == pTableResult || NULL == pRow || NULL == pColumn || NULL == pDb)
	{
		return SQ_ERROR;
	}
	char* pErrMsg = NULL;
	int iResult = sqlite3_get_table(pDb, pStrSql, pTableResult, pRow, pColumn, &pErrMsg);

	if (SQLITE_OK != iResult)
	{
		DebugPrint("sqlite3_get_table err: %s\n", pErrMsg);
		sqlite3_free(pErrMsg);
		return SQ_ERROR;
	}

	return SQ_OK;
}
















