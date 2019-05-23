#include "IocpServer.h"
#include <WinSock2.h>
#include <mswsock.h>
#include "overlapped.h"
#include <memory>
#include "Workers.h"
#include <Net/Sender.h>
#include <Net/NetDispatch.h>
#pragma warning(disable:4996)



IocpServer *g_pIocpMgr = NULL;

static int _count_recv_failed = 0;
std::atomic_bool IocpServer::m_bDestroy{ false };
IocpServer::IocpServer()
{
	m_pThreadPool = CThreadPool::CreateNew(0, 0);
	m_pSvrLocker = CDataLocker::CreateNew();
	m_pConnectionMap = ConnectionMap::CreateNew();
	m_pNetDispatch = CNetDispatch::CreateNew();
}


IocpServer::~IocpServer()
{
}

void IocpServer::Cleans()
{
}

bool IocpServer::Init()
{
	CHECKF(m_pSvrLocker);
	CHECKF(m_pThreadPool);
	CHECKF(m_pNetDispatch);
	IF_NOT(!m_bInit)
		return false;

	//加载网络环境
	CHECKF(WinSockInit());

	//回调函数
	m_pNetDispatch->Init();

	//地址端口
	BUFFER128 szIpStr = { 0 };
	char *pStrIP = szIpStr;
	if (m_strIP.GetLength() == 0)
	{
		CHECKF(gtool::GetLocalIp(pStrIP));
	}
	else
	{
		_sntprintf(szIpStr, sizeof(szIpStr) - 1, "%s", m_strIP.c_str());
	}


	CHECKF(strlen(szIpStr) > 0);

	//地址
	m_strIP = szIpStr;

	//创建完成端口
	_completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	IF_NOT(_completion_port)
	{
		LOGNET(("Init 执行CreateIoCompletionPort()出现错误.错误代码："), GetLastError());
		return false;
	}

	//创建监听端口
	CHECKF(InitListenSocket());

	//绑定地址
	CHECKF(Bind());

	//监听队列设置
	CHECKF(Listen(SOMAXCONN));

	//获取acceptex函数地址，只获取一次就可以了
	SocketExFnsHunter _socketExFnsHunter;
	_acceptex_func = _socketExFnsHunter.AcceptEx;

	CHECKF(_acceptex_func);

	//工作线程配置
	auto nWorkersNum = gtool::CpuCoreCount()*WORKER_THREADS_PER_PROCESSOR;
	m_pThreadPool->SetThreadNumLimit(0, nWorkersNum + 1);	//sender线程
	CHECKF(InitWorkers(nWorkersNum));

	//sender线程
	gtool::CSmartPtr<CSenderThread> pSender = new CSenderThread(m_bIsThreadWork);
	m_pSenderThread = pSender;
	CHECKF(m_pThreadPool->AddTask(*pSender.pop(), 1, true));
	CHECKF(m_pSenderThread);

	m_bInit = true;
	m_bDestroy = false;

	return true;
}

void IocpServer::Fini()
{
	if (m_bDestroy)
		return;

	m_bDestroy = true;
	m_bInit = false;

	//唤醒外部recv
	//ASSERTNET(PostStatus2External());
	const I32 nCurThreadNum = m_nCurTaskNum;
	IF_OK(_completion_port)
	{
		I32 nLoop = 0;
		for (;m_nCurTaskNum>0;)
		{
			// 通知所有的完成端口操作退出
			PostQueuedCompletionStatus(_completion_port, 0, (DWORD)EXIT_CODE, NULL);
			Sleep(1000);
			DEAD_LOOP_BREAK(nLoop, 100);
		}
	}

	//唤醒线程
	IF_OK(m_pSenderThread)
	{
		if (m_bIsThreadWork)
		{
			THREAD_TRANSFER::Post2Thread(m_pSenderThread, POST_OBJ_TYPE_NETWORK_SENDER, 0, 0, 0);
		}

		THREAD_TRANSFER::SetEnablePost(m_pSenderThread, POST_OBJ_TYPE_NETWORK_SENDER, false);

		//等待线程结束
		THREAD_TRANSFER::WaitForFini(m_pSenderThread, POST_OBJ_TYPE_NETWORK_SENDER, false);

		m_pSenderThread = NULL;
	}

	//销毁线程池
	IF_OK(m_pThreadPool)
		m_pThreadPool->Destroy();

	//端口客户端链接
	IF_OK(Lock())
	{
		IF_OK(m_pConnectionMap)
		{
			m_pConnectionMap->Cleans();
		}
		ASSERTEX(Unlock());
	}

	//关闭监听
	RELEASE_SOCKET(_socketSvrListen);

	// 关闭IOCP句柄
	RELEASE_HANDLE(_completion_port);

	//释放网络环境
	ASSERTNET(gtool::UnLoadSocketEnvLib());

	LOGNET("停止监听");
}

bool IocpServer::WinSockInit()
{
	CHECKF(gtool::LoadSocketEnvLib());
	CHECKF(gtool::IsLoadSocketEnvLib());

	return true;
}

