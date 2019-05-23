#include "NetDispatch.h"
#include "Message.hpp"
#include <Net/IocpServer.h>
#include <Net/connection.h>

CNetDispatch::CNetDispatch()
{
}

CNetDispatch::~CNetDispatch()
{

}

void CNetDispatch::Cleans()
{
}


void CNetDispatch::OnWrite(const Connection* conn, std::size_t bytes_transferred)
{
	CHECK(conn);
	//继续异步读取客户端
	ASSERTEX(IocpMgr.Lock());
	IocpMgr.AsyncRead(conn);
	ASSERTEX(IocpMgr.Unlock());
}

void CNetDispatch::OnRead(const Connection* conn, void* data, std::size_t size)
{
	CHECK(conn);

	auto variable_conn = const_cast<Connection*>(conn);

	MesageHeader *mh = nullptr;
	int msgPos = variable_conn->GetMsgPos();
	memmove(variable_conn->GetMsgBuffer() + msgPos, variable_conn->GetReadBuffer(), size);
	variable_conn->SetMsgPos(msgPos + size);

	LOGNET_FMT512("接收到数据 %I64u", size);

	while (variable_conn->GetMsgPos() >= sizeof(MesageHeader))
	{
		mh = (MesageHeader*)variable_conn->GetMsgBuffer();
		IF_NOT(mh)
			break;

		//解析数据
		if (mh->lenth <= variable_conn->GetMsgPos() && (mh->lenth>=0))
		{
			//移动位置 扣除当前解析的消息长度
			variable_conn->SetMsgPos(variable_conn->GetMsgPos() - mh->lenth);

			//处理消息
			ProcessMessage(mh, variable_conn);

			//后面未处理消息往前移
			if(variable_conn->GetMsgPos()>0)
				memmove(variable_conn->GetMsgBuffer(), variable_conn->GetMsgBuffer() + mh->lenth, variable_conn->GetMsgPos());
			
		}
		else{
			ASSERTNET("OnRead 数据解析错误 mh->lenth > variable_conn->GetMsgPos() && (mh->lenth>=0)"," mh->lenth:" mh->lenth);
			break;
		}
	}

	ASSERTEX(IocpMgr.Lock());
	IocpMgr.AsyncRead(conn);
	ASSERTEX(IocpMgr.Unlock());

}

void CNetDispatch::OnClientDisconnected(const Connection& conn)
{
#if 0
	if (conn == NULL)
		int mm = 0;
	auto variable_conn = const_cast<Connection*>(conn);
	SOCKET clifd = variable_conn->GetSocket();
	fprintf(stderr, "client:%d 断开\n", clifd);
#endif
	ASSERTNET(IocpMgr.Lock());
	SOCKET  s = conn.GetSocket();
	IF_OK(IS_VALID_SOCKET_IDD(s))
		IocpMgr.DeductClientCount(s);
	LOGNET("客户端断开", s);
	ASSERTNET(IocpMgr.Unlock());

}

void CNetDispatch::Init()
{
	//绑定回调函数
	IocpMgr.SetOnConnectedCallback(std::bind(&CNetDispatch::OnClientConnected, this, std::placeholders::_1));
	IocpMgr.SetOnDisconnectedCallback(std::bind(&CNetDispatch::OnClientDisconnected, this, std::placeholders::_1));
	IocpMgr.SetOnReadCallback(std::bind(&CNetDispatch::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	IocpMgr.SetOnWriteCallback(std::bind(&CNetDispatch::OnWrite, this, std::placeholders::_1, std::placeholders::_2));

}

void CNetDispatch::OnClientConnected(const Connection* conn)
{
//test03 屏蔽打印消息
	//fprintf(stderr, "新客户端连接\n");

//test05
	LOGNET("客户端连入.");
	IocpMgr.AddClientCount();
	ASSERTEX(IocpMgr.Lock());
	IocpMgr.AsyncRead(conn);
	ASSERTEX(IocpMgr.Unlock());
}


void CNetDispatch::ProcessMessage(MesageHeader* mh,Connection* conn)
{
	CHECK(conn&&mh);
	SOCKET sSocket = conn->GetSocket();

	MesageHeader *sendMsg = nullptr;
	switch (mh->type)
	{
	case T_Login:
	{
#if 1
			IocpMgr.AddClientCount();
		//if (_iocpServer._chrono_timer.GetSecInterval() > 1.0)
		//{
		//	fprintf(stderr, "client count<%d> msg count<%d>\n", _iocpServer._client_count, _iocpServer._msg_count);
		//	_iocpServer._chrono_timer.FlushTime();
		//	_iocpServer._msg_count = 0;
		//}
		//return;
#endif
		Login* lg = (Login*)mh;
		LOGNET_FMT1K("client socket<%d> T_Login name(%s),passwrod(%s) 数据长度:%d\n", sSocket, lg->name, lg->password, mh->lenth);
		sendMsg = new Login_Result();
		strcpy(((Login_Result *)sendMsg)->name, lg->name);
		auto pBusinessThread = IocpMgr.GetBusinessLogicMainThread();
		IF_OK(pBusinessThread)
		{
			POST_OBJ_TYPE_MAIN_THREAD;
			ASSERTNET(THREAD_TRANSFER::Post2Thread(pBusinessThread, POST_OBJ_TYPE_MAIN_THREAD, (U64)conn, mh, sizeof(Login)));
		}
		//IocpMgr.AsyncRead(conn);
	}
		break;
	case T_Logout:
	{
		Logout* lg = (Logout*)mh;
		LOGNET_FMT1K("client socket<%d> T_Logout name(%s),数据长度:%d\n", sSocket, lg->name, mh->lenth);
		sendMsg = new Logout_Result();
	}
		break;
	case T_ERROR:
		LOGNET_FMT1K("T_ERROR\n");
		break;
	default:
		fprintf(stderr, "未知消息\n");
		break;
	}
	if (sendMsg != nullptr)
	{
		ASSERTEX(IocpMgr.Lock());
		IocpMgr.AsyncWrite(conn, sendMsg, sendMsg->lenth);
		ASSERTEX(IocpMgr.Unlock());
	}		
}