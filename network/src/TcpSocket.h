#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_
#pragma once
#include <BaseCode/NetworkServiceApi.h>
#include <Net/SocketProtocol.h>

//操作成功返回值为0; 否则返回值为SOCKET_ERROR，可以通过调用WSAGetLastError获取错误代码。

//在一个多线程的环境下，WSACleanup()中止了Windows Sockets在所有线程上的操作.
//定义地址信息结构体


//客户端： 单服务器使用阻塞式，多服务器应该分配多个socket使用非阻塞式
//createsocket接口
//connect
//send
//recv

//服务端 非阻塞式
//使用iocp
//只有常规的send和recv接口

class CSenderThread;

class NETWORK_SERVICE_API CTcpServerMgr
{
	NO_COPY_NO_MOVE(CTcpServerMgr);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CTcpServerMgr);

	//接口功能
public:
	bool Start();	//启动iocp
	bool Send(const SOCKET_IDD idSocket, const void *&pData, const U64 u64DataLen, TRAN_PROTOCOL::TRAN_TYPE eTranType = TRAN_PROTOCOL::TRAN_TYPE_MSG);	//发送数据
	bool Recv();	//pData输出为堆区创建内存需要被智能指针持有
	bool Close(const SOCKET_IDD idSocket);
	bool FinishIocp();
	bool IsInitIocp();
	bool IsInit() const { return m_bIsInit; }
	bool IsDestroy() const { return m_bIsDestroy; }

	//辅助功能
public:
	const void *GetSenderThread() const { ASSERTEX(m_pSenderThread); return m_pSenderThread; }

	//数据处理
protected:
	bool HandlerRecvData(SocketMsgInfo& rMsgInfo);

	//流量统计
protected:
	boost::atomic_bool	m_bIsInit{ false };
	static boost::atomic_bool	m_bIsDestroy;

	//一个线程用来 发送和接收iocp数据
	gtool::CSmartPtr<CThreadPool> m_pThreadPool = NULL;		//两条线程 recv一条，sender一条
	boost::atomic_bool				m_bIsThreadWork{ false };
	CSenderThread *				m_pSenderThread = NULL;				//不管理
};

typedef NETWORK_SERVICE_API gtool::CSingleton<CTcpServerMgr> CTcpServerMgrInstance;
extern NETWORK_SERVICE_API CTcpServerMgr *g_pTcpServerMgr;
#define TcpServerMgr  (ASSERT(g_pTcpServerMgr), *g_pTcpServerMgr)


////////////////////////////////////////////////////////////////

//IPC
#endif