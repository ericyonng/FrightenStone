/*
==========================================================================

Purpose:

* 这个类CIOCPModel是本代码的核心类，用于说明WinSock服务器端编程模型中的
完成端口(IOCP)的使用方法，并使用MFC对话框程序来调用这个类实现了基本的
服务器网络通信的功能。

* 其中的PER_IO_DATA结构体是封装了用于每一个重叠操作的参数
PER_HANDLE_DATA 是封装了用于每一个Socket的参数，也就是用于每一个完成端口的参数

* 详细的文档说明请参考 http://blog.csdn.net/PiggyXP

Notes:

* 具体讲明了服务器端建立完成端口、建立工作者线程、投递Recv请求、投递Accept请求的方法，
所有的客户端连入的Socket都需要绑定到IOCP上，所有从客户端发来的数据，都会实时显示到
主界面中去。

Author:

* PiggyXP【小猪】

Date:

* 2009/10/04

==========================================================================
*/

#ifndef IOCPMODEL_H_
#define IOCPMODEL_H_


#pragma once

#include<BaseCode/NetworkServiceApi.h>

typedef struct _RW_CONTEXT PER_RW_CONTEXT, *PPER_RW_CONTEXT;
typedef struct _PER_SOCKET_CONTEXT PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;

//
typedef const void* (*pfnGetBusinessLogicMainThread)();

class CThreadPool;
class CIocpTask;
class CSenderThread;
enum NET_IO_ERR_TYPE;


//判活由协议层的心跳包来判活，底层不判活
// CIOCPModel类
NAMESPACE_BEGIN(gtool)

class NETWORK_SERVICE_API CIOCPModelMgr
{
	friend class CIocpTask;

	NO_COPY_NO_MOVE(CIOCPModelMgr);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CIOCPModelMgr);

	//模块操作
public:	
	bool Init();	// 启动服务器	
	void Fini();	//	停止服务器
	bool IsDestroy() const;	//是否消耗
	bool IsMgrInit()const;
	//busines api地址
	void SetfnGetBusinessLogicMainThread(const void *pfn) {
		pfnGetBusinessLogicMainThread p = (pfnGetBusinessLogicMainThread)pfn;
		m_pfnGetBusinessLogicMainThread = p;
		ASSERTNET(m_pfnGetBusinessLogicMainThread);
	}
	const void *GetBusinessLogicMainThread()
	{
		CHECKF(m_pfnGetBusinessLogicMainThread);
		return m_pfnGetBusinessLogicMainThread;
	}

	//cp工作线程同步操作
public:
	bool Lock();	//iocp公共资源锁
	bool IsLock();
	bool Unlock();
	bool Wait(const unsigned long ulMilliseconds = INFINITE);
	bool SinalOne();
	
	//查询socket
	PER_SOCKET_CONTEXT* QueryClientContext(const SOCKET_IDD idSocket);

	//外部接口
public:
	_RW_CONTEXT * AddData(const SOCKET_IDD sDest, const void *pData, const U64 nSize, NET_IO_ERR_TYPE& errOut, bool bPutOutErrlog = false);
	bool SendData(const SOCKET_IDD sDest, _RW_CONTEXT& rDataContext, NET_IO_ERR_TYPE& errOut, bool bPutOutErrlog=false);	//线程不安全需要加锁

//	bool RecvData(const SOCKET_IDD sDest, void *& pData, U64& nRecvSize);	//pData将返回堆区创建的指针需要用智能指针接 nRecvSize 返回接收的数据大小
	bool Close(const SOCKET_IDD idSocket);
	void GetBindAddrStr(void *pBuffer, const U16 u16SizeBuffer) const;
	const void *GetSenderThread() const { ASSERTEX(m_pSenderThread); return m_pSenderThread; }
		
	//辅助
public:	
	void SetPort(const U16& nPort) { m_nPort = nPort; }		// 设置监听端口
	void SetIp(const char * ip) { m_strIP = ip; }

	//iocp初始化
protected:
	bool InitializeIOCP();		// 初始化IOCP
	bool PreBindListenEnv();	//准备服务器监听环境等
	void FreeResource();		// 最后释放资源
	bool AssociateWithIOCP(PER_SOCKET_CONTEXT *pContext);	// socket关联IOCp															
	bool HandleError(PER_SOCKET_CONTEXT *pSocketContext, OVERLAPPED* & pOverlapped, const DWORD& dwErr);	// 处理完成端口上的错误

	//网络操作
