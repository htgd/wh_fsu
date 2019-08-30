/* soapClient.c
   Generated by gSOAP 2.8.83 for sc.h

gSOAP XML Web services tools
Copyright (C) 2000-2018, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"
#ifdef __cplusplus
extern "C" {
#endif

SOAP_SOURCE_STAMP("@(#) soapClient.c ver 2.8.83 2019-07-04 00:58:17 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call_ns1__invoke(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *_xmlData, char **_invokeReturn)
{	if (soap_send_ns1__invoke(soap, soap_endpoint, soap_action, _xmlData) || soap_recv_ns1__invoke(soap, _invokeReturn))
		return soap->error;
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_send_ns1__invoke(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *_xmlData)
{	struct ns1__invoke soap_tmp_ns1__invoke;
	if (soap_endpoint == NULL)
		soap_endpoint = "http://127.0.0.1:8080/service/SCService";
	if (soap_action == NULL)
		soap_action = "";
	soap_tmp_ns1__invoke._xmlData = _xmlData;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
//	soap_set_version(soap, 1);
	soap->encodingStyle = "http://schemas.xmlsoap.org/soap/encoding/"; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_ns1__invoke(soap, &soap_tmp_ns1__invoke);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns1__invoke(soap, &soap_tmp_ns1__invoke, "ns1:invoke", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns1__invoke(soap, &soap_tmp_ns1__invoke, "ns1:invoke", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_recv_ns1__invoke(struct soap *soap, char **_invokeReturn)
{
	struct ns1__invokeResponse *soap_tmp_ns1__invokeResponse;
	if (!_invokeReturn)
		return soap_closesock(soap);
	*_invokeReturn = NULL;
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_ns1__invokeResponse = soap_get_ns1__invokeResponse(soap, NULL, "", NULL);
	if (!soap_tmp_ns1__invokeResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (_invokeReturn && soap_tmp_ns1__invokeResponse->_invokeReturn)
		*_invokeReturn = *soap_tmp_ns1__invokeResponse->_invokeReturn;
	return soap_closesock(soap);
}

#ifdef __cplusplus
}
#endif

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapClient.c */