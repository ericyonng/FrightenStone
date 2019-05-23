#include "stdafx.h"
#include<Net/TcpSocket.h>
#include<errno.h>	
#include <Net/IOCPModel.h>
#include <BaseCode/enumtype.h>
#include <Net/NetDataPack.h>
//#include"SocketProtocol.h"
//#include"Dailylog.h"
//#include"rsa.h"
#include <BaseCode/Win/ThreadPost.inl>
#include<Net/Sender.h>
#include<Net/Reciever.h>


////////////////////////////////////


/////////////////////////////////////////////////////



////////////////////////////////////////////

//tcpsocket
boost::atomic_bool CTcpServerMgr::m_bIsDestroy{ false };
CTcpServerMgr *g_pTcpServerMgr = CTcpServerMgrInstance::GetInstance();
CTcpServerMgr::CTcpServerMgr()
{
	m_pThreadPool = CThreadPool::CreateNew();
}

CTcpServerMgr:: ~CTcpServerMgr()
{

}

void CTcpServerMgr::Cleans()
{
}


//断开链接
bool CTcpServerMgr::Close(const SOCKET_IDD idSocket)
{
	CHECKF(IocpMgr.Lock());
	auto bClose = IocpMgr.Close(idSocket);
	ASSERTNET(IocpMgr.Unlock());
	return bClose;
}

bool CTcpServerMgr::FinishIocp()
{
	m_bIsInit = false;

	if (m_bIsDestroy)
		return true;

	m_bIsDestroy = true;

	ASSERTNET(IocpMgr.PostStatus2External());

	//唤醒线程
	IF_OK(m_pSenderThread)
	{
		if (m_bIsThreadWork)
		{
			THREAD_TRANSFER::Post2Thread(m_pSenderThread, POST_OBJ_TYPE_NETWORK_SENDER, 0, 0, 0);
		}

		//等待线程结束
		THREAD_TRANSFER::WaitForFini(m_pSenderThread, POST_OBJ_TYPE_NETWORK_SENDER);

		m_pSenderThread = NULL;
	}



	//销毁线程池
	IF_OK(m_pThreadPool)
		m_pThreadPool->Destroy();

	IocpMgr.Fini();

	return true;
}

bool CTcpServerMgr::IsInitIocp()
{
	return IocpMgr.IsMgrInit();
}

bool CTcpServerMgr::Start()
{
	CHECKF(m_pThreadPool);

	CHECKF(IocpMgr.Init());

	m_pThreadPool->SetThreadNumLimit(0, 2);

	//sender任务
	gtool::CSmartPtr<CSenderThread> pSenderThread = new CSenderThread(m_bIsThreadWork);
	CHECKF(pSenderThread);
	IF_NOT(m_pThreadPool->AddTask(*pSenderThread, 1, true))
	{
		IocpMgr.Fini();
		return false;
	}
	m_pSenderThread = pSenderThread.pop();

	gtool::CSmartPtr<CReciever> pRecieverThread = new CReciever();
	CHECKF(pRecieverThread);
	IF_NOT(m_pThreadPool->AddTask(*pRecieverThread, 1, true))
	{
		IocpMgr.Fini();
		return false;
	}
	pRecieverThread.pop();

	m_bIsInit = true;
	m_bIsDestroy = false;
	return true;
}

