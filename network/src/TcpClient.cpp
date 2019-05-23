#include <Net/TcpClient.h>
#include <Net/NetDataPack.h>
#include <Net/IOCPModel.h>
#include <Net/ClientReciever.h>

#undef SEND_ERR_MAX_COUNT			
#define SEND_ERR_MAX_COUNT			3		//发送失败最大次数
// 
// CTcpClient::CTcpClient()
// {
// }
// 
// CTcpClient::~CTcpClient()
// {
// }
// 
// void CTcpClient::Cleans()
// {
// 	ASSERTEX(Close());
// }
// 
// bool CTcpClient::SetAddr(const char *domain, const U16 nPort)
// {
// 	m_addrRemote.SetAddrInfo(AF_INET, NULL, domain, false, &nPort, false);
// 	return true;
// }
// 
// bool CTcpClient::Create()
// {
// 	//socket环境
// 	CHECKF(gtool::LoadSocketEnvLib());
// 
// 	//是否已创建套接字
// 	CHECKF(!IS_VALID_SOCKET_IDD(m_sRemote));
// 
// 	//创建socket
// 	m_sRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// 	CHECKF(IS_VALID_SOCKET_IDD(m_sRemote));
// 
// 	return true;
// }
// 
// bool CTcpClient::Connect()
// {
// 	//合法验证
// 	CHECKF(IS_VALID_SOCKET_IDD(m_sRemote));
// 
// 	const sockaddr_in *sin = m_addrRemote;
// 	CHECKF(sin);
// 
// 	//切换套接字模式
// 	CHECKF(gtool::SetSocketNoBlockMode(m_sRemote));
// 
// 	//链接
// 	bool bConnectTimeOut = true;
// 	WORD i = 0;
// 	for (i = 0; i < TIME_OUT_COUNT; i++)
// 	{
// 		ASSERTEX((SOCKET_ERROR !=connect(m_sRemote, (struct sockaddr *)sin, sizeof(sockaddr_in))) || (WSAGetLastError() != WSAEWOULDBLOCK));
// 
// 		int ret = 1;
// 		if (gtool::IsDetectTimeOut(m_sRemote, m_fs_read, m_fs_write, 0, 500000, false, true, &ret, false)){
// 			continue;	//超时为500毫秒
// 		}
// 
// 		//判断是否链接成功
// 		int optval = -1;
// 		int optlen = sizeof(optval);
// 		int retget = getsockopt(m_sRemote, SOL_SOCKET, SO_ERROR, (char*)(&optval), &optlen);
// 		if (retget != 0 || optval != 0)
// 			continue;
// 
// 		//多次超时
// 		if (ret <= 0 && (i == (TIME_OUT_COUNT - 1)))
// 			continue;
// 
// 		//连通
// 		if (ret > 0)
// 		{
// 			bConnectTimeOut = false;
// 			break;
// 		}
// 	}
// 
// 	//判断超时
// 	CHECKF(!bConnectTimeOut);
// 
// 	m_bConnected = true;
// 	return true;
// }
// 
// bool CTcpClient::Send(const void *pData, const I64 nDataSize)
// {
// 	//合法性校验
// 	CHECKF(m_bConnected);
// 	CHECKF(IS_VALID_SOCKET_IDD(m_sRemote));
// 	CHECKF(pData&&nDataSize > 0);
// 
// 	//发送数据
// 	I64 nRet = 0;
// 	nRet = send(m_sRemote, (char *)pData, nDataSize, 0);
// 	CHECKF((nRet != -1) && (nRet == nDataSize));
// 
// 	return true;
// }
// 
// bool CTcpClient::Recv(void *pData, I64& nDataSizeOut)
// {
// 	CHECKF(m_bConnected);
// 	CHECKF(IS_VALID_SOCKET_IDD(m_sRemote));
// 
// 	//合法验证
// 	CHECKF(pData && (nDataSizeOut > 0));
// 	ZERO_CLEAN_PTR(pData, nDataSizeOut);
// 
// 	//定义
// 	I64 nRecvByte = 0;
// 	nRecvByte = recv(m_sRemote, (char *)pData, nDataSizeOut, 0);
// 	CHECKF(nRecvByte <= 0);
// 	nDataSizeOut = nRecvByte;
// 
// 	return true;
// }
// 
// bool CTcpClient::Close()
// {
// 	m_bConnected = false;
// 	if (IS_VALID_SOCKET_IDD(m_sRemote))
// 		return true;
// 
// 	if (closesocket(m_sRemote) != 0)
// 	{
// 		SET_INVALID_SOCKET(m_sRemote);
// 		return false;
// 	}
// 
// 	SET_INVALID_SOCKET(m_sRemote);
// 	return true;
// }
// 
// 
// 
// CClientSocketMgr *g_pClientSocketMgr = CClientSocketMgrInstance::GetInstance();
// 
// CClientSocketMgr::CClientSocketMgr()
// {
// 	//m_pNetDataBufferList = NetDataBufferList::CreateNew();
// 	m_pSendCacheAllNetworkDataCacheMap = AllNetworkDataCacheMap::CreateNew();
// 	m_pSendCacheLocker = CDataLocker::CreateNew();
// 	m_pRecvCacheAllNetworkDataCacheMap = AllNetworkDataCacheMap::CreateNew();
// 	m_pRecvCacheLocker = CDataLocker::CreateNew();
// 	m_pThreadPool = CThreadPool::CreateNew();
// }
// 
// CClientSocketMgr::~CClientSocketMgr()
// {
// }
// 
// void CClientSocketMgr::Cleans()
// {
// }
// 
// #undef ADDCLIENT_SOCKETTASK
// #define ADDCLIENT_SOCKETTASK(classname, status_param)\
// gtool::CSmartPtr<CTaskBase> pTask_##classname = new classname(status_param);\
// CHECKF(pTask_##classname);\
// CHECKF(m_pThreadPool->AddTask(*pTask_##classname, 1, true));\
// pTask_##classname.pop();
// 
// bool CClientSocketMgr::Init()
// {
// //	CHECKF(m_pNetDataBufferList);
// 	CHECKF(m_pThreadPool);
// 	CHECKF(m_pSendCacheAllNetworkDataCacheMap);
// 	CHECKF(m_pSendCacheLocker);
// 	CHECKF(m_pRecvCacheAllNetworkDataCacheMap);
// 	CHECKF(m_pRecvCacheLocker);
// 	m_pThreadPool->SetThreadNumLimit(0, 2);
// 
// 	//初始化发送和接收线程
// 	ADDCLIENT_SOCKETTASK(CClientSender, m_bSenderThreadWork);
// 	ADDCLIENT_SOCKETTASK(CClientReciever, m_bRecieverThreadWork);
// 	CHECKF(m_bSenderThreadWork);
// 	CHECKF(m_bRecieverThreadWork);
// 
// 	m_bInit = true;
// 	return true;
// }
// 
// bool CClientSocketMgr::Fini()
// {
// 	m_bInit = false;
// 
// 	if (m_bDestroy)
// 		return true;
// 
// 	m_bDestroy = true;
// 
// 	//发送线程唤醒
// 	IF_OK(m_pClientSender)
// 	{
// 		if (m_bSenderThreadWork)
// 		{
// 			THREAD_TRANSFER::Post2Thread(m_pClientSender, POST_OBJ_TYPE_NETWORK_CLIENT_SENDER, 0, 0, 0);
// 			THREAD_TRANSFER::WaitForFini(m_pClientSender, POST_OBJ_TYPE_NETWORK_CLIENT_SENDER);
// 		}
// 		m_pClientSender = NULL;
// 	}
// 
// 	//接收线程唤醒
// 	IF_OK(m_pClientReciever)
// 	{
// 		if (m_bSenderThreadWork)
// 		{
// 			THREAD_TRANSFER::Post2Thread(m_pClientReciever, POST_OBJ_TYPE_NETWORK_CLIENT_RECIEVER, 0, 0, 0);
// 			THREAD_TRANSFER::WaitForFini(m_pClientReciever, POST_OBJ_TYPE_NETWORK_CLIENT_RECIEVER);
// 		}
// 		m_pClientReciever = NULL;
// 	}
// 
// 	//销毁线程池
// 	IF_OK(m_pThreadPool)
// 		m_pThreadPool->Destroy();
// 
// 	//数据销毁
// 	IF_OK(m_pSendCacheAllNetworkDataCacheMap)
// 	{
// 		ASSERTNET(m_pSendCacheAllNetworkDataCacheMap->empty());
// 		m_pSendCacheAllNetworkDataCacheMap->Cleans();
// 	}
// 
// 	IF_OK(m_pRecvCacheAllNetworkDataCacheMap)
// 	{
// 		ASSERTNET(m_pRecvCacheAllNetworkDataCacheMap->empty());
// 		m_pRecvCacheAllNetworkDataCacheMap->Cleans();
// 	}
// }
// 
// bool CClientSocketMgr::SendData(const void *pData, const I64 nDataSize)
// {
// 	CHECKF(m_pClientSender);
// 	CHECKF(m_bInit);
// 
// 	//封包
// 	CHECKF(IS_VALID_SOCKET_IDD(m_sRemote));
// 	NET_IO_ERR_TYPE eErrType = NET_IO_ERR_TYPE_NONE;
// 
// 	//封包
// 	CNetDataPack NetDataPack;
// 	gtool::CRandGen<> gRandor;
// 	++m_nSendPackBatchID;
// 	NetDataPack.Init(pData, nDataSize, true, gtool::GetMilliSecTimestamp(), gRandor
// 		, TRAN_PROTOCOL::TRAN_TYPE_MSG, TRAN_PROTOCOL::PACK_TYPE_SEND, m_nSendPackBatchID);
// 	char *pCache = NULL;
// 	U64 nPackSize = 0;
// 	const auto nTotalNum = NetDataPack.GetCurPackModeTotalNum();
// 
// 	CHECKF(LockSendCache());
// 	while (NetDataPack.PackNext(pCache, nPackSize))
// 	{
// 		IF_NOT(pCache&&nPackSize)
// 			continue;
// 
// 		//压入缓冲
// 		auto pXByteCache = QuerySendCache(m_nSendPackBatchID, NetDataPack.GetCurTranType()
// 			, NetDataPack.GetCurPackModeSeqNum(), true, nPackSize);
// 
// 		CHECKF(pXByteCache);
// 		memmove(pXByteCache, pCache, nPackSize);
// 
// 		auto pMapInfo = QueryPackCacheMapInfo(m_nSendPackBatchID, NetDataPack.GetCurTranType());
// 		IF_NOT(pMapInfo)
// 			continue;
// 
// 		pMapInfo->m_nTotalByte += nPackSize;
// 	}
// 
// 	//post数据给发送数据线程
// 	THREAD_TRANSFER::Post2Thread(m_pClientSender, POST_OBJ_TYPE_NETWORK_CLIENT_SENDER, m_sRemote, (void *)1, 1);	//1， 1 无实际意义仅仅为了与唤醒线程做区分，client的sender线程的 pData和datasize无任何实际意义
// 
// 	return true;
// }
// 
// bool CClientSocketMgr::RecvData(void *&pData, I64& nDataSize, bool bSocketBlockMode/*=false*/, bool bNeedSocketBlockSet/*=true*/)
// {
// 	CHECKF(IS_VALID_SOCKET_IDD(m_sRemote));
// //	CHECKF(m_pNetDataBufferList);
// 	CleanResource();
// 
// 	//设置阻塞模式
// 	if (bNeedSocketBlockSet)
// 	{
// 		CHECKF(gtool::SetSocketNoBlockMode(m_sRemote, !bSocketBlockMode));
// 	}
// 
// 	I64 nSizeRecvTotalByte = 0;
// 	while (true)
// 	{
// 		//以最大包接收
// 		gtool::CSmartPtr<NetDataBuffer> pBuffer = NetDataBuffer::CreateNew();
// 		CHECKF(pBuffer);
// 		I64 nSizeBuffer = sizeof(pBuffer->szBuffer);
// 		if(!Recv(pBuffer->szBuffer, nSizeBuffer))
// 			break;
// 
// 		IF_NOT(nSizeBuffer>0)
// 			continue;
// 
// 		nSizeRecvTotalByte += nSizeBuffer;
// 		pBuffer->nSizeData = nSizeBuffer;
// 
// 		//压入队列
// //		ASSERTNET(m_pNetDataBufferList->PushBack(pBuffer.pop()));
// 	}
// 
// //	CHECKF(!m_pNetDataBufferList->IsEmpty());
// 
// 	//数据整合
// // 	gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> pByte = new CXByte[nSizeRecvTotalByte];
// // 	CHECKF(pByte);
// // 	CXByte *pByteTmp = pByte;
// // 	auto IterList = m_pNetDataBufferList->NewEnum();
// // 	while (IterList.Next())
// // 	{
// // 		NetDataBuffer *pBuffer = IterList;
// // 		IF_NOT(pBuffer)
// // 			continue;
// // 
// // 		IF_NOT(pBuffer->nSizeData > 0)
// // 			continue;
// // 			
// // 		memmove(pByteTmp, pBuffer->szBuffer, pBuffer->nSizeData);
// // 		pByteTmp += pBuffer->nSizeData;
// // 	}
// // 	pData = pByte.pop();
// // 	nDataSize = nSizeRecvTotalByte;	
// // 	CHECKF(nDataSize > 0);
// // 	CHECKF(pData);
// 	
// 	return true;
// }
// 
// bool CClientSocketMgr::LockSendCache()
// {
// 	CHECKF(m_pSendCacheLocker);
// 	return m_pSendCacheLocker->Lock();
// }
// 
// TimestampXByte* CClientSocketMgr::QueryTimestampSendStream(const PACK_UNIQUE_ID idPackBatch, TRAN_PROTOCOL::TRAN_TYPE eTransType, const I32 nSeqNum, bool bCreate /*= false*/)
// {
// 	CHECKF(m_pSendCacheAllNetworkDataCacheMap);
// 	auto pTransTypePackCacheMap = m_pSendCacheAllNetworkDataCacheMap->GetObj(idPackBatch);
// 	if (pTransTypePackCacheMap || !bCreate)
// 	{
// 		if (!pTransTypePackCacheMap)
// 			return NULL;
// 
// 		//协议类型
// 		PackCacheMapInfo* pMapInfo = pTransTypePackCacheMap->GetObj(eTransType);
// 		if (pMapInfo || !bCreate)
// 		{
// 			if (!pMapInfo)
// 				return NULL;
// 
// 			CHECKF(pMapInfo->m_pXByteStreamMap);
// 
// 			//数据流总量
// 			if (pMapInfo->m_nTotalByte > 0 || !bCreate)
// 			{
// 				if (pMapInfo->m_nTotalByte <= 0)
// 					return NULL;
// 
// 				//序号
// 				auto pXByte = pMapInfo->m_pXByteStreamMap->GetObj(nSeqNum);
// 				if (pXByte || !bCreate)
// 				{
// 					if (!pXByte)
// 						return NULL;
// 
// 					return pXByte;
// 				}
// 			}
// 		}
// 	}
// 
// 	//创建
// 	if (!pTransTypePackCacheMap)
// 	{
// 		gtool::CSmartPtr<TransTypePackCacheMap> pMap = TransTypePackCacheMap::CreateNew();
// 		CHECKF(pMap);
// 		ASSERTNET(m_pSendCacheAllNetworkDataCacheMap->AddObj(idPackBatch, pMap.pop()));
// 		pTransTypePackCacheMap = m_pSendCacheAllNetworkDataCacheMap->GetObj(idPackBatch);
// 	}
// 	CHECKF(pTransTypePackCacheMap);
// 
// 	//mapinfo
// 	auto pMapInfo = pTransTypePackCacheMap->GetObj(eTransType);
// 	if (!pMapInfo)
// 	{
// 		gtool::CSmartPtr<PackCacheMapInfo> pMapInfoTmp = new PackCacheMapInfo;
// 		CHECKF(pMapInfoTmp);
// 		ASSERTNET(pTransTypePackCacheMap->AddObj(eTransType, pMapInfoTmp.pop()));
// 		pMapInfo = pTransTypePackCacheMap->GetObj(eTransType);
// 	}
// 	CHECKF(pMapInfo);
// 	CHECKF(pMapInfo->m_pXByteStreamMap);
// 	auto &XByteStreamMap = *pMapInfo->m_pXByteStreamMap;
// 
// 	//创建时间数据流 时间戳由sender线程填充精确到毫秒
// 	auto pXByte = XByteStreamMap.GetObj(nSeqNum);
// 	if (!pXByte)
// 	{
// 		gtool::CSmartPtrC<TimestampXByte, TimestampXByte, DEL_WAY_DELETE> pXByteTmp = new TimestampXByte;
// 		CHECKF(pXByteTmp);
// 		ASSERTNET(XByteStreamMap.AddObj(nSeqNum, pXByteTmp.pop()));
// 		pXByte = XByteStreamMap.GetObj(nSeqNum);
// 	}
// 	CHECKF(pXByte);
// 
// 	return pXByte;
// }
// 
// CXByte* CClientSocketMgr::QuerySendCache(const PACK_UNIQUE_ID idPackBatch, TRAN_PROTOCOL::TRAN_TYPE eTransType, const I32 nSeqNum, bool bCreate /*= false*/, const I64 nCreateByte /*= 0*/)
// {
// 	auto pTimestampXByte = QueryTimestampSendStream(idPackBatch, eTransType, nSeqNum, false);
// 	if (pTimestampXByte || !bCreate)
// 	{
// 		if (!pTimestampXByte)
// 			return NULL;
// 
// 		if (pTimestampXByte->pdata_stream || !bCreate)
// 		{
// 			if (!pTimestampXByte->pdata_stream)
// 				return NULL;
// 
// 			IF_NOT(pTimestampXByte->nSizeStream == nCreateByte)
// 			{
// 				pTimestampXByte->pdata_stream.Release();
// 			}
// 			pTimestampXByte->nSizeStream = nCreateByte;
// 			pTimestampXByte->nTimestamp = 0;
// 
// 			return pTimestampXByte->pdata_stream;
// 		}			
// 	}
// 
// 	CHECKF(nCreateByte);
// 	pTimestampXByte = QueryTimestampSendStream(idPackBatch, eTransType, nSeqNum, true);
// 	CHECKF(pTimestampXByte);
// 	IF_NOT(!pTimestampXByte->pdata_stream)
// 	{
// 		pTimestampXByte->pdata_stream.Release();
// 	}
// 	pTimestampXByte->nSizeStream = 0;
// 	pTimestampXByte->nTimestamp = 0;
// 
// 	pTimestampXByte->pdata_stream = new CXByte[nCreateByte];
// 	CHECKF(pTimestampXByte->pdata_stream);
// 	pTimestampXByte->nSizeStream = nCreateByte;
// 
// 	return pTimestampXByte->pdata_stream;
// }
// 
// PackCacheMapInfo* CClientSocketMgr::QueryPackCacheMapInfo(const PACK_UNIQUE_ID idPackBatch, TRAN_PROTOCOL::TRAN_TYPE eTransType)
// {
// 	CHECKF(m_pSendCacheAllNetworkDataCacheMap);
// 	auto pTransTypePackCacheMap = m_pSendCacheAllNetworkDataCacheMap->GetObj(idPackBatch);
// 	if (!pTransTypePackCacheMap)
// 		return NULL;
// 
// 	return pTransTypePackCacheMap->GetObj(eTransType);
// }
// 
// bool CClientSocketMgr::UnlockSendCache()
// {
// 	CHECKF(m_pSendCacheLocker);
// 	return m_pSendCacheLocker->UnLock();
// }
// 
// bool CClientSocketMgr::DividePack(void *&pData, I64&nDataSize)
// {
// 
// }
// 
// void CClientSocketMgr::CleanResource()
// {
// // 	IF_OK(m_pNetDataBufferList)
// // 		m_pNetDataBufferList->Clear();
// }



