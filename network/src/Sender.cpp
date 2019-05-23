#include <Net/Sender.h>
#include <Net/IOCPModel.h>
#include <Net/connection.h>

MEMPOOL_CREATE_IMPLEMENTATION(CSenderThread, m_MemObj)

int CSenderThread::Run()
{
	LOGSYS("CSenderThread Network Sender启动.");
	m_bThreadWork = true;
	while (!IocpMgr.IsDestroy() || !THREAD_TRANSFER::IsMsgQueueEmpty(this, POST_OBJ_TYPE_NETWORK_SENDER))
	{
		//处理
		DOTRY_B
		{
			ASSERTNET(THREAD_TRANSFER::RecverWaitAndHandle(this, POST_OBJ_TYPE_NETWORK_SENDER));
		}
			DOCATCH_ANY("CSenderThread::Run()")
		{
			ASSERTNET("CSenderThread Run");
		}
		DOCATCH_ANY_END
	}

	ASSERTEX(THREAD_TRANSFER::SetThreadAwakeStatus(this, POST_OBJ_TYPE_NETWORK_SENDER, true)); //线程苏醒
	LOGSYS("CSenderThread Network Sender退出.");
	m_bThreadWork = false;

	return 0;
}

int CSenderThread::Release()
{
	delete this;
	return 0;
}

bool CSenderThread::HandleRecvMsg(const U64 u64ConnectionPtr, const void *pData, const I64 nSize)
{
	if (!pData)
		return true;

	if (nSize <= 0)
		return true;

	if (!u64ConnectionPtr)
		return true;

	ASSERTEX(IocpMgr.Lock());
	NET_IO_ERR_TYPE eRROut = NET_IO_ERR_TYPE_NONE;
	auto pRWContext = IocpMgr.AddData(u64ConnectionPtr, pData, nSize, eRROut, true);
	IF_NOT(eRROut == NET_IO_ERR_TYPE_NONE)
	{
		LOGNET_FMT512("ADDData err[%d]", eRROut);
	}

	IF_OK(pRWContext)
	{
		eRROut == NET_IO_ERR_TYPE_NONE;
		ASSERTNET(IocpMgr.SendData(u64ConnectionPtr, *pRWContext, eRROut, true));
		IF_NOT(eRROut == NET_IO_ERR_TYPE_NONE)
		{
			LOGNET_FMT512("ADDData err[%d]", eRROut);
		}
	}	
	ASSERTEX(IocpMgr.Unlock());

	//todo send
	return true;
}
