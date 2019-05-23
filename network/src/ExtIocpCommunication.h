#ifndef EXT_IOCP_COMMUNICATION_H_
#define EXT_IOCP_COMMUNICATION_H_

#pragma once

#pragma MEM_ALIGN_BEGIN(1)

NAMESPACE_BEGIN(EXTIOCP_NETDATA)

//外部与iocp通讯协议
struct ExtIocpNetData
{
	SOCKET_IDD idSocket;					//socketid
	TRAN_PROTOCOL::TRAN_TYPE nTranType;		//协议类型
	NET_DATA_SIZE nszLen;					//数据长度
	char rStream[0];						//数据
};

NAMESPACE_END(EXTIOCP_NETDATA)

#pragma MEM_ALIGN_END(1)

#endif 