/////////////////////////////////////////////////////


////////////////////////////////////////////

CTcpSocket::CTcpSocket()
{
	m_bConnected = FALSE;
	m_nSocket = INVALID_SOCKET;
	m_bAutoCloseSocket = true;
	m_bSocketReadable = false;
	m_bSocketWriteable = false;
	m_pLocker = CDataLocker::CreateNew();
}

CTcpSocket:: ~CTcpSocket()
{

}

void CTcpSocket::Cleans()
{
	if (m_bAutoCloseSocket)
		Close();
}

//启动socket环境
bool CTcpSocket::InitSocketEnvironment()
{
	try
	{
		if (!gtool::IsLoadSocketEnvLib())
		{
			gtool::LoadSocketEnvLib();
		}
	}
	catch (...)
	{
	}

	return TRUE;
}

//清理环境
bool CTcpSocket::CleanSocketEnvironment()
{
	int iRet = 0;
	try
	{
		if (gtool::IsLoadSocketEnvLib())
		{
			gtool::UnLoadSocketEnvLib();
		}
	}
	catch (...)
	{
		iRet = -1;
	}


	//一个进程调用一次就够了若使用过程中被清除则会出现异常

	return iRet == 0 ? true : false;
}

//套接字
bool CTcpSocket::CreateSocket()
{
	//先断开
	Close();

	m_nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);			//生成套接字
	IF_NOT(IS_VALID_SOCKET_IDD(m_nSocket))
	{
		m_bConnected = false;
		return false;
	}

	m_bSocketReadable = true;
	m_bSocketWriteable = true;

	return true;
}