bool IocpServer::InitListenSocket()
{
	CHECKF(m_pConnectionMap);

	//创建服务器套接字，这里要注意的是最后一个参数必须为：WSA_FLAG_OVERLAPPED重叠模式
	_socketSvrListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	CHECKF(IS_VALID_SOCKET_IDD(_socketSvrListen));
	ASSERTNET(gtool::SetSocketNoBlockMode(_socketSvrListen, true));

	std::unique_ptr<Connection> new_connection(new Connection(_socketSvrListen));
	CHECKF(new_connection.get());

	//关联完成端口
	IF_NOT(CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socketSvrListen), _completion_port, DWORD(new_connection.get()), 0))
	{
		LOGNET(("InitSocket 执行CreateIoCompletionPort()出现错误.错误代码："), GetLastError());
		return false;
	}	

	ASSERTNET(m_pConnectionMap->AddObj(new_connection->GetSocket(), new_connection.get()));

	new_connection.release();

	return true;
}

bool IocpServer::Bind()
{
	SOCKADDR_IN addr;
	CHECKF(gtool::FillTcpAddrInfo(addr, m_strIP.c_str(), m_nPort, AF_INET));

	CHECKF(bind(_socketSvrListen, (SOCKADDR*)&addr, sizeof(SOCKADDR)) != SOCKET_ERROR);

	return true;
}

bool IocpServer::Listen(unsigned int nListen)
{
	CHECKF(listen(_socketSvrListen, nListen) != SOCKET_ERROR);

	return true;
}

bool IocpServer::Accept(bool bSvrListen /*= false */)
{
	CHECKF(_acceptex_func);
	CHECKF(_completion_port);
	SOCKET accepted_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	CHECKF(IS_VALID_SOCKET_IDD(accepted_socket));

	std::unique_ptr<Connection> new_connection(new Connection(accepted_socket));
	CHECKF(new_connection.get());

	DWORD bytes = 0;
	auto pAcceptOverlapped = const_cast<Overlapped *>(new_connection->GetAcceptOverlapped());
	const int accept_ex_result = _acceptex_func
	(
		_socketSvrListen,
		accepted_socket,
		new_connection ->GetReadBuffer(),
		0,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		&bytes, reinterpret_cast<LPOVERLAPPED>(pAcceptOverlapped)
	);

	CHECKF((accept_ex_result == TRUE || WSAGetLastError() == WSA_IO_PENDING));

	// 将用于和客户端通信的SOCKET绑定到完成端口中
	HANDLE hTemp = CreateIoCompletionPort(reinterpret_cast<HANDLE>(accepted_socket), _completion_port, (DWORD)new_connection.get(), 0);
	IF_NOT(hTemp)
	{
		LOGNET(("执行CreateIoCompletionPort()出现错误.错误代码："), GetLastError());
		return false;
	}

	IF_OK(Lock())
	{
		ASSERTNET(m_pConnectionMap->AddObj(new_connection->GetSocket(), new_connection.get()));
		ASSERTEX(Unlock());
	}

	new_connection.release();

	return true;
}
// 
// void IocpServer::Run(const char* ip, unsigned short port, unsigned int nListen = 5)
// {
// 	if (Init(ip, port, nListen) == -1)
// 	{
// 		fprintf(stderr, "服务器启动失败\n");
// 		return;
// 	}
// 	//Mainloop();
// }
// 
// void IocpServer::Mainloop()
// {
// 	//现在有工作者线程去处理业务，那主循环可以做点其他的事情了
// 	for (;;)
// 	{
// 		if (_chrono_timer.GetSecInterval() > 1.0)
// 		{
// 			int  ClntCount = _client_count;
// 			int  msgCount = _msg_count;
// 			int  RecvFailedCount = _count_recv_failed;
// 
// 			fprintf(stderr, "client count<%d> msg count<%d> recv failed count:%d\n", ClntCount, msgCount, RecvFailedCount);
// 
// //			LOGOUT("client count<%d> msg count<%d>\n", _client_count, _msg_count);
// 			_chrono_timer.FlushTime();
// 			_msg_count = 0;
// 		}
// 		Sleep(1000);
// 	}
// 
// }

bool IocpServer::AsyncRead(const Connection* conn)
{
	CHECKF(conn);
	CHECKF(m_pConnectionMap);

	auto overlapped = conn->GetReadOverlapped();
	CHECKF(overlapped);
	overlapped->wsa_buf.len = overlapped->connection->GetReadBufferSize();
	overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(overlapped->connection->GetReadBuffer());

	//断开连接
	if (!m_pConnectionMap->IsExist(conn->GetSocket()))
		return true;

	DWORD flags = 0;
	DWORD bytes_transferred = 0;

	auto recv_result = WSARecv(overlapped->connection->GetSocket(),
		&overlapped->wsa_buf, 1, &bytes_transferred, &flags,
		reinterpret_cast<LPWSAOVERLAPPED>(overlapped), NULL);
	
	auto nLastErr = WSAGetLastError();
	IF_NOT ((recv_result == 0 || (recv_result == SOCKET_ERROR && nLastErr == WSA_IO_PENDING)))
	{
		BUFFER256 szBuffer = { 0 };
		char *pIp = szBuffer;
		U16 u16Port = 0;
		if (gtool::GetPeerAddr(overlapped->connection->GetSocket(), pIp, sizeof(szBuffer), u16Port) >= 0)
		{
			LOGNET_FMT_C1024("[%s:%hu].err[%d]. WSARecv失败 !(recv_result == 0 || (recv_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)", szBuffer, u16Port, nLastErr);
		}
		else
		{
			LOGNET_FMT_C1024("err[%d]. WSARecv失败 !(recv_result == 0 || (recv_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)", nLastErr);
		}

		//DeductClientCount(conn->GetSocket());
		return false;
	}

	return true;
}

