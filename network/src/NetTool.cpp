#include "stdafx.h"
#include <Net/NetTool.h>


NAMESPACE_BEGIN(gtool)



bool GetIPByDomain(
	P_IN const char *&domain												//域名或主机名
	, P_IN const char *& service											/*端口号“80”等、服务名 如"ftp", "http"等*/
	, P_OUT char *& ip														//输出
	, P_OUT const I32 szIp													//输出缓存的大小
	, const I32& nNetCardCount											//若获取的是本地的地址，为选择网址列表的某一个网址
	, const I32& eFlags				//NET_SYS::AI_FLAGS_TYPE 各个位的组合 默认AI_PASSIVE 即用于bind绑定 不设置则用于connect
	, NET_SYS::SOCK_TYPE eSockType		//默认流数据
	, NET_SYS::FAMILY_TYPE eFamily			//默认ipv4
	, NET_SYS::PROTOCOL_TYPE eProtocol //默认任意协议 即ip协议
)
{
	CHECKF_NL(domain || service);	//不可同时为NULL

									// 获得本机主机名+
	struct addrinfo hints = { 0 };
	struct addrinfo *res = NULL, *cur = NULL;
	struct sockaddr_in *addr = NULL;
	BUFFER128 szBuffer = { 0 };

	//获取地址的参数设置
	hints.ai_family = eFamily;
	hints.ai_flags = eFlags;
	hints.ai_protocol = eProtocol;
	hints.ai_socktype = eSockType;

	CHECKF_NL(getaddrinfo(domain, service, &hints, &res) == 0);
	I32 nCount = -1;
	for (cur = res; cur != NULL; cur = cur->ai_next)
	{
		addr = (struct sockaddr_in *)cur->ai_addr;
		IF_NOT_NL(addr)
			continue;

		++nCount;

		szBuffer[0] = 0;
		sprintf(szBuffer, "%d.%d.%d.%d"
			, (*addr).sin_addr.S_un.S_un_b.s_b1
			, (*addr).sin_addr.S_un.S_un_b.s_b2
			, (*addr).sin_addr.S_un.S_un_b.s_b3
			, (*addr).sin_addr.S_un.S_un_b.s_b4);

		if (nNetCardCount == nCount)
			break;
	}

	//释放资源
	freeaddrinfo(res);

	//无网卡？
	auto nLen = strlen(szBuffer);
	CHECKF_NL(nLen != 0);
	CHECKF_NL(szIp > nLen);

	strcpy(ip, szBuffer);

	return strlen(ip) > 0;
}


//获取对象地址信息 0表示成功其他值为错误信息
I32 GetPeerAddr(const U64 sSocket, P_OUT char *&ip, const U64 sizeIp, P_OUT U16& port)
{
	CHECKF_NL(sSocket&&sSocket != INVALID_SOCKET && sizeIp != 0);

	//定义
	struct sockaddr_in dstadd_in;
	ZERO_CLEAN_OBJ(dstadd_in);
	socklen_t  len = sizeof(dstadd_in);

	IF_NOT_NL(getpeername(sSocket, (struct sockaddr*)&dstadd_in, &len) < 0) {
		return WSAGetLastError();
	}

	IF_NOT_NL(inet_ntop(dstadd_in.sin_family, &dstadd_in.sin_addr.s_addr, ip, sizeIp) != NULL)
		return -1;

	port = ntohs(dstadd_in.sin_port);

	return 0;
}


//转换为网络字节序
bool FillTcpAddrInfo(P_OUT SOCKADDR_IN& AddrObj, P_IN const char *ip, P_IN  const U16 port, P_IN  const U16 family)
{
	CHECKF_NL(ip&&strlen(ip) > 0);

	ZERO_CLEAN_OBJ(AddrObj);
	AddrObj.sin_family = family;
	CHECKF_NL(inet_pton(AddrObj.sin_family, ip, &AddrObj.sin_addr.s_addr) > 0);
	AddrObj.sin_port = htons(port);

	return true;
}

