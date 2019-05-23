#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "Workers.h"
#include "overlapped.h"
#include "SocketExFnsHunter.h"
#include <BaseCode/TaskBase.h>
#include<Net/IocpServer.h>

Workers::Workers(std::atomic<I32>& rCurThreadNum)
	:m_nCurThreadNum(rCurThreadNum)
{

}

I32 Workers::Run()
{
	DWORD bytes_transferred;
	ULONG_PTR completion_key;
	DWORD Flags = 0;
	Overlapped* overlapped = NULL;

	LOGSYSFMT_C512("IOCP 工作者线程 线程id[%d] 启动. 当前线程数[%d]", std::this_thread::get_id(), ++m_nCurThreadNum);
	
	DOTRY_B
	{
		while (!IocpMgr.IsDestroy())
		{
			BOOL bRet = GetQueuedCompletionStatus(IocpMgr.GetIocpPort(),
				&bytes_transferred,
				&completion_key,
				reinterpret_cast<LPOVERLAPPED*>(&overlapped),
				INFINITE);

			if (!completion_key || !overlapped)
				continue;

			// 		// 如果收到的是退出标志，则直接退出
			// 		if (EXIT_CODE == (DWORD)completion_key && !overlapped)
			// 			break;

			// 如果收到的是退出标志，则直接退出
			if (EXIT_CODE == (DWORD)completion_key)
				break;

			if (bRet == FALSE)
			{

				//客服端直接退出，没有调用closesocket正常关闭连接
				auto nErr = GetLastError();
				if (nErr != WAIT_TIMEOUT && ERROR_NETNAME_DELETED != nErr)
					continue;

				if (!overlapped)
					continue;

				if (overlapped->connection == NULL)
					continue;

				//客户端断开,调用回调函数
				if (IocpMgr.OnDisconnected)
					IocpMgr.OnDisconnected(*overlapped->connection);	//已经释放

				IF_NOT(overlapped->type != Overlapped::Accept_type)
				{
					IF_NOT(IocpMgr.Accept())
						continue;
				}

				continue;
			}

			if ((bytes_transferred == 0) &&
				(overlapped->type == Overlapped::Type::Read_type ||
					overlapped->type == Overlapped::Type::Write_type))
			{
				//断开
				if (overlapped->connection != NULL)
				{
					//客户端断开
					if (IocpMgr.OnDisconnected)
						IocpMgr.OnDisconnected(*overlapped->connection); //已经释放

																		 //overlapped = NULL;
																		 //SAFE_DELETE(overlapped->connection);

					IF_NOT(overlapped->type != Overlapped::Accept_type)
					{
						IF_NOT(IocpMgr.Accept())
							continue;
					}
				}

				continue;
			}
			else
			{
				switch (overlapped->type)
				{
				case Overlapped::Accept_type:
					{
						//acceptex完成了操作，所以我们还要将其关联到完成端口。
						//这里先不改造，等后面我们会进行优化改造
						//我们也可以添加多个accept到完成端口
						IF_NOT(IocpMgr.Accept())
							continue;

						//新客户端连接
						if (IocpMgr.OnConnected)
							IocpMgr.OnConnected(overlapped->connection);
					}
					break;
				case Overlapped::Type::Read_type:
					{
						// 异步读完成
						if (IocpMgr.OnRead)
							IocpMgr.OnRead(overlapped->connection, overlapped->connection->GetReadBuffer(), bytes_transferred);
					}
					break;
				case Overlapped::Type::Write_type:
					{
						auto conn = overlapped->connection;
						IF_NOT(conn)
							continue;
						conn->SetSentBytes(conn->GetSentBytes() + bytes_transferred);

						//判断是否只发送了一部分
						if (conn->GetSentBytes() < conn->GetTotalBytes())
						{
							//将剩余部分再发送
							overlapped->wsa_buf.len = conn->GetTotalBytes() - conn->GetSentBytes();
							overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(conn->GetWriteBuffer()) + conn->GetSentBytes();

							auto send_result = WSASend(conn->GetSocket(),
								&overlapped->wsa_buf, 1, &bytes_transferred,
								0, reinterpret_cast<LPWSAOVERLAPPED>(overlapped),
								NULL);

							ASSERTNET((send_result == NULL || (send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)));
						}
						else
						{
							//发送完成
							//AsyncRead(overlapped->connection);
							LOGNET("发送完成.");
							if (IocpMgr.OnWrite)
								IocpMgr.OnWrite(overlapped->connection, bytes_transferred);
						}
					}
					break;
				default:
					LOGNET_FMT512("错误的 overlapped->type[%d].", overlapped->type);
					ASSERTNET(false);
					break;
				}
			}
		}

	}
	DOCATCH_E1("Workers::Run()")
	{
		LOGCRASH("Network Module Workers::Run()");
	}
	
	//线程退出
	LOGSYSFMT_C512("IOCP 工作者线程 线程id[%d] 退出. 当前线程数[%d]", std::this_thread::get_id(), --m_nCurThreadNum);

	return 0;
}

Workers::~Workers()
{
}

void Workers::Cleans()
{
}