bool IocpServer::AsyncWrite(const Connection* conn, const void* data, const std::size_t size)
{
	CHECKF(data);
	CHECKF(m_pConnectionMap);
	auto mutable_conn = const_cast<Connection*>(conn);
	CHECKF(mutable_conn);

	//断开连接
	if (!m_pConnectionMap->IsExist(mutable_conn->GetSocket()))
		return true;

	if (mutable_conn->GetWriteBufferSize() < size)
		mutable_conn->ResizeWriteBuffer(size);

	memmove_s(mutable_conn->GetWriteBuffer(), mutable_conn->GetWriteBufferSize(), data, size);

	mutable_conn->SetSentBytes(0);
	mutable_conn->SetTotalBytes(size);

	auto overlapped = mutable_conn->GetWriteOverlapped();
	overlapped->wsa_buf.len = size;
	overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(mutable_conn->GetWriteBuffer());

	DWORD bytes = 0;
	auto send_result = WSASend(mutable_conn->GetSocket(),
		&overlapped->wsa_buf, 1,
		&bytes, 0,
		reinterpret_cast<LPWSAOVERLAPPED>(overlapped),
		NULL);

	auto nLastErr = WSAGetLastError();
	IF_NOT((send_result == 0 || (send_result == SOCKET_ERROR && nLastErr == WSA_IO_PENDING)))
	{
		BUFFER256 szBuffer = { 0 };
		char *pIp = szBuffer;
		U16 u16Port = 0;
		if (gtool::GetPeerAddr(overlapped->connection->GetSocket(), pIp, sizeof(szBuffer), u16Port) >= 0)
		{
			LOGNET_FMT_C1024("[%s:%hu].err[%d]. WSASend发送数据失败 !(send_result == 0 || (send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))", szBuffer, u16Port, nLastErr);

		}
		else
		{
			LOGNET_FMT_C1024("err[%d]. WSASend发送数据失败 !(send_result == 0 || (send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))", nLastErr);

		}
		//DeductClientCount(conn->GetSocket());
		return false;
	}

	return true;
}

bool IocpServer::Lock()
{
	CHECKF(m_pSvrLocker);
	return m_pSvrLocker->Lock();
}

bool IocpServer::Unlock()
{
	CHECKF(m_pSvrLocker);
	return m_pSvrLocker->UnLock();
}

bool IocpServer::IsDestroy() const
{
	return m_bDestroy;
}

bool IocpServer::IsInit() const
{
	return m_bInit;
}

void IocpServer::DeductClientCount(const SOCKET_IDD sSocket)
{
	--_client_count;

	CHECK(m_pConnectionMap);
	ASSERTEX(m_pConnectionMap->DelObj(sSocket));
}

bool IocpServer::InitWorkers(const I32 nWorkNum)
{
	//根据本机处理器情况，建立对应的线程
	for (MYULONG i = 0; i < nWorkNum; ++i)
	{
		gtool::CSmartPtr<CTaskBase> pIocpTask = Workers::CreateNew(m_nCurTaskNum);
		CHECKF(pIocpTask);

		pIocpTask->SetArg(this);

		ASSERTNET(m_pThreadPool->AddTask(*pIocpTask.pop(), 1, true));
		CHECKF(Accept(true));
	}
	return true;
}

void IocpServer::GetBindAddrStr(void *pBuffer, const U16 u16SizeBuffer) const
{
	_snprintf((char *)pBuffer, u16SizeBuffer - 1, "%s:%hu", m_strIP.c_str(), m_nPort);
}

// 显示并处理完成端口上的错误
bool IocpServer::HandleError(const Connection* conn, OVERLAPPED* & pOverlapped, const DWORD& dwErr)
{
	CHECKF(conn);

	LOGNET_FMT512("网络异常!eRR[%lu]", dwErr);

	// 如果是超时了，就再继续等吧  
	if (WAIT_TIMEOUT == dwErr)
	{
		CHECKF(pOverlapped);

		LOGNET("网络操作超时！重试中..., socket可能需要判活");

		Overlapped* pOverlappedTmp = (Overlapped *)pOverlapped;

		switch (pOverlappedTmp->type)
		{
		case ACCEPT_POSTED:
			{}
			break;
		case SEND_POSTED:
			{
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
	else if (ERROR_NETNAME_DELETED == dwErr)
	{
		LOGNET("检测到客户端异常退出！ ERROR_NETNAME_DELETED");
	}

	else
	{
		LOGNET("完成端口操作出现错误，线程退出。错误代码:", dwErr);
	}

	return false;
}