//转换为主机信息
bool GetAddrInfoFromNetInfo(P_IN const SOCKADDR_IN& AddrObj, P_OUT char *& ip, P_OUTSIZE const U64 szip, P_OUT U16& port)
{
	CHECKF_NL(inet_ntop(AddrObj.sin_family, &AddrObj.sin_addr.s_addr, ip, szip) != NULL);
	port = ntohs(AddrObj.sin_port);

	return true;
}

static boost::atomic_bool	m_bInitEnvironment{ false };				//是否初始化网络环境

bool LoadSocketEnvLib()
{
	try
	{
		if (m_bInitEnvironment)
			return false;

		m_bInitEnvironment = true;

		//启动socket 初始化环境
		WSADATA wsaData;
		memset(&wsaData, 0, sizeof(WSADATA));
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);	//加载winsock DLL 每个进程调用一次就够了 否则无法使用socket
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		{
			WSACleanup();
			m_bInitEnvironment = false;
			return false;
		}

		IF_NOT(iResult == NO_ERROR)
		{
			m_bInitEnvironment = false;
			return false;
		}

	}
	catch (...)
	{
		m_bInitEnvironment = false;
		return false;
	}

	return true;
}

bool UnLoadSocketEnvLib()
{
	int iRet = NO_ERROR;
	try
	{
		if (!m_bInitEnvironment) return true;

		m_bInitEnvironment = false;

		if ((iRet = WSACleanup()) == SOCKET_ERROR)
		{
			if (WSACancelBlockingCall() == 0)//取消阻塞套接口
			{
				iRet = WSACleanup();
			}
		}

		IF_NOT(iRet == NO_ERROR)
		{
			m_bInitEnvironment = true;
			return false;
		}
	}
	catch (...)
	{
		iRet = -1;
	}


	//一个进程调用一次就够了若使用过程中被清除则会出现异常

	return iRet == NO_ERROR ? true : false;
}

bool IsLoadSocketEnvLib()
{
	return m_bInitEnvironment;
}

bool SetSocketNoBlockMode(SOCKET& rSocket, bool bNoBlock /*= true*/)
{
	CHECKF(IS_VALID_SOCKET_IDD(rSocket));

	//设置非阻塞模式
	MYULONG ul = (bNoBlock ? 1 : 0);
	if (0 != ioctlsocket(rSocket, FIONBIO, &ul))
	{
		return false;
	}

	return true;
}

//套接字超时监控
bool IsDetectTimeOut(SOCKET& rSocket, fd_set&rReadableSet, fd_set&rWritableSet, long tv_sec, long tv_usec, bool bReadableDetect, bool bWriteableDetect, int *pErrOut, bool bSetOneAtLeast, bool bInfiniteWaiting)
{
	//变量定义
	ZERO_CLEAN_OBJ(rReadableSet);
	ZERO_CLEAN_OBJ(rWritableSet);

	//清理
	FD_ZERO(&rReadableSet);
	FD_ZERO(&rWritableSet);
	FD_SET(rSocket, &rReadableSet);
	FD_SET(rSocket, &rWritableSet);

	//超时监控参数初始化
	timeval timeout;
	ZERO_CLEAN_OBJ(timeout);
	timeout.tv_sec = tv_sec;	//监控秒
	timeout.tv_usec = tv_usec;	//监控微妙

								//监听套接字状态
	int ret = SOCKET_ERROR;
	if (bInfiniteWaiting) { //永久阻塞
		ret = select(rSocket + 1, &rReadableSet, &rWritableSet, NULL, NULL);	//0表示超时，否则返回SOCKET_ERROR 当返回为-1时，所有描述符集清0。 当返回为正数时，表示已经准备好的描述符数。
	}
	else {
		ret = select(rSocket + 1, &rReadableSet, &rWritableSet, NULL, &timeout);	//0表示超时，否则返回SOCKET_ERROR 当返回为-1时，所有描述符集清0。 当返回为正数时，表示已经准备好的描述符数。
	}

	//出错带出
	if (pErrOut) *pErrOut = ret;
	IF_NOT(ret != SOCKET_ERROR) {
		FD_CLR(rSocket, &rReadableSet);
		FD_CLR(rSocket, &rWritableSet);
		return true;
	}

	//定义变量
	bool bTimeOut = false, bReadTimeOut = false, bWriteTimeOut = false;

	// 判断socket句柄是否可读  
	if (!FD_ISSET(rSocket, &rReadableSet)) bReadTimeOut = true;

	// 判断socket句柄是否可写  
	if (!FD_ISSET(rSocket, &rWritableSet)) bWriteTimeOut = true;

	//清除状态
	FD_CLR(rSocket, &rReadableSet);
	FD_CLR(rSocket, &rWritableSet);

	//监听状态导出
	if (bSetOneAtLeast) {
		bTimeOut = (bReadTimeOut || bWriteTimeOut);
	}
	else {
		if (bReadableDetect && !bWriteableDetect) {
			bTimeOut = (bReadTimeOut);
		}
		else if (!bReadableDetect && bWriteableDetect) {
			bTimeOut = (bWriteTimeOut);
		}
		else {
			bTimeOut = (bReadTimeOut && bWriteTimeOut);
		}
	}

	return bTimeOut;
}