//断开链接
bool CTcpSocket::Close()
{
	m_bConnected = false;
	m_bSocketReadable = false;
	m_bSocketWriteable = false;

	SOCKET nSock = m_nSocket;
	m_nSocket = INVALID_SOCKET;
	if(!IS_VALID_SOCKET_IDD(nSock))
		return true;

	CHECKF(closesocket(nSock) == 0);
	return true;
}

//优雅的断开连接
bool CTcpSocket::GraciousClose(int iHowto)
{
	if (m_nSocket <0 || INVALID_SOCKET == m_nSocket) return true;

	if (shutdown(m_nSocket, iHowto) != 0)
	{
		return false;
	}

	switch (iHowto)
	{
	case SD_RECEIVE:
		{
			m_bSocketReadable = false;
			break;
		}
	case SD_SEND:
		{
			m_bSocketWriteable = false;
			break;
		}
	case SD_BOTH:
		{
			m_bSocketReadable = false;
			m_bSocketWriteable = false;
			break;
		}
	}


	return true;
}

//发送数据
bool CTcpSocket::SendData(void *pData, long *plDataLen)
{
	CHECKF(IS_VALID_SOCKET_IDD(m_nSocket));
	CHECKF(pData&&plDataLen);

	//不可发送
	if (!m_bSocketWriteable) return false;

	//发送数据
	long lSendRet = 0;
	lSendRet = send(m_nSocket, (char *)pData, *plDataLen, 0);
	if (lSendRet == -1)
	{
		*plDataLen = -1;
		return false;
	}
	else if (lSendRet<*plDataLen)
	{
		*plDataLen = lSendRet;
		return false;
	}

	*plDataLen = lSendRet;

	//流量统计
	//m_DataFlowCount.lSendTotal += lSendRet;
	return true;
}

//接收数据
bool CTcpSocket::RecvData(void *pData, long *plDataLen)
{
	CHECKF(IS_VALID_SOCKET_IDD(m_nSocket));
	CHECKF(pData&&plDataLen);

	//不可接收
	CHECKF (m_bSocketReadable);

	//定义
	long lRecvByte = 0;

	//接收数据
	memset(pData, 0, *plDataLen);
	lRecvByte = recv(m_nSocket, (char *)pData, *plDataLen, 0);
	if (lRecvByte <= 0)
	{
		//Close();
		*plDataLen = lRecvByte;
		return false;
	}

	//输出
	*plDataLen = lRecvByte;

	//流量统计
	//m_DataFlowCount.lRecvTotal += lRecvByte;
	return true;
}


