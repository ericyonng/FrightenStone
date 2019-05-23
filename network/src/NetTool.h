#ifndef NET_TOOL_H_
#define NET_TOOL_H_

#pragma once

NAMESPACE_BEGIN(gtool)


bool GetIPByDomain(
	P_IN const char *&domain												//域名或主机名
	, P_IN const char *& service											/*端口号“80”等、服务名 如"ftp", "http"等*/
	, P_OUT char *& ip														//输出
	, P_OUT const I32 szIp													//输出缓存的大小
	, const I32& nNetCardCount = 0											//若获取的是本地的地址，为选择网址列表的某一个网址
	, const I32& eFlags = NET_SYS::AI_FLAGS_TYPE_AI_PASSIVE					//NET_SYS::AI_FLAGS_TYPE 各个位的组合 默认AI_PASSIVE 即用于bind绑定 不设置则用于connect
	, NET_SYS::SOCK_TYPE eSockType = NET_SYS::SOCK_TYPE_SOCK_STREAM			//默认流数据
	, NET_SYS::FAMILY_TYPE eFamily = NET_SYS::FAMILY_TYPE_AF_INET			//默认ipv4
	, NET_SYS::PROTOCOL_TYPE eProtocol = NET_SYS::PROTOCOL_TYPE_IPPROTO_IP //默认任意协议 即ip协议
);


//获取对象地址信息 0表示成功其他值为错误信息
I32 GetPeerAddr(const U64 sSocket, P_OUT char *&ip, const U64 szIp, P_OUT U16& port);


//转换为网络字节序
bool FillTcpAddrInfo(P_OUT SOCKADDR_IN& AddrObj, P_IN const char *ip, P_IN  const U16 port, P_IN  const U16 family);

//转换为主机信息
bool GetAddrInfoFromNetInfo(P_IN const SOCKADDR_IN& AddrObj, P_OUT char *& ip, P_OUTSIZE const U64 szip, P_OUT U16& port);

bool LoadSocketEnvLib();
bool UnLoadSocketEnvLib();
bool IsLoadSocketEnvLib();

bool SetSocketNoBlockMode(SOCKET& rSocket, bool bNoBlock = true);

bool IsDetectTimeOut(SOCKET& rSocket, fd_set&rReadableSet, fd_set&rWritableSet, long tv_sec, long tv_usec, bool bReadableDetect = true, bool bWriteableDetect = false, int *pErrOut = NULL, bool bSetOneAtLeast = false, bool bInfiniteWaiting = false);

//设置socket缓冲区大小
bool SetSocketCacheSize(SOCKET& rSocket, SOCKET_CACHE_TYPE eType, const I64 nSize);

//获取socket缓冲区大小
bool GetSocketCacheSize(SOCKET& rSocket, SOCKET_CACHE_TYPE eType, I64& nSize);

NAMESPACE_END(gtool)

#endif

