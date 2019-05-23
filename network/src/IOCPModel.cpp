#include "StdAfx.h"
//#include <Net/NetConfig.h>
#include <Net/IOCPModel.h>
#include <BaseCode/Win/ThreadPool.h>
#include<BaseCode/TaskBase.h>
#include <thread>
#include <iostream>
#include <BaseCode/enumtype.h>
#include <Net/Sender.h>



#define WORKER_THREADS_PER_PROCESSOR	8		// 每一个处理器上产生多少个线程(为了最大限度的提升服务器性能，详见配套文档)
#define MAX_POST_ACCEPT					10		// 同时投递的Accept请求的数量(这个要根据实际的情况灵活设置)
#define EXIT_CODE						NULL	// 传递给Worker线程的退出信号
#define JUDGE_SOCKET_ALIVE_DELAY_TIME	100		//100微妙 判断客户端是否存活


#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}	// 释放指针宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}	// 释放句柄宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}	// 释放Socket宏

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//心跳

//任务
class CIocpTask :public CTaskBase
{
	NO_COPY(CIocpTask);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CIocpTask);

public:	
	virtual int Run();	//任务执行体

protected:
	bool NetHandler(gtool::CIOCPModelMgr& rIocpMgr, DWORD& rBytesTransfered, PER_SOCKET_CONTEXT& rSockteContext, PER_RW_CONTEXT& rRWContext, bool& bRecvData);	//网络操作
	bool NewPostAccept(gtool::CIOCPModelMgr& rIocpMgr);	//新的accept投递
};

CIocpTask::CIocpTask()
{
}

CIocpTask::~CIocpTask()
{
}

bool CIocpTask::NewPostAccept(gtool::CIOCPModelMgr& rIocpMgr)
{
	CHECKF(gtool::IsLoadSocketEnvLib());
	if (rIocpMgr.IsDestroy())
		return true;

	gtool::CSmartPtrC<PER_SOCKET_CONTEXT> pNewSocket = new PER_SOCKET_CONTEXT;
	CHECKF(pNewSocket);

	//重新投递
	CHECKF(rIocpMgr.PostAccept(*pNewSocket));

	CHECKF(rIocpMgr.AddToSocketMap(pNewSocket.pop()));

	return true;
}

//网络操作
bool CIocpTask::NetHandler(gtool::CIOCPModelMgr& rIocpMgr, DWORD& rBytesTransfered, PER_SOCKET_CONTEXT& rSockteContext, PER_RW_CONTEXT& rRWContext, bool& bRecvData)
{
	// 判断是否有客户端断开了
	if ((0 == rBytesTransfered)
		&& (RECV_POSTED == rRWContext.m_OpType
			|| SEND_POSTED == rRWContext.m_OpType))
	{
		//rBuffer[0] = 0;
		LOGNET("客户端", (const char *)(rSockteContext.GetAddrInfo()), ":", U16(rSockteContext.GetAddrInfo()), " 断开连接.");
	
		// 释放掉对应的资源
		rIocpMgr.RemoveFromSocketMap(&rSockteContext);

		//重新投递新的accpet
		IF_NOT(NewPostAccept(rIocpMgr)) {
			return false;
		}
	}
	else
	{
		switch (rRWContext.m_OpType)
		{
		case ACCEPT_POSTED:// Accept  
			{
				IF_NOT(rIocpMgr.DoAccpet(&rRWContext))
				{
					//移除
					rIocpMgr.RemoveFromSocketMap(&rSockteContext);

					//重新投递新的accpet
					IF_NOT(NewPostAccept(rIocpMgr)) {
						return false;
					}
				}
			}
			break;
		case RECV_POSTED:	// RECV
			{
				IF_NOT(rIocpMgr.DoRecv(&rSockteContext, &rRWContext, rBytesTransfered, bRecvData)) {
					ASSERTNET("DoRecv出错");
				}

				//返回数据
// 				BUFFER128 szBuffer = { 0 };
// 				sprintf(szBuffer, "hello！");
// 				NET_IO_ERR_TYPE eErrType = NET_IO_ERR_TYPE_NONE;
// 				IF_NOT(rIocpMgr.SendData(SOCKET_IDD(&rSockteContext), szBuffer, strlen(szBuffer) + 1, eErrType))
// 				{
// 					ASSERTNET("SendData出错");
// 					LOGNET("SendData出错 eErrType[", eErrType, "]");
// 				}
			}
			break;
		case SEND_POSTED:	// SEND
			{
				LOGNET("SEND_POSTED.");
				IF_NOT(rIocpMgr.DoSend(rSockteContext, rRWContext, rBytesTransfered)) {
					ASSERTNET("DoSend出错");
				}
			}
			break;
// 		case CONNECT_POSTED:
// 			{
// 				LOGNET("CONNECT_POSTED.");
// 				IF_NOT(rIocpMgr.DoConnect(&rSockteContext, &rRWContext, rBytesTransfered)) {
// 					ASSERTNET(!"DoConnect 出错");
// 				}
// 			}
// 			break;
		default:
			LOGNET("_WorkThread中的 pIoContext->m_OpType=[", rRWContext.m_OpType, "] 参数异常.");
			ASSERTNET(false);
			break;
		} //switch
	}//if

	return true;
}