protected:
	bool CreateSocket(_PER_SOCKET_CONTEXT& rSocketContext, bool bSetNoBlock = true);	//创建套接字
	bool DoAccpet(PER_RW_CONTEXT* pIoContext);	// 在有客户端连入的时候，进行处理																					
	bool DoRecv(PER_SOCKET_CONTEXT* pSocketContext, PER_RW_CONTEXT* pIoContext, const U64& nRecvByte, bool& bRecvNetData);	// 在有接收的数据到达的时候，进行处理
	//bool DoConnect(PER_SOCKET_CONTEXT* pSocketContext, PER_RW_CONTEXT* pIoContext, const U64& nRecvByte);	//连接完成
	bool DoSend(PER_SOCKET_CONTEXT& rSocketContext, PER_RW_CONTEXT& rIoContext, const U64& nSendByte);
	bool PostAccept(PER_SOCKET_CONTEXT& rSocketContext, PER_RW_CONTEXT* pIOContext = NULL);	//投递Accept请求	
	bool PostRecv(PER_RW_CONTEXT* pIoContext);	// 投递接收数据请求
	bool PostSend(PER_RW_CONTEXT& rIoContext);	// 投递发送数据请求
	//bool PostConnect(PER_RW_CONTEXT* pIoContext);	//投递连接请求
												
	bool AddToSocketMap(PER_SOCKET_CONTEXT *pSocketContext);	// 将客户端的相关信息存储到数组中																
	void RemoveFromSocketMap(PER_SOCKET_CONTEXT *pSocketContext);	// 将客户端的信息从数组中移除
	void ClearSocketContainer();						// 清空客户端信息
//	bool IsSocketAlive(PER_SOCKET_CONTEXT& rSocketContext);				// 判断客户端Socket是否已经断开	

												//套接字超时监控
//	bool DelayMonitorSocket(long tv_sec, long tv_usec, bool bReadableDetect = true, bool bWriteableDetect = false, int *pErrOut = NULL, bool bSetOneAtLeast = false, bool bInfiniteWaiting = false);

	//辅助
protected:
	bool DoAcceptPostNewRecvSock(const SOCKET sSocket, const SOCKADDR_IN* pClientAddr);	//doaccept 投递recv

private:
	boost::atomic_bool			m_bIsInitMgr{false};					//是否初始化完毕
	HANDLE                      m_hIOCompletionPort = NULL;    // 完成端口的句柄
	static boost::atomic_bool	m_bDestroy;			//销毁iocp
	CFmtIOString                 m_strIP;                       // 服务器端的IP地址
	U16                          m_nPort;                       // 服务器端的监听端口

	boost::atomic<I32>			m_CurWorkThreadNum{ 0 };		//当前工作线程数
	CSmartPtr<CThreadPool>		m_pThreadPool = NULL;			//线程池
	CSmartPtr<CDataLocker>		m_pContextLocer = NULL;         // 用于Worker线程同步的互斥量
      
	typedef gtool::CLitPtrMap<PER_SOCKET_CONTEXT, U64, DEL_WAY_DELETE> NetPerSocketContextMap;	//key:PER_SOCKET_CONTEXT ADDR SOCKET_IDD, val:PER_SOCKET_CONTEXT
	CSmartPtr<NetPerSocketContextMap>	m_pClientSocketMap = NULL;		// 客户端Socket的Context信息  
	CSmartPtr<PER_SOCKET_CONTEXT>		m_pSvrListenSocket;             // 用于监听的Socket的Context信息


//	CSmartPtr<SocketMsgInfoList> m_pSocketMsgInfoRecvList;		//iocp2ext的 消息队列

	//函数指针
	void *						m_lpfnAcceptEx;                // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	void *					    m_lpfnGetAcceptExSockAddrs;


private:
// 	CSmartPtr<CDataLocker>		m_pIocp2ExternalLock = NULL;		//数据状态信号	iocp代理外部数据接收
// 	CSmartPtr<CDataLocker>		m_pIocp2ExternalLockIocpGuard = NULL;		//iocp端守护 //iocp向外部通知数据到来时外部或者iocp的反馈

	boost::atomic_bool			m_bExternalRecvFromIocpAwake{ false };
	boost::atomic_bool			m_bExternalRecvFinish{ false };
	boost::atomic<pfnGetBusinessLogicMainThread> m_pfnGetBusinessLogicMainThread{ NULL };
	CSenderThread *				m_pSenderThread = NULL;				//不管理
	boost::atomic_bool				m_bIsThreadWork{ false };
};

//LPFN_ACCEPTEX
//LPFN_GETACCEPTEXSOCKADDRS
NAMESPACE_END(gtool)

typedef NETWORK_SERVICE_API gtool::CSingleton<gtool::CIOCPModelMgr> CIOCPModelInstance;
extern NETWORK_SERVICE_API gtool::CIOCPModelMgr *g_pIocpMgr;
#define IocpMgr  (ASSERT(g_pIocpMgr), *g_pIocpMgr)

#endif



