#include <Net/ClientReciever.h>
#include <Net/TcpClient.h>
#include <BaseCode/TaskBase.h>

MEMPOOL_CREATE_IMPLEMENTATION(CTcpClientReciever, m_MemObj)

CTcpClientReciever::CTcpClientReciever()
{
}

CTcpClientReciever::~CTcpClientReciever()
{
}

void CTcpClientReciever::Cleans()
{
}

//必须先链接服务器
int CTcpClientReciever::Run()
{
	//检测可读
	SOCKET nSock = INVALID_SOCKET;
	I32 nErr = 0;
	
	
	CHECKF(TcpClientMgr.IsInit());
	CHECKF(TcpClientMgr.GetConnectStatus());

	TRAN_PROTOCOL::NetCache rNetCache;
	long nSizeBuffer = 0;
	BUFFER1024 szBuffer = { 0 };
	BUFFER512 szAddr = { 0 };
	gtool::CFmtIOString rIoStr;
	while (!TcpClientMgr.IsDestroy())
	{
		nSock = TcpClientMgr.GetSocketFD();
		IF_NOT(IS_VALID_SOCKET_IDD(nSock))
			continue;

		//读取数据	
		TcpClientMgr.Lock();
		auto&rReadFd = TcpClientMgr.GetReadFdSet();
		auto&rWriteFd = TcpClientMgr.GetWriteFdSet();
		if (gtool::IsDetectTimeOut(nSock, rReadFd, rWriteFd, 0, 5000, true, false, &nErr))
		{
			TcpClientMgr.Unlock();
			continue;
		}

		IF_NOT(TcpClientMgr.RecvDataFrom(rNetCache.m_ptr, nSizeBuffer))
		{
			TcpClientMgr.Unlock();
			continue;
		}
		TcpClientMgr.Unlock();			
		
		//反馈
		if (rNetCache.m_ptr)
		{
			rIoStr.Clear();
			ZERO_CLEAN_PTR(szAddr, sizeof(szAddr));
			TcpClientMgr.GetSvrAddr(szAddr, sizeof(szAddr));
			ZERO_CLEAN_PTR(szBuffer, sizeof(szBuffer));
			_snprintf((char *)szBuffer, sizeof(szBuffer) - 1, "收到数据：[%s] ", szAddr);
			rIoStr << szBuffer;
			ZERO_CLEAN_PTR(szBuffer, sizeof(szBuffer));
			memmove(szBuffer, rNetCache.m_pstr, nSizeBuffer);
			SAFE_DELETE_MULTI(rNetCache.m_pstr);
			rIoStr << szBuffer;

			LOGSYS(rIoStr.c_str());
		}
	}

	return 0;
	
	//读取数据
	//反馈
}