bool SetSocketCacheSize(SOCKET& rSocket, SOCKET_CACHE_TYPE eType, const I64 nSize)
{
	CHECKF(IS_VALID_SOCKET_IDD(rSocket));
	switch (eType)
	{
	case SOCKET_CACHE_TYPE_RECV:
		{
			auto nRet = setsockopt(rSocket, SOL_SOCKET, SO_RCVBUF, (char *)(&nSize), sizeof(nSize));
			auto nErr = WSAGetLastError();
			IF_NOT(nRet == 0)
			{
				LOGERR("SetSocketCacheSize SOCKET_CACHE_TYPE_RECV nSize[", nSize, "]. err[", nErr, "].");
				return false;
			}
		}
		break;
	case SOCKET_CACHE_TYPE_SEND:
		{
			auto nRet = setsockopt(rSocket, SOL_SOCKET, SO_SNDBUF, (char *)(&nSize), sizeof(nSize));
			auto nErr = WSAGetLastError();
			IF_NOT(nRet == 0)
			{
				LOGERR("SetSocketCacheSize SOCKET_CACHE_TYPE_SEND nSize[", nSize, "]. err[", nErr, "].");
				return false;
			}
		}
		break;
	default:
		{
			ASSERT(false);
			LOGERR("SetSocketCacheSize unknown type[", eType, "]. nSize[" , nSize, "].");
		}
		return false;
	}

	return true;
}

bool GetSocketCacheSize(SOCKET& rSocket, SOCKET_CACHE_TYPE eType, I64& nSize)
{
	CHECKF(IS_VALID_SOCKET_IDD(rSocket));
	switch (eType)
	{
	case SOCKET_CACHE_TYPE_RECV:
		{
			I32 nSizeLen = sizeof(nSize);
			auto nRet = getsockopt(rSocket, SOL_SOCKET, SO_RCVBUF, (char *)(&nSize), &nSizeLen);
			auto nErr = WSAGetLastError();
			IF_NOT(nRet == 0)
			{
				LOGERR("GetSocketCacheSize SOCKET_CACHE_TYPE_RECV err[", nErr, "].");
				return false;
			}
		}
		break;
	case SOCKET_CACHE_TYPE_SEND:
		{
			I32 nSizeLen = sizeof(nSize);
			auto nRet = getsockopt(rSocket, SOL_SOCKET, SO_SNDBUF, (char *)(&nSize), &nSizeLen);
			auto nErr = WSAGetLastError();
			IF_NOT(nRet == 0)
			{
				LOGERR("GetSocketCacheSize SOCKET_CACHE_TYPE_SEND err[", nErr, "].");
				return false;
			}
		}
		break;
	default:
		{
			ASSERT(false);
			LOGERR("GetSocketCacheSize unknown type[", eType, "].");
		}
		return false;
	}

	return true;
}


NAMESPACE_END(gtool)