int CIocpTask::Run()
{
	gtool::CIOCPModelMgr* pIOCPModel = (gtool::CIOCPModelMgr*)m_pArg;	//iocp模块
	CHECKF(pIOCPModel);

	if (pIOCPModel->IsDestroy())
		return 0;

	//变量定义
	OVERLAPPED           *pOverlapped = NULL;
	PER_SOCKET_CONTEXT   *pSocketContext = NULL;
	DWORD                dwBytesTransfered = 0;
	
	//线程进入
	++(pIOCPModel->m_CurWorkThreadNum);	//线程数目
	LOGNET("IOCP 工作者线程 ", std::this_thread::get_id(), "启动.");

	// 循环处理请求，知道接收到Shutdown信息为止
	while (!pIOCPModel->IsDestroy())
	{
		bool bReturn = GetQueuedCompletionStatus(
			pIOCPModel->m_hIOCompletionPort,		//cp对象
			&dwBytesTransfered,						//传输的字节数
			(PULONG_PTR)&pSocketContext,			//套接字等，结束不用时需要释放 pSocketContext的socket不一定与pOverlapped的socket对应上
			&pOverlapped,							//传入send recv的数据缓冲等重叠结构，在堆区 是pSocketContext中的缓冲数据上下文 socket主要看pOverlapped
			INFINITE);

		if(!pSocketContext || !pOverlapped)
			continue;

		// 如果收到的是退出标志，则直接退出
		if (EXIT_CODE == (DWORD)pSocketContext)
			break;

		//临界区
		ASSERTNET(pIOCPModel->Lock());

		// 判断套接字是否出现了错误
		if (!bReturn)
		{
			DWORD dwErr = GetLastError();
			if (!pIOCPModel->HandleError(pSocketContext, pOverlapped, dwErr))
			{				
				PER_RW_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_RW_CONTEXT, m_Overlapped);
				
				char *ip = "";
				U16 u16Port = 0;
				if (pIoContext)
				{
					ip = const_cast<char *>(pIoContext->GetIp());
					u16Port = pIoContext->GetPort();
				}

				LOGNET("套接字出错.", ip, ":", u16Port, "dwErr[", dwErr, "]");

				// 关闭客户端
				pIOCPModel->RemoveFromSocketMap(pSocketContext);	//移除socket

				//重新投递新的accpet
				IF_NOT(NewPostAccept(*pIOCPModel)){		//重新投递accept
					pIOCPModel->Unlock();
					break;
				}
			}

			pIOCPModel->Unlock();
			continue;
		}

		// 读取传入的参数
		PER_RW_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_RW_CONTEXT, m_Overlapped);	//系统填充的缓冲上下文
		IF_NOT(pIoContext)
		{
			pIOCPModel->Unlock();
			continue;
		}			
		
		//网络操作
		bool bRecvData = false;
		IF_NOT(NetHandler(*pIOCPModel, dwBytesTransfered, *pSocketContext, *pIoContext, bRecvData))
		{
			ASSERTNET(pIOCPModel->Unlock());
			break;
		}

		ASSERTNET(pIOCPModel->Unlock());

		//通知接收数据
		if (bRecvData)
		{
		}

	}//while

	//线程退出
	--(pIOCPModel->m_CurWorkThreadNum);
	if (pIOCPModel->m_CurWorkThreadNum <= 0)
	{
		ASSERTNET(pIOCPModel->Lock());
		pIOCPModel->SinalOne();
		ASSERTNET(pIOCPModel->Unlock());
	}
		

	LOGNET("IOCP 工作者线程 ", std::this_thread::get_id(),"退出.");
	return 0;
}

void CIocpTask::Cleans()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
gtool::CIOCPModelMgr *g_pIocpMgr = CIOCPModelInstance::GetInstance();
NAMESPACE_BEGIN(gtool)

//tcpsocket
boost::atomic_bool CIOCPModelMgr::m_bDestroy{ false };
CIOCPModelMgr::CIOCPModelMgr():
							m_hIOCompletionPort(NULL),
							m_strIP(DEFAULT_IP),
							m_nPort(DEFAULT_PORT),
							m_lpfnAcceptEx( NULL ),
							m_pSvrListenSocket( NULL )
{
	m_pThreadPool = CThreadPool::CreateNew();
	m_pContextLocer = CDataLocker::CreateNew(true);
	m_pClientSocketMap = NetPerSocketContextMap::CreateNew();
//	m_pClientContextList = NetPerSocketContextList::CreateNew();
	m_pSvrListenSocket = NULL;
}


CIOCPModelMgr::~CIOCPModelMgr(void)
{
	Cleans();
}

void CIOCPModelMgr::Cleans()
{
	// 确保资源彻底释放
	this->Fini();
}

//====================================================================================
//
//				    系统初始化和终止
//
//====================================================================================

bool CIOCPModelMgr::IsMgrInit() const
{
	return m_bIsInitMgr;
}

////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//	启动服务器
bool CIOCPModelMgr::Init()
{
	CHECKF(m_pThreadPool && m_pContextLocer);
	CHECKF(m_pClientSocketMap);
	
	ASSERTNET(m_pContextLocer->Lock());

	m_pThreadPool->SetThreadNumLimit(0, gtool::CpuCoreCount()*WORKER_THREADS_PER_PROCESSOR + 1);

	//初始化网络环境
	IF_NOT(gtool::LoadSocketEnvLib())
	{
		ASSERTNET(m_pContextLocer->UnLock());
		return false;
	}

	BUFFER128 szIpStr = { 0 };
	char *pStrIP = szIpStr;
	if (m_strIP.GetLength() == 0)
	{
		IF_NOT(gtool::GetLocalIp(pStrIP))
		{
			m_pContextLocer->UnLock();
			return false;
		}
	}
	else
	{
		_sntprintf(szIpStr, sizeof(szIpStr) - 1, "%s", m_strIP.c_str());
	}


	IF_NOT(strlen(szIpStr) > 0)
	{
		m_pContextLocer->UnLock();
		return false;
	}

	//地址
	m_strIP = szIpStr;


	// 初始化线程互斥量

	// 建立系统退出的事件通知
	//m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 初始化IOCP
	IF_NOT(InitializeIOCP())
	{
		ASSERTNET(m_pContextLocer->UnLock());
		return false;
	}

	//初始化工作线程
	// 初始化Socket
	IF_NOT(PreBindListenEnv())
	{
		ASSERTNET(m_pContextLocer->UnLock());
		return false;
	}

	//sender线程
	gtool::CSmartPtr<CSenderThread> pSenderThread = new CSenderThread(m_bIsThreadWork);
	CHECKF(pSenderThread);
	IF_NOT(m_pThreadPool->AddTask(*pSenderThread, 1, true))
	{
		IocpMgr.Fini();
		return false;
	}
	m_pSenderThread = pSenderThread.pop();

	ASSERTNET(m_pContextLocer->UnLock());

	m_bIsInitMgr = true;
	m_bDestroy = false;

	//初始化

	return true;
}


////////////////////////////////////////////////////////////////////
//	开始发送系统退出消息，退出完成端口和线程资源
void CIOCPModelMgr::Fini()
{
	ASSERTNET(m_pContextLocer);
	m_bIsInitMgr = false;

	if(m_bDestroy)
		return;

	m_bDestroy = true;

	//唤醒外部recv
	//ASSERTNET(PostStatus2External());
	//唤醒线程
	IF_OK(m_pSenderThread)
	{
		if (m_bIsThreadWork)
		{
			THREAD_TRANSFER::Post2Thread(m_pSenderThread, POST_OBJ_TYPE_NETWORK_SENDER, 0, 0, 0);
		}

		//等待线程结束
		THREAD_TRANSFER::WaitForFini(m_pSenderThread, POST_OBJ_TYPE_NETWORK_SENDER, false);

		m_pSenderThread = NULL;
	}

	if( m_pSvrListenSocket!=NULL && m_pSvrListenSocket->GetSocket()!=INVALID_SOCKET )
	{
		const I32 nCurThreadNum = m_CurWorkThreadNum;
		for (int i = 0; i < nCurThreadNum; ++i)
		{
			// 通知所有的完成端口操作退出
			PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
		}

		//等待线程全部退出
		IF_OK(m_pContextLocer)
		{
			while (true)
			{
				ASSERTNET(Lock());
				ASSERTNET(Wait(1));
				ASSERTNET(Unlock());
				if(m_CurWorkThreadNum<=0)
					break;
			}
		}
		
		//无线程

		// 清除客户端列表信息
		this->ClearSocketContainer();

		// 释放其他资源
		this->FreeResource();

		ASSERTNET(gtool::UnLoadSocketEnvLib());

		LOGNET("停止监听");
	}	
}