bool CTcpSocket::Init()
{
	CHECKF(m_pLocker);

	return true;
}

//设置地址
void CTcpSocket::SetDestAddr(void *pAddr)
{
	if (pAddr)
		SetAddrInfo(*((CAddr *)pAddr));
}

//设置地址
bool CTcpSocket::SetAddrInfo(CAddr &addr)
{
	m_SocketAddr.Reset();
	m_SocketAddr = addr;

	return	true;
}

//设置地址
bool CTcpSocket::SetAddrInfo(const char *domain, int domainlen, unsigned short port, unsigned short family, bool bUseBind, bool bSocketStream)
{
	m_SocketAddr.Reset();
	m_SocketAddr.SetAddrInfo(family, NULL, domain, false, &port, bUseBind, bSocketStream);
	return true;
}

//设置地址
bool CTcpSocket::SetAddrInfo(const char *ip, int iplen, unsigned short port, unsigned short family, bool bUseBind, bool bSocketStream, bool bIp)
{
	m_SocketAddr.Reset();
	m_SocketAddr.SetAddrInfo(family, ip, NULL, true, &port, bUseBind, bSocketStream);

	return	true;
}

//重置
VOID CTcpSocket::Clear()
{
	if (IS_VALID_SOCKET_IDD(m_nSocket)) Close();
	m_nSocket = INVALID_SOCKET;
	m_SocketAddr.Reset();
	m_bConnected = false;
}

//阻塞模式
bool CTcpSocket::SetBlockMode()
{
	SOCKET sSocket = m_nSocket;
	return gtool::SetSocketNoBlockMode(sSocket, false);
}

//非阻塞模式
bool CTcpSocket::SetNoBlockMode()
{
	SOCKET sSocket = m_nSocket;
	return gtool::SetSocketNoBlockMode(sSocket, true);
}

//套接字超时监控
bool CTcpSocket::DelayMonitorSocket(long tv_sec, long tv_usec, bool bReadableDetect, bool bWriteableDetect, int *pErrOut, bool bSetOneAtLeast, bool bInfiniteWaiting)
{
	SOCKET sSocket = m_nSocket;
	return gtool::IsDetectTimeOut(sSocket, m_fs_read, m_fs_write, tv_sec, tv_usec, bReadableDetect, bWriteableDetect, pErrOut, bSetOneAtLeast, bInfiniteWaiting);

}

//清空socket缓冲区
bool CTcpSocket::ClearSocketCache()
{
	//变量定义
	char clr[8] = { 0 };
	long lClrLen = 1;
	int iErr = 0;

	while (true)
	{
		memset(clr, 0, sizeof(clr));
		if (DelayMonitorSocket(1, 0, true, false, &iErr))
		{
			//异常
			if (iErr < 0) return false;

			break;
		}

		RecvData(clr, &lClrLen);
	}

	return TRUE;
}

//设置套接字
VOID CTcpSocket::SetSocketFD(SOCKET hSock)
{
	if (IS_VALID_SOCKET_IDD(hSock))
	{
		m_bSocketReadable = true;
		m_bSocketWriteable = true;
	}
	else
	{
		m_bSocketReadable = false;
		m_bSocketWriteable = false;
	}

	m_nSocket = hSock;
}

//获取对方ip端口
int CTcpSocket::GetPeerIP(char *peerip, int iPeerIPSize, unsigned short *peerport)
{
	if (!peerip || !peerport || iPeerIPSize <= 0) return -1;
	return gtool::GetPeerAddr(m_nSocket, peerip, iPeerIPSize, *peerport);
}

//获取本地地址端口
int CTcpSocket::GetLocalIP(char *localipaddr, int ipaddrsize, unsigned short *localport)
{
	IF_NOT(localipaddr&&localport && (ipaddrsize > 0))
		return -1;

	int ret = 0;
	struct sockaddr_in dstadd_in;
	socklen_t  len = sizeof(struct sockaddr_in);
	memset(&dstadd_in, 0, sizeof(struct sockaddr_in));
	if (ret = getsockname(m_nSocket, (struct sockaddr*)&dstadd_in, &len) < 0)
	{
		return WSAGetLastError();
		//err_err("getpeername()");
	}
	else
	{
		if (ipaddrsize < strlen(inet_ntoa(dstadd_in.sin_addr))) return -1;
		sprintf(localipaddr, "%s", inet_ntoa(dstadd_in.sin_addr));
		*localport = ntohs(dstadd_in.sin_port);
	}
	return ret;
}


///////////////////////////////////////////////////////////////////////////
//初始化socket环境
CNetworkService::CNetworkService()
{
	//m_BusinessLayerCache = NULL;
	m_ServerPort = 0;
	m_bIsDomainName = false;
	m_bSetTransferSecurity = false;
}

CNetworkService::~CNetworkService()
{
	//清空队列
	ClearAllDataCache();
}

void CNetworkService::Cleans()
{
	ClearAllDataCache();
}

//启动网络服务
bool CNetworkService::StartNetService(const bool bClearDataCache /*= true*/)
{
	//清空队列
	if (bClearDataCache)
		ASSERTEX(ClearAllDataCache());

	//创建socket
	return CreateSocket();
}



//断开链接
bool CNetworkService::CloseNetService()
{
	ASSERTEX(Close());

	//清空队列
	ASSERTEX(ClearAllDataCache());

	return true;
}

//业务逻辑层数据添加
bool CNetworkService::AddDataToSend(const void *pData, const long lDataLen, BusinessDataCacheList& rCacheList, I64& nBusinessTotalSize)
{
	//参数校验
	CHECKF(pData);

	//变量定义

	//是否设置安全传输
	if (m_bSetTransferSecurity)
	{
		CHECKF(false);
// 		//加密
// 		//密钥key
// 		BYTE pcbKey[DES_KEY_BIT + 1] = { 0 };
// 		BYTE cbKeyCipher[RSA_KEY_STR_LEN / 8] = { 0 };	//key密文
// 		BYTE *pKeyCipherTmp = cbKeyCipher;				//key密文
// 		unsigned long KeyCipherLen = RSA_KEY_STR_LEN / 8;
// 		DWORD dwKeyLen = DES_KEY_BIT + 1;
// 		DWORD dwDataLenTmp = lDataLen;		//明文长度
// 		CDESEngine DesSystem;
// 		CRsaEngine RsaSystem;
// 		//设置rsa密钥
// 		RsaSystem.SetPublicKey((unsigned char *)m_strRsaPublicKey.c_str());
// 		DWORD dwNeeded = DesSystem.EncrypteMemoryNeeded((BYTE *)pcbKey, &dwKeyLen, (BYTE *)pData, dwDataLenTmp);
// 		//int iKeyLentmpd = dwKeyLen;
// 		RsaSystem.Encrypte(pcbKey, dwKeyLen, &pKeyCipherTmp, &KeyCipherLen);
// 		pCache->pData = new char[dwNeeded + KeyCipherLen];
// 		if (!pCache->pData)
// 		{
// 			delete pCache;
// 			return FALSE;
// 		}
// 		memset(pCache->pData, 0, dwNeeded + KeyCipherLen);
// 		DesSystem.Encrypte(pcbKey, &dwKeyLen, (BYTE *)pData, dwDataLenTmp, (BYTE *)pCache->pData, &dwNeeded);	//业务层密文
// 		unsigned char *pDataTmp = (unsigned char *)pCache->pData;
// 		pDataTmp += dwNeeded;									//偏移
// 		memmove(pDataTmp, pKeyCipherTmp, KeyCipherLen);			//加密后的密钥
// 		pCache->lDataLen = dwNeeded + KeyCipherLen;				//实际加密后业务层长度
	}
	else
	{
		const auto nCacheSize = sizeof(TRAN_PROTOCOL::BusinessDataCache) + lDataLen;
		gtool::CSmartPtrC<CUXByte, CUXByte, DEL_WAY_DELETE_MULTI> pBusinessDataCache = new CUXByte[nCacheSize];
		TRAN_PROTOCOL::NetCache NetCachePtr(pBusinessDataCache);
		CHECKF(NetCachePtr.m_ptr);
		ZERO_CLEAN_PTR(pBusinessDataCache, nCacheSize);
		memmove(NetCachePtr.m_pBusinessDataCache->rData, pData, lDataLen);
		NetCachePtr.m_pBusinessDataCache->nLen = lDataLen;
		CHECKF(rCacheList.PushBack(NetCachePtr.m_pUXByte));
		pBusinessDataCache.pop();
		nBusinessTotalSize += lDataLen;
	}


	//日志
	//char rizhi[8192] = { 0 };
	//	sprintf(rizhi, "业务层密文：pCache->pData:%s, 长度：%d;其中业务数据长度：%d\r\n加密后的密钥：%s, 长度：%d\r\n加密前的密钥：%s, 长度：%d;\r\n", pDataTmp, pCache->lDataLen, dwNeeded, pKeyCipherTmp, KeyCipherLen, pcbKey, iKeyLentmpd);
	//CWriteDailyLog::WriteLog(rizhi, "yewuceng.txt");

	//测试明文
	//pCache->pData = new char[lDataLen];
	//if (!pCache->pData)
	//{
	//	delete pCache;
	//	return FALSE;
	//}
	//memset(pCache->pData, 0, lDataLen);
	//memmove(pCache->pData, pData, lDataLen);
	//pCache->lDataLen = lDataLen;

	//TCHAR szRizhi[100] = { 0 };
	//TCHAR szResult[200] = { 0 };
	//CWriteDailyLog::AnsiToUnicode((char *)pCache->pData, szRizhi, 100*sizeof(TCHAR));
	//_sntprintf(szResult, 200, TEXT("%s, len:%d, %d"), szRizhi, pCache->lDataLen, m_lCacheDataSize);
	//MessageBox(NULL, szResult, NULL, 0);
	//CWriteDailyLog::WriteDialyLog((char *)pCache->pData);

	//加密
	//if (!DataEncrypt(pData, &lDataLen, &pCache->pData, &pCache->lDataLen))
	//{
	//	if (pCache) delete pCache;
	//	return FALSE;
	//}

	return true;
}

