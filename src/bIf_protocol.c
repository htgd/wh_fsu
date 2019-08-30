/**@file  	    bIf_protocol.c
 * @brief   	动换监控 B接口协议实现
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

#include "soapH.h"
#include "stdsoap2.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SCServiceSoapBinding.nsmap"
#include "public.h"
#include "bIf_protocol.h"


/**
 * @brief B接口soap客户端线程 \n
 *
 */
void * bIfClientThread(void *arg)
{
	struct soap *fsucSoap = soap_new();
	fsucSoap->send_timeout = fsucSoap->recv_timeout = 5;	//soap发送、接收超时
	fsucSoap->transfer_timeout = 30;		//soap消息传输超时

	soap_set_namespaces(fsucSoap, namespaces);
	soap_set_mode(fsucSoap, SOAP_IO_CHUNK);

	while(1)
	{

		DebugPrint("bIfClientThread\n");
		sleep(300);
	}


	return 0;
}


struct soap *wh_soap_init()
{
	struct soap *fsucSoap = soap_new();
	fsucSoap->send_timeout = fsucSoap->recv_timeout = 5;	//soap发送、接收超时
	fsucSoap->transfer_timeout = 30;		//soap消息传输超时

	soap_set_namespaces(fsucSoap, namespaces);
	soap_set_mode(fsucSoap, SOAP_IO_CHUNK);

	return fsucSoap;
}


/**
* @brief B接口soap服务器线程 \n
* 设置服务器soap参数，启动服务器监听
*/
void * bIfServerThread(void *arg)
{
	SOAP_SOCKET iSocket_master, iSocket_slave;
	struct soap *fsusSoap = soap_new();	//新建soap

	fsusSoap->send_timeout = fsusSoap->recv_timeout = 5;	//soap发送、接收超时
	fsusSoap->transfer_timeout = 30;				//soap消息传输超时
//	soap_set_namespaces(fsusSoap, namespaces);	//设置XML消息命名空间列表
//	soap_set_mode(fsusSoap, SOAP_C_NOIOB);

	// saop 服务器程序 8080为端口号，最后一个参数不重要。
	iSocket_master = soap_bind(fsusSoap, NULL, 8080, 100); 	//绑定到相应的IP地址和端口（）NULL指本机，然后监听
	if (iSocket_master< 0)                               	//绑定出错
	{
		soap_print_fault(fsusSoap, stderr);
		exit(-1);
	}
	printf("SoapBind success,the master socket number is:%d\n",iSocket_master); //绑定成功返回监听套接字

	while(1)
	{
		iSocket_slave = soap_accept(fsusSoap);	//收到套接字连接
		if(iSocket_slave < 0)
		{
			soap_print_fault(fsusSoap, stderr);
			exit(-1);
		}
		//客户端的IP地址
		fprintf(stderr,"Accepted connection fromIP= %d.%d.%d.%d socket = %d \n",	\
				((fsusSoap->ip)>>24)&&0xFF,((fsusSoap->ip)>>16)&0xFF,((fsusSoap->ip)>>8)&0xFF,(fsusSoap->ip)&0xFF,(fsusSoap->socket));
		printf("Socket connect success,the slave socket number is:%d\n",iSocket_slave);
		soap_serve(fsusSoap);
		printf("soap_serve end...");
		soap_end(fsusSoap);	//服务器出错才到这一步
	}
	soap_done(fsusSoap);
	soap_free(fsusSoap);

    return 0;
}

/**
* @brief soap服务器用户数据处理函数
* @param[in] *_xmlData			接收客户端的xml格式字符串
* @param[out] **_invokeReturn	服务器要回复大数据
* @return 	SOAP_OK or error code
*/
SOAP_FMAC5 int SOAP_FMAC6 ns1__invoke(struct soap* soap, char *_xmlData, char **_invokeReturn)
{
	printf("ns1__invoke _xmlData\n");
	printf("%s\n", _xmlData);

	char xmlData[500];
	memset(xmlData, 0, sizeof(xmlData));
	strcat(xmlData, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	strcat(xmlData, "<Response>\n");
	strcat(xmlData, "<PK_Type>\n");
	strcat(xmlData, "<Name>LOGIN_ACK</Name>\n");
	strcat(xmlData, "</PK_Type>\n");
	strcat(xmlData, "<Info>\n");
	strcat(xmlData, "<RightLevel/>\n");
	strcat(xmlData, "</Info>\n");
	strcat(xmlData, "</Response>");

//	*_invokeReturn = xmlData;
	*_invokeReturn = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><Response><PK_Type><Name>LOGIN_ACK</Name></PK_Type><Info><RightLevel/></Info></Response>";
//	*_invokeReturn = "hello";

	printf("_invokeReturn _xmlData\n");
	printf("%s\n", *_invokeReturn);
	return SOAP_OK;
}




