bool CIOCPModelMgr::IsDestroy() const
{
	return m_bDestroy;
}

bool CIOCPModelMgr::CreateSocket(_PER_SOCKET_CONTEXT& rSocketContext, bool bSetNoBlock)
{
	// 为以后新连入的客户端先准备好Socket( 这个是与传统accept最大的区别 ) 
	rSocketContext.SetSocketCloseFinal(true);
	rSocketContext.SetSocket(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED));
	IF_NOT(INVALID_SOCKET != rSocketContext.GetSocket())
	{
		LOGNET("创建用于Accept的Socket失败！错误代码: ", WSAGetLastError());
		return false;
	}

	ASSERTNET(gtool::SetSocketNoBlockMode(rSocketContext.GetSocketObj(), bSetNoBlock));

	return true;
}

////////////////////////////////
// 初始化完成端口
bool CIOCPModelMgr::InitializeIOCP()
{
	CHECKF(m_pThreadPool && !m_hIOCompletionPort);

	// 建立第一个完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	CHECKF(m_hIOCompletionPort);

	//根据本机处理器情况，建立对应的线程
	const auto nCoreCount = gtool::CpuCoreCount() * WORKER_THREADS_PER_PROCESSOR;
	for (MYULONG i = 0; i < nCoreCount; ++i)
	{
		gtool::CSmartPtrC<CIocpTask> pIocpTask = CIocpTask::CreateNew();
		IF_NOT(pIocpTask)
			continue;

		pIocpTask->SetArg(this);
		CTaskBase *pTaskBase = pIocpTask.pop();
		IF_NOT(pTaskBase)
			continue;

		ASSERTNET(m_pThreadPool->AddTask(*pTaskBase, 1, true));
	}

	return true;
}