//包分析
bool CNetworkService::CreateSocketPacketToSend(BusinessDataCacheMap& rSendMap, BusinessDataCacheList& rCacheList, const I64 nTotalSize, long &lPackCount)
{
	gtool::CSmartPtr<CNetDataPack> pNetDataPack = CNetDataPack::CreateNew();
	CHECKF(pNetDataPack);

	//变量定义
	long lPackCountTmp = 0;


	//开空间
	gtool::CSmartPtrC<CUXByte, CUXByte, DEL_WAY_DELETE_MULTI> pCacheTmp = new CUXByte[nTotalSize];
	CHECKF(pCacheTmp);
	TRAN_PROTOCOL::NetCache rNetCachePtr(pCacheTmp);
	CHECKF(rNetCachePtr.m_ptr);

	//从缓存中转移数据
	auto IterList = rCacheList.NewEnum();
	while (IterList.Next())
	{
		CUXByte* pCacheData = IterList;
		IF_NOT(pCacheData)
			continue;

		TRAN_PROTOCOL::NetCache rListDataTmp(pCacheData);
		IF_NOT(rListDataTmp.m_ptr)
			continue;

		memmove(rNetCachePtr.m_pUXByte, rListDataTmp.m_pBusinessDataCache->rData, rListDataTmp.m_pBusinessDataCache->nLen);
		rNetCachePtr.m_pUXByte += rListDataTmp.m_pBusinessDataCache->nLen;
	}
	rCacheList.Clear();
	rNetCachePtr.m_ptr = pCacheTmp;

	const auto nTimestamp = 0;
	gtool::CRandGen<U64> rRandNum;
	pNetDataPack->ResetBuffer();
	CHECKF(pNetDataPack->Init(rNetCachePtr.m_ptr, nTotalSize, true, gtool::GetMilliSecTimestamp(), rRandNum, TRAN_PROTOCOL::TRAN_TYPE_MSG, TRAN_PROTOCOL::PACK_TYPE_SEND, m_nPackBatch));

	//循环：
	BUFFER2048 szCacheData = { 0 };
	U64 nSizeDataCache = sizeof(szCacheData);
	while (pNetDataPack->PackNext(szCacheData, nSizeDataCache))
	{

		//一个包完成压入待发包缓冲区
		gtool::CSmartPtrC<CUXByte, CUXByte, DEL_WAY_DELETE_MULTI> pCacheMove = new CUXByte[nSizeDataCache];
		CHECKF(pCacheMove);
		TRAN_PROTOCOL::NetCache rMoveNetCache(pCacheMove);
		rMoveNetCache.m_pBusinessDataCache->nLen = nSizeDataCache;
		memmove(rMoveNetCache.m_pBusinessDataCache->rData, szCacheData, nSizeDataCache);
		CHECKF(rSendMap.AddObj(pNetDataPack->GetCurPackModeSeqNum(), rMoveNetCache.m_pUXByte));
		ASSERTEX(pCacheMove.pop());
	}

	//包总数
	lPackCount = rSendMap.size();

	m_nPackBatch++;

	return true;
}

//发送数据包
bool CNetworkService::SendDataToDest(BusinessDataCacheList& rCacheList, const I64 nTotalSize, const bool bSocketBlockMode /*= false*/, const bool bNeedSocketBlockSet /*= true*/, const bool bNeedReturnPacket /*= true*/, const long lTvPerSec /*= 1*/, const long lTvPerUsec /*= 0*/, const int iTimeOutCountSet /*= TIME_OUT_COUNT*/)
{
	//条件校验
	//是否已经创建socket
	CHECKF(IS_VALID_SOCKET_IDD(m_nSocket));
	long lPackCount = 0;

	gtool::CSmartPtr<BusinessDataCacheMap> pCacheMap = BusinessDataCacheMap::CreateNew();
	CHECKF(pCacheMap);

	//分析并组装包(加密业务层数据)
	CHECKF (CreateSocketPacketToSend(*pCacheMap, rCacheList,nTotalSize,lPackCount));

	//有无数据要发送
	CHECKF(!pCacheMap->empty());

	//设置阻塞模式
	if (bNeedSocketBlockSet)
	{
		if (bSocketBlockMode)
		{
			CHECKF (SetBlockMode());
		}
		else
		{
			CHECKF (SetNoBlockMode());
		}
	}


	//发包过程
	int iTimeOutCount = iTimeOutCountSet;
	bool bRecvReturn = false;

	//发包
	BusinessDataCacheMap::iterator IterMap = pCacheMap->begin();
	for (IterMap = pCacheMap->begin(); IterMap != pCacheMap->end(); ++IterMap)
	{
		//取包
		TRAN_PROTOCOL::NetCache rCache(IterMap->second);
		CHECKF(rCache.m_ptr);

		//超时次数限制
		//iTimeOutCount = iTimeOutCountSet;
		iTimeOutCount = 1;
		bRecvReturn = false;

		//发送一个包			
		while ((iTimeOutCount--)>0)
		{
			bRecvReturn = false;

			//发包
			long nSendSize = rCache.m_pBusinessDataCache->nLen;
			CHECKF(SendData(rCache.m_pBusinessDataCache->rData, &nSendSize));

			//是否需要成功接收包应答
			if (!bNeedReturnPacket) break;

			//超时等待	500ms
			if (DelayMonitorSocket(0, 500000)) continue;

			if (bRecvReturn = IsDestRecvSuccess(IterMap->first, bSocketBlockMode, !bNeedSocketBlockSet))
			{
				break;
			}
		}

		//发送失败判断
		IF_NOT(iTimeOutCount > 0 || bRecvReturn || !bNeedReturnPacket)
			return false;
	}

	return true;

	//(从待发包序列取一个包，并send给服务端，等待服务端发回接收完成应答再发下一个包（若1s内未回复重发）如此循环直到把包发完，最后清空队列)
	//发包结束
}

