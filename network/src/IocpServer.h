#ifndef _IocpServer_h_
#define _IocpServer_h_
#include <WinSock2.h>
#include <Windows.h>
#include <io.h>
#include <thread>
#include <iostream>
#include "connection.h"
#include "callbacks.h"
#include "SocketExFnsHunter.h"
#include <atomic>
#pragma comment(lib,"ws2_32.lib")

typedef const void* (*pfnGetBusinessLogicMainThread)();

class CSenderThread;
class CNetDispatch;

class IocpServer :public CallbacksExecutor
{
	friend class Workers;
	NO_COPY(IocpServer);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(IocpServer);

public:
	bool Init();
	void Fini();

	bool AsyncRead(const Connection* conn);

	bool AsyncWrite(const Connection* conn, const void* data, const std::size_t size);

	bool Lock();
	bool Unlock();

	//辅助函数
public:
	bool IsDestroy() const;	//是否销毁
	bool IsInit() const;	//是否初始化
	void SetPort(const U16& nPort) { m_nPort = nPort; }		// 设置监听端口
	void SetIp(const char * ip) { m_strIP = ip; }

	auto& GetIocpPort() {	return _completion_port; }
	void AddClientCount() { ++_client_count; }
	void DeductClientCount(const SOCKET_IDD sSocket);
	void AddMsgCount() { ++_msg_count; }
	void DeductMsgCount() { --_msg_count; }

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

	void GetBindAddrStr(void *pBuffer, const U16 u16SizeBuffer) const;
	const void *GetSenderThread() const { ASSERTEX(m_pSenderThread); return m_pSenderThread; }
	bool HandleError(const Connection* conn, OVERLAPPED* & pOverlapped, const DWORD& dwErr);

protected:
	bool InitWorkers(const I32 nWorkNum);
	bool WinSockInit();

	bool InitListenSocket();

	bool Accept(bool bSvrListen = false );

	bool Bind();

	bool Listen(unsigned int nListen);

protected:
	gtool::CSmartPtr<CDataLocker>		m_pSvrLocker = NULL;         // 用于Worker线程同步的互斥量
	gtool::CFmtIOString                 m_strIP;                       // 服务器端的IP地址
	U16                          m_nPort;                       // 服务器端的监听端口
	std::atomic_bool m_bInit{ false };			//是否初始化
	std::atomic<I32> m_nCurTaskNum{ 0 };	//当前任务线程数
	static std::atomic_bool m_bDestroy;		//是否销毁
	HANDLE _completion_port = NULL;			//完成端口
	SOCKET _socketSvrListen = INVALID_SOCKET;	//监听socket
	std::atomic_int _client_count{ 0 };		//客户端数
	std::atomic_int _msg_count{ 0 };		//消息量
	LPFN_ACCEPTEX _acceptex_func = NULL;	//acceptex
	gtool::CSmartPtr<CThreadPool> m_pThreadPool;			//工作线程池

	boost::atomic<pfnGetBusinessLogicMainThread> m_pfnGetBusinessLogicMainThread{ NULL };	//业务逻辑模块线程
	gtool::CLitUnmanagedPtr<CSenderThread>				m_pSenderThread;				//不管理
	boost::atomic_bool									m_bIsThreadWork{ false };

	typedef gtool::CLitPtrMap<Connection, SOCKET_IDD, DEL_WAY_DELETE> ConnectionMap;	//客户端链接
	gtool::CSmartPtr<ConnectionMap> m_pConnectionMap;
	gtool::CSmartPtr<CNetDispatch>	m_pNetDispatch;
};

typedef NETWORK_SERVICE_API gtool::CSingleton<IocpServer> IocpServerInstance;
extern NETWORK_SERVICE_API IocpServer *g_pIocpMgr;
#define IocpMgr  ( g_pIocpMgr=IocpServerInstance::GetInstance(), ASSERT(g_pIocpMgr), *g_pIocpMgr)

#endif