/////////////////////////////////////////////////////////////////
// 初始化Socket
bool CIOCPModelMgr::PreBindListenEnv()
{
	CHECKF(m_pClientSocketMap);

	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	// 服务器地址信息，用于绑定Socket
	struct sockaddr_in ServerAddress;

	// 生成用于监听的Socket的信息
	CSmartPtrC<PER_SOCKET_CONTEXT> pListenContext= new PER_SOCKET_CONTEXT;
	CHECKF(pListenContext);

	// 需要使用重叠IO，必须得使用WSASocket来建立Socket，才可以支持重叠IO操作
	CHECKF(CreateSocket(*pListenContext));

	// 将Listen Socket绑定至完成端口中
	IF_NOT(AssociateWithIOCP(pListenContext))
	{
		RELEASE_SOCKET(pListenContext->GetSocketObj());
		return false;
	}

	// 填充地址信息
	CHECKF(gtool::FillTcpAddrInfo(ServerAddress, m_strIP.c_str(), m_nPort, AF_INET));                        

	// 绑定地址和端口
	CHECKF(SOCKET_ERROR != bind(pListenContext->GetSocket(), (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)));

	// 开始进行监听
	CHECKF(SOCKET_ERROR != listen(pListenContext->GetSocket(), SOMAXCONN));

	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下函数的指针，
	// 获取AcceptEx函数指针
	MYULONG dwBytes = 0;  
	IF_NOT(SOCKET_ERROR != WSAIoctl(
		pListenContext->GetSocket(),
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx, 
		sizeof(GuidAcceptEx), 
		&(m_lpfnAcceptEx),
		sizeof(m_lpfnAcceptEx), 
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		this->FreeResource();
		return false;  
	}  

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	IF_NOT(SOCKET_ERROR != WSAIoctl(
		pListenContext->GetSocket(),
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs), 
		&m_lpfnGetAcceptExSockAddrs, 
		sizeof(m_lpfnGetAcceptExSockAddrs),   
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		LOGERR("WSAIoctl 未能获取GuidGetAcceptExSockAddrs函数指针。错误代码:", WSAGetLastError());
		this->FreeResource();
		return false; 
	}  

	m_pSvrListenSocket = pListenContext.pop();
	ASSERTNET(m_pSvrListenSocket);

	// 为AcceptEx 准备参数，然后投递AcceptEx I/O请求
	for( int i=0;i<MAX_POST_ACCEPT;i++ )
	{
		gtool::CSmartPtrC<PER_SOCKET_CONTEXT> pSocketNewListen = new PER_SOCKET_CONTEXT;
		IF_NOT(pSocketNewListen)
			continue;

		CHECKF(PostAccept(*pSocketNewListen));	//这些socket不需要绑定完成端口，由完成端口监视accept的socket 再由 和该socket绑定的 socket触发ACCEPT事件
		
		CHECKF(this->AddToSocketMap(pSocketNewListen.pop()));
	}

	LOGNET("投递", MAX_POST_ACCEPT, " 个AcceptEx请求完毕");
	//this->_ShowMessage( _T("投递 %d 个AcceptEx请求完毕"),MAX_POST_ACCEPT );

	return true;
}

////////////////////////////////////////////////////////////
//	最后释放掉所有资源
void CIOCPModelMgr::FreeResource()
{
	IF_OK(m_pThreadPool)
		m_pThreadPool->Destroy();

	IF_OK(m_pSvrListenSocket)
		m_pSvrListenSocket.Release();

	// 关闭IOCP句柄
	RELEASE_HANDLE(m_hIOCompletionPort);
}


//====================================================================================
//
//				    投递完成端口请求
//
//====================================================================================

bool CIOCPModelMgr::PostAccept(PER_SOCKET_CONTEXT& rSocketContext, PER_RW_CONTEXT* pIOContext /*= NULL*/)
{
	CHECKF(m_lpfnAcceptEx);
	CHECKF(m_pSvrListenSocket);
	CHECKF(INVALID_SOCKET != m_pSvrListenSocket->GetSocket());
	CHECKF(rSocketContext.m_pListReadContext);

	//IO上下文
	if (!pIOContext)
	{
		pIOContext = rSocketContext.GetNewReadContext();
	}
	CHECKF(pIOContext);
	CHECKF(rSocketContext.m_pListReadContext->IsExist(pIOContext));	

	// 重叠结构缓冲区 系统填写
	DWORD dwBytes = 0;
	pIOContext->m_OpType = ACCEPT_POSTED;
	WSABUF *p_wbuf = &pIOContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIOContext->m_Overlapped;

	// 为以后新连入的客户端先准备好Socket( 这个是与传统accept最大的区别 ) 	
	if (rSocketContext.GetSocket() == NULL
		|| rSocketContext.GetSocket() == INVALID_SOCKET)
	{
		IF_NOT(CreateSocket(rSocketContext))
		{
			rSocketContext.DelReadContext(pIOContext);
			return false;
		}
	}

	// 投递AcceptEx
	LPFN_ACCEPTEX pAcceptex = (LPFN_ACCEPTEX)m_lpfnAcceptEx;
	if (!(pAcceptex(m_pSvrListenSocket->GetSocket(), rSocketContext.GetSocket(), p_wbuf->buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, p_ol)))
	{
		IF_NOT(WSA_IO_PENDING == WSAGetLastError())
		{
			LOGNET("投递 AcceptEx 请求失败，错误代码: ", WSAGetLastError());

			rSocketContext.DelReadContext(pIOContext);
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////

//accept连入，其中listensockt是绑定完成端口的，始终用于监听连入，被listensocket关联的socket是为了节省系统创建socket的时间，不可以绑定完成端口
//，因为这些socket是要去监听网络读写
bool CIOCPModelMgr::DoAccpet( PER_RW_CONTEXT* pIoContext )
{
	CHECKF(pIoContext);
	CHECKF(m_lpfnGetAcceptExSockAddrs);
	PER_SOCKET_CONTEXT *pCurSocketContainer = (PER_SOCKET_CONTEXT *) (pIoContext->m_pParrentObj);
	CHECKF(pCurSocketContainer);

	//准备参数
	SOCKADDR_IN* pClientAddr = NULL;
	SOCKADDR_IN* pLocalAddr = NULL;  
	int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);  

	// 1. 首先取得连入客户端的地址信息
	LPFN_GETACCEPTEXSOCKADDRS pGetAcceptexSockaddrs = (LPFN_GETACCEPTEXSOCKADDRS)m_lpfnGetAcceptExSockAddrs;
	pGetAcceptexSockaddrs(pIoContext->m_wsaBuf.buf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&pLocalAddr, &localLen, (LPSOCKADDR*)&pClientAddr, &remoteLen);

	IF_OK(pClientAddr)
	{
		//打印客户端连入
		BUFFER128 szBuffer = { 0 };
		szBuffer[0] = 0;
		U16 uPort = 0;
		char *pBuffer = szBuffer;
		gtool::GetAddrInfoFromNetInfo(*pClientAddr, pBuffer, sizeof(szBuffer), uPort);
		LOGNET("客户端 ", szBuffer, ":", uPort, "连入.");
	}

	//投递recv
	ASSERTNET(DoAcceptPostNewRecvSock(pIoContext->GetSocket(), pClientAddr));	

	// 创建新的套接字关联listensocket重新投递accept监听连入
	pIoContext->ResetBuffer();
	CHECKF(CreateSocket(*pCurSocketContainer));	//需要使用新的套接字 且不需要绑定完成端口，只要绑定listensokcet就可以
	return this->PostAccept(*(pCurSocketContainer), pIoContext);		//生成新的clientsocket	
}

////////////////////////////////////////////////////////////////////
// 投递接收数据请求
bool CIOCPModelMgr::PostRecv( PER_RW_CONTEXT* pIoContext )
{
	CHECKF(pIoContext);
	auto sSocket = pIoContext->GetSocket();
	CHECKF(sSocket&&sSocket != INVALID_SOCKET);

	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	pIoContext->ResetBuffer();
	pIoContext->m_OpType = RECV_POSTED;

	// 初始化完成后，，投递WSARecv请求
	int nBytesRecv = WSARecv( pIoContext->GetSocket(), p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	const I32 nLastErr = WSAGetLastError();
	IF_NOT ( (SOCKET_ERROR != nBytesRecv) || (WSA_IO_PENDING == nLastErr) )
	{
		LOGNET("投递WSARecv失败！nLastErr:", nLastErr,"dest ip:", pIoContext->GetIp(), ":", pIoContext->GetPort());
		return false;
	}
	return true;
}

bool CIOCPModelMgr::PostSend(PER_RW_CONTEXT& rIoContext)
{
	auto sSocket = rIoContext.GetSocket();
	CHECKF(sSocket&&sSocket != INVALID_SOCKET);
	rIoContext.m_wsaBuf.len = rIoContext.m_u64DataToHandle;
	CHECKF(rIoContext.m_wsaBuf.len <= MAX_BUFFER_LEN);

	// 初始化变量
	DWORD dwFlags = 0;
	DWORD nByteSend = 0;
	WSABUF *p_wbuf = &rIoContext.m_wsaBuf;
	OVERLAPPED *p_ol = &rIoContext.m_Overlapped;
	rIoContext.m_OpType = SEND_POSTED;

	// 初始化完成后，，投递WSARecv请求
	int nBytesSend = WSASend(rIoContext.GetSocket(), p_wbuf, 1, &nByteSend, dwFlags, p_ol, NULL);

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	auto nLastErr = WSAGetLastError();
	IF_NOT( (SOCKET_ERROR != nBytesSend)
		|| (WSA_IO_PENDING == nLastErr) )
	{
		LOGNET("投递WSASend失败！nLastErr:", nLastErr, "dest ip:", rIoContext.GetIp(), ":", rIoContext.GetPort());
		return false;
	}
	return true;
}

// Return value
// If no error occurs, WSAConnect returns zero.Otherwise, it returns SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError.On a blocking socket, the return value indicates success or failure of the connection attempt.
// With a nonblocking socket, the connection attempt cannot be completed immediately.In this case, WSAConnect will return SOCKET_ERROR, and WSAGetLastError will return WSAEWOULDBLOCK; the application could therefore :
// Use select to determine the completion of the connection request by checking if the socket is writeable.
// If your application is using WSAAsyncSelect to indicate interest in connection events, then your application will receive an FD_CONNECT notification when the connect operation is complete(successful or not).
// If your application is using WSAEventSelect to indicate interest in connection events, then the associated event object will be signaled when the connect operation is complete(successful or not).
// For a nonblocking socket, until the connection attempt completes all subsequent calls to WSAConnect on the same socket will fail with the error code WSAEALREADY.
// If the return error code indicates the connection attempt failed(that is, WSAECONNREFUSED, WSAENETUNREACH, WSAETIMEDOUT) the application can call WSAConnect again for the same socket.
// bool CIOCPModelMgr::PostConnect(PER_RW_CONTEXT* pIoContext)
// {
// 	CHECKF(pIoContext);
// 	auto sSocket = pIoContext->GetSocket();
// 	CHECKF(sSocket&&sSocket != INVALID_SOCKET);
// 
// 	// 初始化变量
// 	DWORD dwFlags = 0;
// 	DWORD dwBytes = 0;
// 	pIoContext->ResetBuffer();
// 	WSABUF *p_wbuf = &pIoContext->m_wsaBuf;
// 	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;
// 	pIoContext->ResetBuffer();
// 	pIoContext->m_OpType = CONNECT_POSTED;
// 	
// 	int nBytesConnect = WSAConnect(sSocket, (const struct sockaddr *)pIoContext->GetAddr(), sizeof(*pIoContext->GetAddr()), NULL, NULL, NULL, NULL);
// 
// 	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
// 	auto nErrCode = WSAGetLastError();
// 	IF_NOT((SOCKET_ERROR != nBytesConnect) || (WSA_IO_PENDING == nErrCode))
// 	{
// 		LOGNET("PostConnect失败！", "nErrCode=[", nErrCode, "]");
// 		return false;
// 	}
// 	return true;
// }

/////////////////////////////////////////////////////////////////
// 在有接收的数据到达的时候，进行处理
bool CIOCPModelMgr::DoRecv( PER_SOCKET_CONTEXT* pSocketContext, PER_RW_CONTEXT* pIoContext, const U64& nRecvByte, bool& bRecvNetData )
{
	CHECKF(pSocketContext);
	CHECKF(pIoContext);

	//数据长度
	pIoContext->m_u64DataToHandle = nRecvByte;

	// 先把上一次的数据显示出现，然后就重置状态，发出下一个Recv请求
	auto nCacheLen = nRecvByte * 2 + 4;
	gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> pBuffer = new CXByte[nCacheLen];

	char *pCache = (char *)((void *)pBuffer);
	IF_OK(pBuffer)
	{
		memset(pCache, 0, nCacheLen);
		ASSERTNET(gtool::Conver2HexStr(pIoContext->m_wsaBuf.buf, nRecvByte, pCache, nCacheLen));
	
		//todo
		ASSERTNET(THREAD_TRANSFER::Post2Thread(GetBusinessLogicMainThread()
			, POST_OBJ_TYPE_MAIN_THREAD, (SOCKET_IDD)(pSocketContext), pIoContext->m_wsaBuf.buf, nRecvByte));

	}

	//创建新的recv缓冲
	auto pNewRecv = pSocketContext->GetNewReadContext();
	CHECKF(pNewRecv);

	// 然后开始投递下一个WSARecv请求 注册下一个接收
	auto bSucRecv = PostRecv(pNewRecv);
	
	LOGNET("收到", (const char *)(pSocketContext->GetAddrInfo()), ":", U16(pSocketContext->GetAddrInfo()), "数据长度：", nRecvByte,"十六进制数据：", (char *)(pCache), " 信息字符串:", pIoContext->m_wsaBuf.buf);
	
	//保存住数据pIoContext

	return bSucRecv;
}


// 
// bool CIOCPModelMgr::DoConnect(PER_SOCKET_CONTEXT* pSocketContext, PER_RW_CONTEXT* pIoContext, const U64& nRecvByte)
// {
// 	CHECKF(pSocketContext);
// 	CHECKF(pIoContext);
// 
// 	// 先把上一次的数据显示出现，然后就重置状态，发出下一个Recv请求
// 	auto nCacheLen = nRecvByte * 2 + 4;
// 	gtool::CSmartPtrC<char, char, DEL_WAY_DELETE_MULTI> pBuffer = new char[nCacheLen];
// 
// 	char *pCache = pBuffer;
// 	IF_OK(pBuffer)
// 	{
// 		memset(pCache, 0, nCacheLen);
// 		ASSERTNET(gtool::Conver2HexStr(pIoContext->m_wsaBuf.buf, nRecvByte, pCache, nCacheLen));
// 	}
// 
// 	LOGNET("DoConnect 连接成功.", (const char *)(pSocketContext->m_Addrinfo), ":", U16(pSocketContext->m_Addrinfo), "数据长度：", nRecvByte, "十六进制数据：", (char *)(pCache), " 信息字符串:", pIoContext->m_wsaBuf.buf);
// 
// 	// 然后开始投递下一个WSARecv请求 注册下一个接收
// 	return true;
// }

bool CIOCPModelMgr::DoSend(PER_SOCKET_CONTEXT& rSocketContext, PER_RW_CONTEXT& rIoContext, const U64& nSendByte)
{
	CHECKF(rSocketContext.m_pListWriteContext);

	// 先把上一次的数据显示出现，然后就重置状态，发出下一个Recv请求
	auto nCacheLen = nSendByte * 2 + 4;
	gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> pBuffer = new CXByte[nCacheLen];

	char *pCache =(char*)(void *)pBuffer;
	IF_OK(pBuffer)
	{
		memset(pCache, 0, nCacheLen);
		ASSERTNET(gtool::Conver2HexStr(rIoContext.m_wsaBuf.buf, nSendByte, pCache, nCacheLen));
	}

	LOGNET("发送完成.", (const char *)(rSocketContext.GetAddrInfo()), ":", U16(rSocketContext.GetAddrInfo())," 缓冲长度：", rIoContext.m_u64DataToHandle, " 数据长度：", nSendByte, " 十六进制数据：", (char *)(pCache), " 信息字符串:", rIoContext.m_wsaBuf.buf);

	//校验发送完成的长度和缓冲的数据长度 重新发送
	IF_NOT (rIoContext.m_u64DataToHandle == nSendByte)
	{	
		++rIoContext.m_u16FailHandleCount;
		LOGNET("发送完成数据长度与缓冲数据长度不一致，", rIoContext.GetIp(), ":"
			, rIoContext.GetPort(), "数据长度：", nSendByte, "十六进制数据："
			, (char *)(pCache), " 信息字符串:", rIoContext.m_wsaBuf.buf
			, "发送失败. 失败次数[", rIoContext.m_u16FailHandleCount, "]");

		IF_NOT(SEND_FALE_COUNT_MAX >= rIoContext.m_u16FailHandleCount)
			return false;

		return PostSend(rIoContext);
	}

	LOGNET("发送成功.", (const char *)(rSocketContext.GetAddrInfo()), ":", U16(rSocketContext.GetAddrInfo()), "数据长度：", nSendByte, "十六进制数据：", (char *)(pCache), " 信息字符串:", rIoContext.m_wsaBuf.buf);

	//从send队列中移除若还有数据继续postsend
	IF_OK(rSocketContext.m_pListWriteContext->IsExist(&rIoContext))
		rSocketContext.DelWriteContext(&rIoContext);

	if (rSocketContext.m_pListWriteContext->IsEmpty())
		return true;

	auto pNewIoContext = rSocketContext.FrontToSend();
	IF_NOT(pNewIoContext)
		return true;	

	// 然后开始投递下一个WSARecv请求 注册下一个接收
	return PostSend(*pNewIoContext);
}

/////////////////////////////////////////////////////
// 将句柄(Socket)绑定到完成端口中
bool CIOCPModelMgr::AssociateWithIOCP( PER_SOCKET_CONTEXT *pContext )
{
	// 将用于和客户端通信的SOCKET绑定到完成端口中
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pContext->GetSocket(), m_hIOCompletionPort, (DWORD)pContext, 0);

	IF_NOT (hTemp)
	{
		LOGNET(("执行CreateIoCompletionPort()出现错误.错误代码："), GetLastError());
		return false;
	}

	return true;
}




//====================================================================================
//
//				    ClientSocketMap 相关操作
//
//====================================================================================


//////////////////////////////////////////////////////////////
// 将客户端的相关信息存储到数组中
bool CIOCPModelMgr::AddToSocketMap( PER_SOCKET_CONTEXT *pSocketContext )
{
	CHECKF(pSocketContext);
	CHECKF(m_pContextLocer&&m_pClientSocketMap);

	IF_OK(!m_pClientSocketMap->IsExist(U64(pSocketContext)))
	{
		m_pClientSocketMap->insert(std::make_pair(U64(pSocketContext), pSocketContext));
		CHECKF(m_pClientSocketMap->IsExist(U64(pSocketContext)));
	}

	return true;
}

////////////////////////////////////////////////////////////////
//	移除某个特定的Context
void CIOCPModelMgr::RemoveFromSocketMap( PER_SOCKET_CONTEXT *pSocketContext )
{
	CHECK(m_pClientSocketMap);
	CHECK(pSocketContext);

	U64 addrSocketContext = U64(pSocketContext);
	ASSERTNET(m_pClientSocketMap->DelObj(addrSocketContext));
}

////////////////////////////////////////////////////////////////
// 清空客户端信息
void CIOCPModelMgr::ClearSocketContainer()
{
	IF_OK(m_pClientSocketMap)
		m_pClientSocketMap->Cleans();
}
// 
// bool CIOCPModelMgr::DataSinal()
// {
// 	CHECKF(m_pDataStatusSinal);
// 	return m_pDataStatusSinal->SinalOne();
// }
// 
// bool CIOCPModelMgr::WaitForData(const unsigned long ulMilliseconds /*= INFINITE*/)
// {
// 	CHECKF(m_pDataStatusSinal);
// 	return m_pDataStatusSinal->WaitEvent(ulMilliseconds) != WIN_WAIT_EVENT_FAILED;
// }
// 
// bool CIOCPModelMgr::NetDataLock()
// {
// 	CHECKF(m_pDataStatusSinal);
// 	return m_pDataStatusSinal->Lock();
// }
// 
// bool CIOCPModelMgr::NetDataUnLock()
// {
// 	CHECKF(m_pDataStatusSinal);
// 	return m_pDataStatusSinal->UnLock();
// }


_RW_CONTEXT * CIOCPModelMgr::AddData(const SOCKET_IDD sDest, const void *pData, const U64 nSize, NET_IO_ERR_TYPE& errOut, bool bPutOutErrlog /*= false*/)
{
	CHECKF(m_bIsInitMgr);
	CHECKF(m_pContextLocer);
	CHECKF(m_pClientSocketMap);

	//初始化
	errOut = NET_IO_ERR_TYPE_NONE;

	//是否存在
	auto pSocketContext = m_pClientSocketMap->GetObj(sDest);
	IF_NOT(pSocketContext)
	{
		errOut = NET_IO_ERR_TYPE_SOCKET_NOT_EXIST;

		if (bPutOutErrlog)
			LOGNET("AddData nSize[", nSize, "].", " sDest[", sDest, "] NET_IO_ERR_TYPE_SOCKET_NOT_EXIST");

		return false;
	}

	//单包不能大于MAX_BUFFER_LEN
	IF_NOT(MAX_BUFFER_LEN >= nSize)
	{
		errOut = NET_IO_ERR_TYPE_PACKET_OVER_MAX;

		if (bPutOutErrlog)
			LOGNET("AddData nSize[", nSize, "].", " sDest[", sDest, "]", pSocketContext->GetIp(), ":", pSocketContext->GetPort(), " NET_IO_ERR_TYPE_PACKET_OVER_MAX");

		return false;
	}

	//创建写缓冲
	auto pNewIOContext = pSocketContext->GetNewWriteContext();
	IF_NOT(pNewIOContext)
	{
		errOut = NET_IO_ERR_TYPE_MEMORY_NOT_ENOUGH;

		if (bPutOutErrlog)
			LOGNET("AddData nSize[", nSize, "].", " sDest[", sDest, "]", pSocketContext->GetIp(), ":", pSocketContext->GetPort(), " NET_IO_ERR_TYPE_MEMORY_NOT_ENOUGH");

		return false;
	}

	//数据转移 优化处，考虑指针转移而非数据拷贝
	pNewIOContext->m_u64DataToHandle = nSize;
	memmove(pNewIOContext->m_szBuffer, pData, pNewIOContext->m_u64DataToHandle);

	return pNewIOContext;		
}

//====================================================================================
//
//				       其他辅助函数定义
//
//====================================================================================

bool CIOCPModelMgr::SendData(const SOCKET_IDD sDest, _RW_CONTEXT& rDataContext, NET_IO_ERR_TYPE& errOut, bool bPutOutErrlog/*=false*/)
{
	CHECKF(m_bIsInitMgr);
	CHECKF(m_pContextLocer);
	CHECKF(m_pClientSocketMap);

	//初始化
	errOut = NET_IO_ERR_TYPE_NONE;

	//是否存在
	auto pSocketContext = m_pClientSocketMap->GetObj(sDest);
	IF_NOT(pSocketContext)
	{
		errOut = NET_IO_ERR_TYPE_SOCKET_NOT_EXIST;

		if (bPutOutErrlog)
			LOGNET("SendData nSize[", rDataContext.m_u64DataToHandle, "].", " sDest[", sDest, "] NET_IO_ERR_TYPE_SOCKET_NOT_EXIST");

		return false;
	}

	//投递send
	IF_NOT(PostSend(rDataContext))
	{
		errOut = NET_IO_ERR_TYPE_POST_SEND;

		if (bPutOutErrlog)
			LOGNET("SendData PostSend nSize[", rDataContext.m_u64DataToHandle, "].", " sDest[", sDest, "]", rDataContext.GetIp(), ":", rDataContext.GetPort(), " NET_IO_ERR_TYPE_POST_SEND");

		IF_OK(pSocketContext->m_pListWriteContext->IsExist(&rDataContext))
			pSocketContext->DelWriteContext(&rDataContext);

		return false;
	}

	return true;
}

// nRecvSize 传0可以获取所需缓冲大小
// bool CIOCPModelMgr::RecvData(const SOCKET_IDD sDest, void *& pData, U64& nRecvSize)
// {
// 	pData = NULL;
// 	CHECKF(m_bIsInitMgr);
// 	CHECKF(m_pContextLocer);
// 	CHECKF(m_pClientSocketMap);
// 
// 	//是否存在
// 	auto pSocketContext = m_pClientSocketMap->GetObj(sDest);
// 	CHECKF(pSocketContext);
// 
// 	//有没有数据返回
// 	auto pIOContext = pSocketContext->GetReadFront();
// 	if (!pIOContext)
// 		return false;
// 
// 	//数据转移
// 	gtool::CSmartPtrC<char, char, DEL_WAY_DELETE_MULTI> pCache = new char[pIOContext->m_u64DataToHandle];
// 	CHECKF(pCache);
// 	nRecvSize = pIOContext->m_u64DataToHandle;
// 	memmove((char *)pCache, pIOContext->m_szBuffer, pIOContext->m_u64DataToHandle);
// 	
// 	//释放空间
// 	pSocketContext->DelReadContext(pIOContext);
// 
// 	pData = pCache.pop();
// 	ASSERTEX(pData);
// 
// 	return true;
// }

bool CIOCPModelMgr::Close(const SOCKET_IDD idSocket)
{
	CHECKF(idSocket != INVALID_SOCKET_IDD);
	CHECKF(m_bIsInitMgr);
	CHECKF(m_pClientSocketMap);
	CHECKF(m_pContextLocer);

	RemoveFromSocketMap((PER_SOCKET_CONTEXT *)(idSocket));

	return true;
}

void CIOCPModelMgr::GetBindAddrStr(void *pBuffer, const U16 u16SizeBuffer) const
{
	_snprintf((char *)pBuffer, u16SizeBuffer - 1, "%s:%hu", m_strIP.c_str(), m_nPort);
}



////////////////////////////////////////////////////////////////////

bool CIOCPModelMgr::SinalOne()
{
	CHECKF(m_pContextLocer);
	return m_pContextLocer->SinalOne();
}


PER_SOCKET_CONTEXT* CIOCPModelMgr::QueryClientContext(const SOCKET_IDD idSocket)
{
	CHECKF(IS_VALID_SOCKET_IDD(idSocket));
	CHECKF(m_pClientSocketMap);

	return m_pClientSocketMap->GetObj(idSocket);
}

bool CIOCPModelMgr::Wait(const unsigned long ulMilliseconds /*= INFINITE*/)
{
	CHECKF(m_pContextLocer);
	return m_pContextLocer->WaitEvent(ulMilliseconds) != WIN_WAIT_EVENT_FAILED;
}

bool CIOCPModelMgr::Unlock()
{
	CHECKF(m_pContextLocer);
	return m_pContextLocer->UnLock();
}

bool CIOCPModelMgr::Lock()
{
	CHECKF(m_pContextLocer);
	return m_pContextLocer->Lock();
}

bool gtool::CIOCPModelMgr::IsLock()
{
	CHECKF(m_pContextLocer);
	return m_pContextLocer->Islock();
}

///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// 判断客户端Socket是否已经断开，否则在一个无效的Socket上投递WSARecv操作会出现异常
// 使用的方法是尝试向这个socket发送数据，判断这个socket调用的返回值
// 因为如果客户端网络异常断开(例如客户端崩溃或者拔掉网线等)的时候，服务器端是无法收到客户端断开的通知的
//由协议层来判活
// bool CIOCPModelMgr::IsSocketAlive(PER_SOCKET_CONTEXT& rSocketContext)
// {
// 	ASSERTNET(SetSocketNoBlockMode(rSocketContext.m_Socket));
// 	int nByteSent=send(rSocketContext.m_Socket,"1", 1,0);
// 
// 	if (rSocketContext.IsDetectTimeOut(0, JUDGE_SOCKET_ALIVE_DELAY_TIME))
// 		return false;
// 
// 	return true;
// }

bool CIOCPModelMgr::DoAcceptPostNewRecvSock(const SOCKET sSocket, const SOCKADDR_IN* pClientAddr)
{
	// 2. 这里需要注意，这里传入的这个是ListenSocket上的Context，这个Context我们还需要用于监听下一个连接
	// 所以我还得要将ListenSocket上的Context复制出来一份为新连入的Socket新建一个SocketContext
	CSmartPtrC<PER_SOCKET_CONTEXT> pNewSocketContext = new PER_SOCKET_CONTEXT;
	CHECKF(pNewSocketContext);
	pNewSocketContext->SetSocket(sSocket);	//移交socket	用于读写数据
	IF_OK(pClientAddr)
		pNewSocketContext->SetAddrInfo(*pClientAddr);
	pNewSocketContext->SetSocketCloseFinal(true);

	// 参数设置完毕，将这个Socket和完成端口绑定(这也是一个关键步骤) 读写数据需要绑定完成端口，accpet只要listensocket绑定完成端口即可其他的不需要
	//accept只需要listensockt绑定完成端口即可，被listensockt关联的socket需要在下回读写post的时候才绑定端口用于监听网络读写操作
	CHECKF(AssociateWithIOCP(pNewSocketContext));

	// 3. 继续，建立其下的IoContext，用于在这个Socket上投递一个Recv数据请求
	PER_RW_CONTEXT* pNewIoContext = pNewSocketContext->GetNewReadContext();
	CHECKF(pNewIoContext);

	pNewIoContext->m_OpType = RECV_POSTED;

	// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
	CHECKF(PostRecv(pNewIoContext));

	// 4. 如果投递成功，那么就把这个有效的客户端信息，加入到Socketmap中去(需要统一管理，方便释放资源)
	PER_SOCKET_CONTEXT *pNetContext = NULL;
	IF_NOT(AddToSocketMap(pNetContext = pNewSocketContext.pop()))
	{
		SAFE_DELETE(pNetContext);
		return false;
	}

	return true;
}

// 
// //套接字超时监控
// bool CIOCPModelMgr::DelayMonitorSocket(long tv_sec, long tv_usec, bool bReadableDetect, bool bWriteableDetect, int *pErrOut, bool bSetOneAtLeast, bool bInfiniteWaiting)
// {
// 	bool bTimeOut = false;
// 
// 	//变量定义
// 	fd_set *pFd_ReadOpt = NULL;
// 	fd_set *pFd_WriteOpt = NULL;
// 	memset(&m_fs_read, 0, sizeof(m_fs_read));
// 	memset(&m_fs_write, 0, sizeof(m_fs_write));
// 
// 	//可读检测
// 	if (bReadableDetect)
// 	{
// 		FD_ZERO(&m_fs_read);
// 		FD_SET(m_nSocket, &m_fs_read);
// 		pFd_ReadOpt = &m_fs_read;
// 	}
// 
// 	//可写检测
// 	if (bWriteableDetect)
// 	{
// 		FD_ZERO(&m_fs_write);
// 		FD_SET(m_nSocket, &m_fs_write);
// 		pFd_WriteOpt = &m_fs_write;
// 	}
// 
// 	//超时监控参数初始化
// 	timeval timeout;
// 	memset(&timeout, 0, sizeof(timeout));
// 	timeout.tv_sec = tv_sec;	//监控
// 	timeout.tv_usec = tv_usec;
// 
// 	int ret = SOCKET_ERROR;
// 	if (bInfiniteWaiting)
// 	{
// 		ret = select(m_nSocket + 1, pFd_ReadOpt, pFd_WriteOpt, NULL, NULL);	//0表示超时，否则返回SOCKET_ERROR 当返回为-1时，所有描述符集清0。 当返回为正数时，表示已经准备好的描述符数。
// 	}
// 	else
// 	{
// 		ret = select(m_nSocket + 1, pFd_ReadOpt, pFd_WriteOpt, NULL, &timeout);	//0表示超时，否则返回SOCKET_ERROR 当返回为-1时，所有描述符集清0。 当返回为正数时，表示已经准备好的描述符数。
// 	}
// 
// 
// 	if (ret == SOCKET_ERROR)
// 	{
// 		if (pErrOut) *pErrOut = SOCKET_ERROR;
// 		if (bReadableDetect) FD_CLR(m_nSocket, pFd_ReadOpt);
// 		if (bWriteableDetect) FD_CLR(m_nSocket, pFd_WriteOpt);
// 		return TRUE;
// 	}
// 	else
// 	{
// 		if (pErrOut) *pErrOut = ret;
// 	}
// 
// 	if (!bSetOneAtLeast)
// 	{
// 		if (bReadableDetect && !bWriteableDetect)
// 		{
// 			// 判断socket句柄是否可读  
// 			if (!FD_ISSET(m_nSocket, &m_fs_read))
// 			{
// 				if (bReadableDetect) FD_CLR(m_nSocket, pFd_ReadOpt);
// 				bTimeOut = TRUE;
// 			}
// 		}
// 		else if (bWriteableDetect && !bReadableDetect)
// 		{
// 			// 判断socket句柄是否可写  
// 			if (!FD_ISSET(m_nSocket, &m_fs_write))
// 			{
// 				if (bWriteableDetect) FD_CLR(m_nSocket, pFd_WriteOpt);
// 				bTimeOut = TRUE;
// 			}
// 		}
// 		else if (bWriteableDetect&&bWriteableDetect)
// 		{
// 			if ((!FD_ISSET(m_nSocket, &m_fs_read)) && (!FD_ISSET(m_nSocket, &m_fs_write)))
// 			{
// 				if (bReadableDetect) FD_CLR(m_nSocket, pFd_ReadOpt);
// 				if (bWriteableDetect) FD_CLR(m_nSocket, pFd_WriteOpt);
// 				bTimeOut = TRUE;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if (bWriteableDetect || bReadableDetect)
// 		{
// 			bTimeOut = FALSE;
// 			// 判断socket句柄是否可读  
// 			if (!FD_ISSET(m_nSocket, &m_fs_read))
// 			{
// 				if (bReadableDetect) FD_CLR(m_nSocket, pFd_ReadOpt);
// 				bTimeOut |= TRUE;
// 			}
// 
// 			// 判断socket句柄是否可写  
// 			if (!FD_ISSET(m_nSocket, &m_fs_write))
// 			{
// 				if (bWriteableDetect) FD_CLR(m_nSocket, pFd_WriteOpt);
// 				bTimeOut |= TRUE;
// 			}
// 		}
// 	}
// 
// 
// 	return bTimeOut;
// }

///////////////////////////////////////////////////////////////////
// 显示并处理完成端口上的错误
bool CIOCPModelMgr::HandleError( PER_SOCKET_CONTEXT *pSocketContext, OVERLAPPED* & pOverlapped,const DWORD& dwErr )
{
	CHECKF(pSocketContext);

	// 如果是超时了，就再继续等吧  
	if(WAIT_TIMEOUT == dwErr)  
	{  	
		// 确认客户端是否还活着...
// 		IF_NOT(IsSocketAlive(*pContext) )
// 		{
// 			LOGNET(!"检测到客户端异常退出！NotAlive");
// 		}
// 		else
// 		{
// 			LOGNET(!"网络操作超时！重试中...");
// 			return true;
// 		}
		CHECKF(pOverlapped);
		
		LOGNET("网络操作超时！重试中..., socket可能需要判活");

		PER_RW_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_RW_CONTEXT, m_Overlapped);
		CHECKF(pIoContext);

		switch (pIoContext->m_OpType)
		{
		case ACCEPT_POSTED:
			{}
			break;
		case SEND_POSTED:
			{
				++pIoContext->m_u16FailHandleCount;
				LOGNET("发送完成数据长度与缓冲数据长度不一致，", pIoContext->GetIp(), ":"
					, pIoContext->GetPort(), "数据长度：", pIoContext->m_u64DataToHandle, "发送失败. 失败次数[", pIoContext->m_u16FailHandleCount, "]");

				IF_NOT(SEND_FALE_COUNT_MAX >= pIoContext->m_u16FailHandleCount)
					break;

				PostSend(*pIoContext);
			}
		case RECV_POSTED:
			{}
			break;
		default:
			break;
		}	

		return true;
	}  

	// 可能是客户端异常退出了
	else if( ERROR_NETNAME_DELETED==dwErr )
	{
		LOGNET("检测到客户端异常退出！ ERROR_NETNAME_DELETED");
	}

	else
	{
		LOGNET("完成端口操作出现错误，线程退出。错误代码:", dwErr );
	}

	return false;
}

NAMESPACE_END(gtool)