//对方是否接收成功确认
bool CNetworkService::IsDestRecvSuccess(/*待确认的包序号*/long lCurrentPackSeq, const bool bSocketBlockMode, const bool bNeedSocketBlockSet)
{
	//成功接收应打包功能号 FUNID_RIGHT_RECV
	TRAN_PROTOCOL::NetCache rNetCache(NULL);
	long lRecvPackSize = 0;
	bool bLastAReplyPack = false;

	gtool::CSmartPtr<BusinessDataCacheMap> pDataCacheMap = BusinessDataCacheMap::CreateNew();
	gtool::CSmartPtr<BusinessDataCacheList> pDataOutCacheList = BusinessDataCacheList::CreateNew();
	I64 nRecvTotalSize = 0;
	CHECKF(pDataOutCacheList);
	CHECKF(pDataCacheMap);

	CHECKF (RecvDataFromSrc(*pDataOutCacheList,*pDataCacheMap, bLastAReplyPack, nRecvTotalSize, bSocketBlockMode, bNeedSocketBlockSet, false));

	if (!bLastAReplyPack)
	{
		IF_NOT(GetOutRecvData(*pDataOutCacheList, rNetCache.m_ptr, lRecvPackSize, nRecvTotalSize))
		{
			SAFE_DELETE_MULTI(rNetCache.m_pUXByte);
			rNetCache.m_ptr = NULL;
			return bLastAReplyPack;
		}

		if (lRecvPackSize)
			CHECKF(rNetCache.m_ptr);
	}


	//判断
	SAFE_DELETE_MULTI(rNetCache.m_pUXByte);

	return bLastAReplyPack;
}

//接收数据包
//1.以一个数据包最大限制来接收数据包，
//2.定位包头包尾（一个包的头两个字节为包头，一个包的后两个字节为包尾）
//3.定位包数据长度（包接收的字节数应该与之相符）(断包半包等不予接收，进行超时接收等待,若限定时间内未接收到完整包则断开链接接收失败并清理数据)
//4.定位包序号和总包数（用于计数接收次数）
//5.维护一个去重包序列，已接收的包不需要再接收，但表示接收成功

//压入待接收缓冲区并返回接收成功应答包

//退出while
//检测接收包完整性（包的个数）
//进行拆包并拼接业务层，拼接完成后解密业务层，送入等待接收业务层缓冲区
//完成接收
bool CNetworkService::RecvDataFromSrc(BusinessDataCacheList&rDataOutList, BusinessDataCacheMap&rCacheMap, bool& bLastPackReplyPack, I64&rRecvCacheDataSize, const bool bSocketBlockMode /*= false*/, const bool bNeedSocketBlockSet /*= false*/, const bool bNeedSendSuccessRecvReturnPack /*= TRUE*/, const long lTvPerSec /*= 1*/, const long lTvPerUsec /*= 0*/,const int iTimeOutCountSet /*= TIME_OUT_COUNT*/)
{
	CHECKF(IS_VALID_SOCKET_IDD(m_nSocket));
	gtool::CSmartPtr<CNetDataPack> pNetDataPack = CNetDataPack::CreateNew();
	CHECKF(pNetDataPack);

	//设置阻塞模式
	if (bNeedSocketBlockSet)
	{
		if (bSocketBlockMode)
		{
			CHECKF (SetBlockMode());
		}
		else
		{
			CHECKF (SetNoBlockMode());
		}
	}


	//接收包
	BUFFER2048 szBufferRecv = { 0 };
	long lRecvDataLen = 0;
	bool bRecvSuccess = false;
	int iCountTimeOut = iTimeOutCountSet;
	//	int iCountSuccessRecv = 0;		//成功接收总数
	unsigned long ulPacketNum = 0;
	unsigned long ulPacketSeq = 0;

	CNetDataPack rDataPackReply;
	BUFFER2048 szReplyBuffer = { 0 };
	gtool::CRandGen<U64> gRandor;


	rRecvCacheDataSize = 0;		//待

	long lCountSuccess = 0;
	long lCountLen = 0;
	TRAN_PROTOCOL::NetCache rNetCache(NULL);
	while (iCountTimeOut-->0)
	{
		//接收
		lRecvDataLen = PACKAGE_SIZE_MAX;
		bRecvSuccess = false;
		ulPacketNum = 0;
		ulPacketSeq = 0;
		ZERO_CLEAN_PTR(szBufferRecv, sizeof(szBufferRecv));

		//超时接收
		if (DelayMonitorSocket(lTvPerSec, lTvPerUsec)) continue;
		bRecvSuccess = RecvData(szBufferRecv, &lRecvDataLen);
		IF_NOT (bRecvSuccess) continue;

		IF_NOT(pNetDataPack->Init(szBufferRecv, lRecvDataLen, false))
			continue;

		U64 nSizeRecvOut = 0;
		TRAN_PROTOCOL::PackAccess *pAsOut = NULL;
		I64 nBatchKey = 0;
		rNetCache.m_ptr = NULL;	//若是回复确认包则没有数据
		IF_NOT(pNetDataPack->Unpack(rNetCache.m_ptr, nSizeRecvOut, pAsOut, nBatchKey))
			continue;

		IF_NOT(pAsOut)
			continue;

		//IF_NOT(nSizeRecvOut)
		//	continue;

		//校验包长度
		//不返回成功应打包客户端会超时重发
		IF_NOT (pAsOut->nPackSize == lRecvDataLen) continue;

		//包总数
		ulPacketNum = pAsOut->nTotalNum;

		//包序号
		ulPacketSeq = pAsOut->nSeqNum;

		//检测是否重包
		auto pDataCacheToRecv = rCacheMap.GetObj(ulPacketSeq);
		IF_NOT(!pDataCacheToRecv)
		{
			if (bNeedSendSuccessRecvReturnPack)
			{
				rDataPackReply.ResetBuffer();
				IF_NOT(rDataPackReply.Init(NULL, 0, true, gtool::GetMilliSecTimestamp(), gRandor, TRAN_PROTOCOL::TRAN_TYPE_MSG, TRAN_PROTOCOL::PACK_TYPE_REPLY, pAsOut->nPackBatchID))
					continue;
				
				U64 u64Size2Reply = 0;
				u64Size2Reply = sizeof(szReplyBuffer);
				IF_NOT(rDataPackReply.PackNext(szReplyBuffer, u64Size2Reply))
					continue;
				
				gtool::CSmartPtr<BusinessDataCacheList> pCacheList = BusinessDataCacheList::CreateNew();	//业务层数据缓冲
				CHECKF(pCacheList);

				I64 nBusinessTotalSize = 0;
				IF_NOT(AddDataToSend(szReplyBuffer, u64Size2Reply, *pCacheList, nBusinessTotalSize))
						continue;

				IF_NOT(SendDataToDest(*pCacheList, nBusinessTotalSize, false, !bNeedSocketBlockSet, false))
					continue;
			}

			//复位超时次数
			iCountTimeOut = iTimeOutCountSet;
			continue;
		}

		//开空间保存业务逻辑层数据
		const I32 nSaveSize = sizeof(TRAN_PROTOCOL::BusinessDataCache) + nSizeRecvOut;
		gtool::CSmartPtrC<CUXByte, CUXByte, DEL_WAY_DELETE_MULTI> pDataCacheSave = new CUXByte[nSaveSize];
		CHECKF(pDataCacheSave);
		TRAN_PROTOCOL::NetCache rNetDataSaveCache(pDataCacheSave);
		CHECKF(rNetDataSaveCache.m_ptr);
		ZERO_CLEAN_PTR(rNetDataSaveCache.m_ptr, nSaveSize);
		rNetDataSaveCache.m_pBusinessDataCache->nLen = nSizeRecvOut;
		if(nSizeRecvOut)
			memmove(rNetDataSaveCache.m_pBusinessDataCache->rData, rNetCache.m_ptr, nSizeRecvOut);
		bLastPackReplyPack = (pAsOut->nPackType == TRAN_PROTOCOL::PACK_TYPE_REPLY);

		//压入待接收socket缓冲
		IF_OK(!rCacheMap.IsExist(ulPacketSeq))
		{
			rRecvCacheDataSize += rNetDataSaveCache.m_pBusinessDataCache->nLen;
			IF_OK(rCacheMap.AddObj(ulPacketSeq, rNetDataSaveCache.m_pUXByte))
			{
				ASSERTEX(pDataCacheSave.pop());

				bool  bRet = false;
				if (bNeedSendSuccessRecvReturnPack)
				{
					rDataPackReply.ResetBuffer();
					IF_NOT(rDataPackReply.Init(NULL, 0, true, gtool::GetMilliSecTimestamp(), gRandor, TRAN_PROTOCOL::TRAN_TYPE_MSG, TRAN_PROTOCOL::PACK_TYPE_REPLY, pAsOut->nPackBatchID))
						continue;

					U64 u64Size2Reply = 0;
					u64Size2Reply = sizeof(szReplyBuffer);
					IF_NOT(rDataPackReply.PackNext(szReplyBuffer, u64Size2Reply))
						continue;

					gtool::CSmartPtr<BusinessDataCacheList> pCacheList = BusinessDataCacheList::CreateNew();	//业务层数据缓冲
					CHECKF(pCacheList);

					I64 nBusinessTotalSize = 0;

					IF_NOT(AddDataToSend(szReplyBuffer, u64Size2Reply, *pCacheList, nBusinessTotalSize))
						continue;

					IF_NOT(SendDataToDest(*pCacheList, nBusinessTotalSize, false, !bNeedSocketBlockSet, false))
						continue;
				}

				//接收成功重置
				iCountTimeOut = iTimeOutCountSet;
			}

			//检测是否接收完成
			if (rCacheMap.size() == ulPacketNum)
				break;
		}
	}

	CHECKF(bRecvSuccess);

	//拼接接收到的业务层数据
	IF_NOT(CreateBussnessLayerToGet(rDataOutList, rCacheMap, rRecvCacheDataSize))
	{
		rCacheMap.Cleans();
		return false;
	}

	//清理socket等待接收缓冲区
	//	ClearSocketPackCacheToRecv();

	return true;
}

