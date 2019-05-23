#ifndef NET_ENUM_H_
#define NET_ENUM_H_

#pragma once

//////////////////////////////////////////////////////////////////
// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE
{
	NULL_POSTED=0,                       // 用于初始化，无意义
	ACCEPT_POSTED,                     // 标志投递的Accept操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
//	CONNECT_POSTED,						//连接完成

}OPERATION_TYPE;

//套接字机器类型
enum SOCKET_MACHINE_TYPE
{
	SOCKET_MACHINE_TYPE_SVR=0,	//服务器套接字
	SOCKET_MACHINE_TYPE_CLIENT,	//客户端套接字
};

enum SOCKET_STATUS_TYPE
{
	SOCKET_STATUS_TYPE_CLOSED=0,	//套接字关闭
	SOCKET_STATUS_TYPE_RECVED=1,	//套接字接收数据
};

enum SOCKET_CACHE_TYPE
{
	SOCKET_CACHE_TYPE_NONE=0,	//无效
	SOCKET_CACHE_TYPE_RECV,		//recv缓冲区
	SOCKET_CACHE_TYPE_SEND,		//send缓冲区
	SOCKET_CACHE_TYPE_END,
};

//网络数据缓冲类型
enum NET_CACHE_TYPE
{
	NET_CACHE_TYPE_NONE=0,
	NET_CACHE_TYPE_NORMAL = 1,		//正常包
	NET_CACHE_TYPE_REPLY = 2,		//回复包
	NET_CACHE_TYPE_END,
};

#endif