bool CTcpServerMgr::Send(const SOCKET_IDD idSocket, const void *&pData, const U64 u64DataLen, TRAN_PROTOCOL::TRAN_TYPE eTranType /*= TRAN_PROTOCOL::TRAN_TYPE_MSG*/)
{
	CHECKF(IS_VALID_SOCKET_IDD(idSocket));
	NET_IO_ERR_TYPE eErrType = NET_IO_ERR_TYPE_NONE;
	
	if (IocpMgr.IsLock())
		IocpMgr.Unlock();

	CHECKF(IocpMgr.Lock());
	auto pSocketContext = IocpMgr.QueryClientContext(idSocket);

	IF_NOT(pSocketContext)
	{
		ASSERTNET(IocpMgr.Unlock());
		return false;
	}

	//封包
	CNetDataPack NetDataPack;
	gtool::CRandGen<> gRandor;
	pSocketContext->SetCurPackBatchID(pSocketContext->GetCurPackBatchID() + 1);
	NetDataPack.Init(pData, u64DataLen, true, gtool::GetMilliSecTimestamp(), gRandor, eTranType, TRAN_PROTOCOL::PACK_TYPE_SEND, pSocketContext->GetCurPackBatchID());
	char *pCache = NULL;
	U64 nPackSize = 0;
	_RW_CONTEXT *pFirstNewContext = NULL;
	bool bFirst = true;
	const auto nTotalNum = NetDataPack.GetCurPackModeTotalNum();
	auto pPackDataMapInfo = pSocketContext->GetNetDataSendMap(pSocketContext->GetCurPackBatchID(), eTranType, true);
	IF_NOT(pPackDataMapInfo)
	{
		ASSERTNET(IocpMgr.Unlock());
		return false;
	}

	PackDataMap *pPackDataMap = pPackDataMapInfo->m_pPackDataMap;
	IF_NOT(pPackDataMap)
	{
		ASSERTNET(IocpMgr.Unlock());
		return false;
	}

	BUFFER2048 szCacheNet = { 0 };
	pCache = szCacheNet;
	nPackSize = sizeof(szCacheNet);
	while (NetDataPack.PackNext(pCache, nPackSize))
	{
		IF_NOT(pCache&&nPackSize)
			continue;

		if (!pPackDataMap->empty())
			bFirst = false;

		//数据转移
		if (!bFirst)
		{
			gtool::CSmartPtrC<char, char, DEL_WAY_DELETE_MULTI> pNetStream = new char[sizeof(TRAN_PROTOCOL::NetDataStream) + nPackSize];
			IF_NOT(pNetStream)
				continue;

			TRAN_PROTOCOL::NetDataStream *pNetStreamPtr = (TRAN_PROTOCOL::NetDataStream *)((char *)pNetStream);
			pNetStreamPtr->nLen = nPackSize;
			memmove(pNetStreamPtr->rData, pCache, nPackSize);

			ASSERTNET(pPackDataMap->AddObj(NetDataPack.GetCurPackModeSeqNum(), pNetStream.pop()));
		}
			
		if(!bFirst)
			continue;

		NET_IO_ERR_TYPE eErrType = NET_IO_ERR_TYPE_NONE;
		_RW_CONTEXT* pNewWriteContext = IocpMgr.AddData(idSocket, pCache, nPackSize, eErrType, true);
		IF_NOT(pNewWriteContext)
		{
			ASSERTNET(" CTcpServerMgr::AddData出错");
			LOGNET(" CTcpServerMgr::AddData出错 eErrType[", eErrType, "] ", pSocketContext->GetIp(), ":", pSocketContext->GetPort(), " 批次：", NetDataPack.GetCurPackModeSeqNum());
			pPackDataMap->DelObj(NetDataPack.GetCurPackModeSeqNum());
			continue;
		}

		pFirstNewContext = pNewWriteContext;
		pNewWriteContext->m_nSeqNum = NetDataPack.GetCurPackModeSeqNum();
		pNewWriteContext->m_nTranType = eTranType;
		pNewWriteContext->m_idPackBatchID = pSocketContext->GetCurPackBatchID();
	}

	IF_NOT(pFirstNewContext)
	{
		pPackDataMap->Cleans();
		ASSERTNET(IocpMgr.Unlock());
		return false;
	}

	//发送首个数据包
	IF_NOT(IocpMgr.SendData(idSocket, *pFirstNewContext, eErrType, true))
	{
		pPackDataMap->Cleans();
		ASSERTNET(IocpMgr.Unlock());
		return false;
	}

	ASSERTNET(IocpMgr.Unlock());
	CHECKF(eErrType == NET_IO_ERR_TYPE_NONE);
		
	return true;
}