//解包(并解密)将socket待接收缓冲中数据拼接成业务层数据 得到一个业务逻辑层数据
bool CNetworkService::CreateBussnessLayerToGet(BusinessDataCacheList&rDataOutList, BusinessDataCacheMap&rCacheMap, I64&rRecvCacheDataSize)
{
	//参数校验
	if (rRecvCacheDataSize <= 0)
	{
		rCacheMap.Cleans();
		return true;
	}
	CHECKF(!rCacheMap.empty() && rRecvCacheDataSize);

	//开空间
	gtool::CSmartPtrC<CUXByte, CUXByte, DEL_WAY_DELETE_MULTI> pBusinessLayerData = new CUXByte[rRecvCacheDataSize];
	CHECKF(pBusinessLayerData);
	TRAN_PROTOCOL::NetCache rNetCache(pBusinessLayerData);
	CHECKF(rNetCache.m_ptr);
	ZERO_CLEAN_PTR(pBusinessLayerData, rRecvCacheDataSize);

	//按顺序取数据 size，从0开始到size
	long lBusinessTotalSize = 0;
	for (BusinessDataCacheMap::iterator IterMap = rCacheMap.begin(); IterMap != rCacheMap.end(); ++IterMap)
	{
		CUXByte *pCacheTmp = IterMap->second;
		CHECKF(pCacheTmp);
		TRAN_PROTOCOL::NetCache rCacheTmp(pCacheTmp);
		CHECKF(rCacheTmp.m_ptr);
		if(rCacheTmp.m_pBusinessDataCache->nLen)
			memmove(rNetCache.m_ptr, rCacheTmp.m_pBusinessDataCache->rData, rCacheTmp.m_pBusinessDataCache->nLen);
		rNetCache.m_pUXByte += rCacheTmp.m_pBusinessDataCache->nLen;
		lBusinessTotalSize += rCacheTmp.m_pBusinessDataCache->nLen;
	}
	rCacheMap.Cleans();

	//校验数据完整性
	IF_NOT(lBusinessTotalSize == rRecvCacheDataSize)
		return false;

	if (m_bSetTransferSecurity)
	{
		CHECKF(false);
// 		//解密
// 		CRsaEngine RsaSystem;
// 		//设置私钥
// 		RsaSystem.SetPrivateKey((unsigned char *)m_strRsaPrivateKey.c_str());
// 		CDESEngine DesSystem;
// 		//寻找密钥 最后的 CIPHER_TEXT_OUT_ENCODE_LEN 长度
// 		BYTE *pcbKeyCipher = (BYTE *)(((char *)pBusinessLayerData) + m_lRecvCacheDataSize - CIPHER_TEXT_OUT_ENCODE_LEN);
// 		BYTE cbKeyPlain[DES_KEY_BIT + 1] = { 0 };
// 		BYTE *pcbKeyPlainTmp = cbKeyPlain;
// 		unsigned long ulKeyPlainSize = DES_KEY_BIT + 1;
// 		//解密密钥
// 		RsaSystem.Decrypte(pcbKeyCipher, CIPHER_TEXT_OUT_ENCODE_LEN, &pcbKeyPlainTmp, &ulKeyPlainSize);
// 
// 		//开空间放解密后的明文文
// 		unsigned char *pPlainDataTmp = new unsigned char[m_lRecvCacheDataSize - CIPHER_TEXT_OUT_ENCODE_LEN];
// 		if (!pPlainDataTmp)
// 		{
// 			delete[]pBusinessLayerData;
// 			return FALSE;
// 		}
// 		memset(pPlainDataTmp, 0, m_lRecvCacheDataSize - CIPHER_TEXT_OUT_ENCODE_LEN);
// 		DWORD dwPlainDataLen = m_lRecvCacheDataSize - CIPHER_TEXT_OUT_ENCODE_LEN;
// 
// 		//解密数据
// 		DesSystem.Decrypte(cbKeyPlain, ulKeyPlainSize, (BYTE *)pBusinessLayerData, m_lRecvCacheDataSize - CIPHER_TEXT_OUT_ENCODE_LEN, pPlainDataTmp, &dwPlainDataLen);
// 
// 		//开空间存放解密后的数据
// 		pBusinessDataCache->pData = new char[dwPlainDataLen];
// 		if (!pBusinessDataCache->pData)
// 		{
// 			delete[]pBusinessLayerData;
// 			delete[]pPlainDataTmp;
// 			return FALSE;
// 		}
// 		memset(pBusinessDataCache->pData, 0, dwPlainDataLen);
// 		memmove(pBusinessDataCache->pData, pPlainDataTmp, dwPlainDataLen);
// 		pBusinessDataCache->lDataLen = dwPlainDataLen;
// 		delete[]pPlainDataTmp;
	}
	else
	{
		//无加密
// 		pBusinessDataCache->pData = new char[m_lRecvCacheDataSize];
// 		if (!pBusinessDataCache->pData)
// 		{
// 			delete[]pBusinessLayerData;
// 			return FALSE;
// 		}
// 		memset(pBusinessDataCache->pData, 0, m_lRecvCacheDataSize);
// 		memmove(pBusinessDataCache->pData, pBusinessLayerData, m_lRecvCacheDataSize);
// 		pBusinessDataCache->lDataLen = m_lRecvCacheDataSize;
	}
	rNetCache.m_ptr = pBusinessLayerData;

	//将数据送入业务层缓冲区
	IF_OK(rNetCache.m_ptr)
	{
		CHECKF(rDataOutList.PushBack(rNetCache.m_pUXByte));
		ASSERTEX(pBusinessLayerData.pop());
	}

	return true;
}

//取出接收到的数据
bool CNetworkService::GetOutRecvData(BusinessDataCacheList&rDataOutList, void *&pData, long &lDataLen, I64&rRecvCacheDataSize)
{
	//参数校验
	if (rRecvCacheDataSize <= 0)
	{
		rDataOutList.Clear();
		return true;
	}		

	CHECKF(!rDataOutList.IsEmpty());
	pData = new CUXByte[rRecvCacheDataSize];
	CHECKF(pData);
	lDataLen = 0;

	//取数据
	//开空间
	//数据迁移
	//清理空间
	//完成
	TRAN_PROTOCOL::NetCache rCachePtr(pData);
	CHECKF(rCachePtr.m_ptr);
	auto IterList = rDataOutList.NewEnum();
	while (IterList.Next())
	{
		CUXByte* pDataCache = IterList;
		CHECKF(pDataCache);
		TRAN_PROTOCOL::NetCache rDataCache(pDataCache);
		memmove(rCachePtr.m_ptr, rDataCache.m_pBusinessDataCache->rData, rDataCache.m_pBusinessDataCache->nLen);
		rCachePtr.m_pUXByte += rDataCache.m_pBusinessDataCache->nLen;
		lDataLen += rDataCache.m_pBusinessDataCache->nLen;
	}
	rDataOutList.Clear();
	return true;
}

//设置公钥
void CNetworkService::SetRsaPublicKey(unsigned char *ucKey)
{
	if (!ucKey) return;
	m_bSetTransferSecurity = true;
	m_strRsaPublicKey.clear();
	m_strRsaPublicKey = (char *)ucKey;
}

//设置私钥
void CNetworkService::SetRsaPrivateKey(unsigned char *ucKey)
{
	if (!ucKey) return;
	m_bSetTransferSecurity = true;
	m_strRsaPrivateKey.clear();
	m_strRsaPrivateKey = (char *)ucKey;
}


//清空缓冲区数据队列 清除所有区数据
bool CNetworkService::ClearAllDataCache()
{
	m_bIsDomainName = true;


	return true;
}

/////////////////////////////////////////////////////////////
//tcp客户端
CNetworkTcpClient *g_pTcpClientMgr = NULL;
boost::atomic_bool CNetworkTcpClient::m_bDestroy{ false };
CNetworkTcpClient::CNetworkTcpClient()
{
	m_pThreadPool = CThreadPool::CreateNew(0, 1);
}

CNetworkTcpClient::~CNetworkTcpClient()
{
}

void CNetworkTcpClient::Cleans()
{
}


//链接服务器
bool CNetworkTcpClient::Link(const bool bClearDataCache)
{
	//创建套接字
	//连接

	//启动网络服务
	CHECKF(StartNetService(bClearDataCache));

	return Connect();
}

//关闭链接
bool CNetworkTcpClient::CloseLink()
{
	return CloseNetService();
}

//向服务器发送数据
bool CNetworkTcpClient::SendDataTo(const void *pData, const long lDataLen)
{
	//参数校验
	CHECKF(pData);

	gtool::CSmartPtr<BusinessDataCacheList> pCacheList = BusinessDataCacheList::CreateNew();	//业务层数据缓冲
	CHECKF(pCacheList);

	I64 nBusinessTotalSize = 0;
	CHECKF(AddDataToSend(pData, lDataLen, *pCacheList, nBusinessTotalSize));

	bool bSend = SendDataToDest(*pCacheList, nBusinessTotalSize,false, true, true);

	return bSend;
}

//从服务器获取数据
bool CNetworkTcpClient::RecvDataFrom(void *&pData, long &lDataLen)
{
	//参数校验
	bool bLastAReplyPack = false;
	gtool::CSmartPtr<BusinessDataCacheMap> pDataCacheMap = BusinessDataCacheMap::CreateNew();
	gtool::CSmartPtr<BusinessDataCacheList> pDataOutCacheList = BusinessDataCacheList::CreateNew();
	I64 nRecvTotalSize = 0;
	CHECKF(pDataOutCacheList);
	CHECKF(pDataCacheMap);
	CHECKF(RecvDataFromSrc(*pDataOutCacheList, *pDataCacheMap,bLastAReplyPack, nRecvTotalSize));

	return GetOutRecvData(*pDataOutCacheList, pData, lDataLen, nRecvTotalSize);
}

//更新配置（必须断开链接）
bool CNetworkTcpClient::RefreshSvrAddr(const char *host/*主机*/, int hostlen/*长度*/, unsigned short port/*端口*/, bool bIsDomainName/*是否域名*/)
{
	CHECKF(host&&hostlen > 0);
	if (!m_strServerAddr.empty()) m_strServerAddr.clear();
	ASSERTEX(CloseNetService());
	m_strServerAddr = host;
	m_ServerPort = port;
	m_bIsDomainName = bIsDomainName;
	
	if (m_bIsDomainName)
	{
		CHECKF(SetAddrInfo(host, strlen(host), port, AF_INET, true, true));
	}
	else
	{
		CHECKF(SetAddrInfo(host, strlen(host), port, AF_INET, true, true, true));
	}	

	return true;
}


void CNetworkTcpClient::GetSvrAddr(void *pBuffer, const U16 nBufferSize)
{
	CHECK(pBuffer&&nBufferSize);
	const char *ip = m_SocketAddr;
	const U16 nPort = m_SocketAddr;
	_snprintf((char *)pBuffer, nBufferSize - 1, "%s:%hu", ip, nPort);
}

bool CNetworkTcpClient::Init()
{
	CHECKF(CTcpSocket::Init());
	CHECKF(InitSocketEnvironment());

	m_bInit = true;
	m_bDestroy = false;
	return true;
}

void CNetworkTcpClient::Fini()
{
	m_bInit = false;
	if (m_bDestroy)
		return;

	m_bDestroy = true;

	//消耗线程池
	IF_OK(m_pThreadPool)
		m_pThreadPool->Destroy();

	//关闭套接字
	CloseLink();
}

bool CNetworkTcpClient::StartMonitor()
{
	CHECKF(m_pThreadPool);
	CHECKF(m_bInit);

	gtool::CSmartPtr<CTaskBase> pMonitorTask = CTcpClientReciever::CreateNew();
	CHECKF(pMonitorTask);
	CHECKF(m_pThreadPool->AddTask(*pMonitorTask));
	ASSERTEX(pMonitorTask.pop());

	return true;
}

bool CNetworkTcpClient::Lock()
{
	CHECKF(m_pLocker);
	return m_pLocker->Lock();
}

bool CNetworkTcpClient::Unlock()
{
	CHECKF(m_pLocker);
	return m_pLocker->UnLock();
}

//链接远程
bool CNetworkTcpClient::Connect()
{
	//合法验证
	CHECKF(IS_VALID_SOCKET_IDD(m_nSocket));
	sockaddr_in *sin = (m_SocketAddr);
	CHECKF(sin);

	//切换套接字模式
	CHECKF(SetNoBlockMode());

	//链接
	bool bConnectTimeOut = true;
	WORD i = 0;
	for (i = 0; i < TIME_OUT_COUNT; i++)
	{
		if (!(SOCKET_ERROR == connect(m_nSocket, (struct sockaddr *)sin, sizeof(sockaddr_in)) && WSAGetLastError() == WSAEWOULDBLOCK))
		{
		}

		int ret = 1;
		bConnectTimeOut = true;
		if (DelayMonitorSocket(0, 500000, false, true, &ret, false))
			continue;	//超时为500毫秒

		//判断是否链接成功
		int optval = -1;
		int optlen = sizeof(optval);
		int retget = getsockopt(m_nSocket, SOL_SOCKET, SO_ERROR, (char*)(&optval), &optlen);
		if (retget != 0 || optval != 0)
		{
			bConnectTimeOut = true;
		}

		if (ret <= 0 && (i == (TIME_OUT_COUNT - 1)))
			break;
			
		if (ret > 0)
		{
			bConnectTimeOut = false;
			break;
		}
	}

	//切换套接字模式
	//SetBlockMode();

	//判断超时
	if (bConnectTimeOut)
	{
		if (m_bAutoCloseSocket)
			Close();

		return false;
	}

	m_bConnected = true;

	return m_bConnected;
}

/////////////////////////////////////////////////////////////