bool CTcpServerMgr::Recv()
{
	LOGNET("CTcpServerMgr::Recv() Start.");

	//数据缓冲
	gtool::CSmartPtr<SocketMsgInfoList> pList = SocketMsgInfoList::CreateNew();
	CHECKF(pList);

	while (IocpMgr.IsMgrInit() && (!m_bIsDestroy))
	{
		ASSERTNET(IocpMgr.Iocp2ExternalLock());
		ASSERTNET(IocpMgr.Iocp2ExternalWait());
		ASSERTNET(IocpMgr.Iocp2ExternalUnlock());

		//数据到来
		IF_OK(IocpMgr.Iocp2ExternalIocpGuardLock())
		{
			//移出数据
			pList->Clear();
			ASSERTNET(IocpMgr.Lock());
			auto IterList = IocpMgr.GetIocp2ExternalRecvList().NewEnum();
			while (IterList.Next())
			{
				SocketMsgInfo *pObj = IterList;
				IF_NOT(pObj)
					continue;

				ASSERTNET(pList->PushBack(pObj));
			}
			IocpMgr.GetIocp2ExternalRecvList().PopAll();

			IocpMgr.SetAwakeStatus(true);
			IocpMgr.Iocp2ExternalExtSinal();	//接收到数据进行缓存
			ASSERTNET(IocpMgr.Unlock());
		}
		ASSERTNET(IocpMgr.Iocp2ExternalIocpGuardUnLock());

		//数据到来数据处理
		if (!pList->IsEmpty())
		{
			auto IterList = pList->NewEnum();
			while (IterList.Next())
			{
				SocketMsgInfo* pObj = IterList;
				IF_NOT(pObj)
					continue;

				ASSERTNET(HandlerRecvData(*pObj));
			}
			
		}
	}

	ASSERTNET(IocpMgr.Iocp2ExternalIocpGuardLock());
	IocpMgr.SetExternalRecvFiniStatus();
	ASSERTNET(IocpMgr.Iocp2ExternalExtSinal());
	ASSERTNET(IocpMgr.Iocp2ExternalIocpGuardUnLock());

	LOGNET("CTcpServerMgr::Recv() Fini.");

	return true;
}


bool CTcpServerMgr::HandlerRecvData(SocketMsgInfo& rMsgInfo)
{
	switch (rMsgInfo.eType)
	{
	case SOCKET_STATUS_TYPE_CLOSED:	//链接断开处理
		{
			//post logout
			//todo

			//删除套接字
			ASSERTNET(IocpMgr.Lock());
			ASSERTNET(IocpMgr.Close(rMsgInfo.idSocket));
			ASSERTNET(IocpMgr.Unlock());			
		}
		break;
	case SOCKET_STATUS_TYPE_RECVED:	//数据到来处理
		{
			//取数据
			void *pData = NULL;
			U64 nRecvSize = 0;
			ASSERTEX(IocpMgr.Lock());
			ASSERTEX(IocpMgr.RecvData(rMsgInfo.idSocket, rMsgInfo.nPackBatchID, rMsgInfo.eTranType, pData, nRecvSize));
			ASSERTEX(IocpMgr.Unlock());
			CHECKF(pData&&nRecvSize);
			gtool::CSmartPtrC<char, char, DEL_WAY_DELETE_MULTI> pDataStream = (char *)pData;	//这样扔到对方线程 pData就成了野指针了
			const void *pStreamPtr = pDataStream;
			auto pLogicMainThread = IocpMgr.GetBusinessLogicMainThread();
			ASSERTEX(pLogicMainThread);
			ASSERTEX(THREAD_TRANSFER::Post2Thread(pLogicMainThread, POST_OBJ_TYPE_MAIN_THREAD, rMsgInfo.idSocket, pStreamPtr, nRecvSize));	//pStreamPtr会在消息队列中进行数据拷贝
			//todo
		}
		break;
	default:
		ASSERTNET(false);
		break;
	}
	return true;
}

