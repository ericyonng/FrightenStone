#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#pragma once


#undef TIME_OUT_COUNT
#define TIME_OUT_COUNT				10		//超时次数（每次一秒，一秒等待后重复操作）



/////////////////////////////////////////////////////////////////////////////

//WSAStartup的目的是初始化winsock DLL，每个应用程序调用一次就行了吧，不用每个线程调用一次吧
//操作成功返回值为0; 否则返回值为SOCKET_ERROR，可以通过调用WSAGetLastError获取错误代码。

//在一个多线程的环境下，WSACleanup()中止了Windows Sockets在所有线程上的操作.
//定义地址信息结构体
class  CTcpSocket
{
	NO_COPY(CTcpSocket);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CTcpSocket);

	//接口功能
public:
	virtual bool Init();
	//设置地址
	virtual void SetDestAddr(void *pAddr);
	//发送
	virtual bool SendDataTo(const void *pData, const long lDataLen) { return false; }
	//接收
	virtual bool RecvDataFrom(void **pData, long *plDataLen) { return false; }

	//辅助功能
public:
	//设置地址
	bool SetAddrInfo(CAddr &addr);
	//设置地址
	bool SetAddrInfo(const char *domain, int domainlen, unsigned short port, unsigned short family, bool bUseBind, bool bSocketStream);
	//设置地址
	bool SetAddrInfo(const char *ip, int iplen, unsigned short port, unsigned short family, bool bUseBind, bool bSocketStream, bool bIp);
	//清空socket缓冲区
	bool ClearSocketCache();
	//设置套接字
	void SetSocketFD(SOCKET hSock);
	//获取套接字
	SOCKET GetSocketFD() { return m_nSocket; }
	//设置自动关闭套接字模式
	void SetAutoCloseSocket(bool bAutoClose) { m_bAutoCloseSocket = bAutoClose; }
	//流量统计
	//TRAN_PROTOCOL::DATA_FLOW_COUNT_T &GetFlowCount() { return m_DataFlowCount; }

	//peer方获取属性
public:
	//获取对方ip端口
	int GetPeerIP(char *peerip, int iPeerIPSize, unsigned short *peerport);
	//获取本地地址端口 0表示成功 其他为错误值
	int GetLocalIP(char *localipaddr, int ipaddrsize, unsigned short *localport);

	//win32dll 网络环境初始化
public:
	//启动socket环境
	static bool InitSocketEnvironment();
	//清理环境
	static bool CleanSocketEnvironment();

	//套接字操作
protected:
	//阻塞模式
	bool SetBlockMode();
	//非阻塞模式
	bool SetNoBlockMode();
	//套接字超时监控
	bool DelayMonitorSocket(long tv_sec, long tv_usec, bool bReadableDetect = true, bool bWriteableDetect = false, int *pErrOut = NULL, bool bSetOneAtLeast = false, bool bInfiniteWaiting = false);
	//重置
	VOID Clear();

public:
	//链接状态
	bool GetConnectStatus() { return m_bConnected; }
	fd_set&GetReadFdSet() { return m_fs_read; }
	fd_set&GetWriteFdSet() { return m_fs_write; }

	//网络操作
protected:
	//套接字
	virtual bool CreateSocket();

	//断开链接
	virtual bool Close();
	//优雅的断开连接
	virtual bool GraciousClose(int iHowto);
	//发送数据
	virtual bool SendData(void *pData, long *plDataLen);
	//接收数据
	virtual bool RecvData(void *pData, long *plDataLen);

	//网络链接状态
	bool GetNetConnectStatus() { return m_bConnected; }



protected:

																	//必要数据
protected:
	boost::atomic<SOCKET>		m_nSocket{INVALID_SOCKET};			//套接字
	boost::atomic_bool			m_bAutoCloseSocket{ true };			//自动关闭套接字
	boost::atomic_bool			m_bConnected{ false };				//链接状态
	fd_set						m_fs_read;							//可读检测
	fd_set						m_fs_write;							//可写检测
	boost::atomic_bool			m_bSocketReadable{ false };			//可读
	boost::atomic_bool			m_bSocketWriteable{ false };		//可写
	CAddr						m_SocketAddr;						//地址
	gtool::CSmartPtr<CDataLocker> m_pLocker;						//共享锁

																	//流量统计
protected:
//	boost::atomic<TRAN_PROTOCOL::DATA_FLOW_COUNT_T>		m_DataFlowCount;					//流量统计

};

////////////////////////////////////////////////////////////////


//网络服务 只发送一个包
class CNetworkService :public CTcpSocket
{
	NO_COPY(CNetworkService);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CNetworkService);
public:
	typedef gtool::TPtrList<CUXByte, DEL_WAY_DELETE_MULTI, false> BusinessDataCacheList;
	typedef gtool::CLitPtrMap<CUXByte, I32, DEL_WAY_DELETE_MULTI> BusinessDataCacheMap;	//key:nSeqNum 包序号

	//外部接口
public:
	//安全传输使能
	void SetTransferSecurity(bool bSet) { m_bSetTransferSecurity = bSet; }
	//安全传输特性
	bool GetTransferSecurityStatus() { return m_bSetTransferSecurity; }

	//内部接口

	//服务器链接
protected:
	//启动网络服务
	virtual bool StartNetService(const bool bClearDataCache = true);

	//断开链接
	virtual bool	CloseNetService();

	//发送模块
protected:
	//接收模块
	//业务逻辑层数据添加 添加之前加密业务层数据
	virtual bool AddDataToSend(const void *pData, const long lDataLen, BusinessDataCacheList& rCacheList, I64& nBusinessTotalSize);
	//发送数据包
	virtual bool SendDataToDest(BusinessDataCacheList& rCacheList, const I64 nTotalSize, const bool bSocketBlockMode = false, const bool bNeedSocketBlockSet = true, const bool bNeedReturnPacket = true,const long lTvPerSec = 1, const long lTvPerUsec = 0, const int iTimeOutCountSet = TIME_OUT_COUNT);
	//对方是否接收成功确认
	virtual bool IsDestRecvSuccess(/*待确认的包序号*/long lCurrentPackSeq, const bool bSocketBlockMode, const bool bNeedSocketBlockSet);
	//包创建 一个包中的业务层数据由 BUSINESS_DATA_LIMIT限制
	virtual bool CreateSocketPacketToSend(BusinessDataCacheMap& rSendMap, BusinessDataCacheList& rCacheList, const I64 nTotalSize, long &lPackCount);

	//接收模块
protected:
	//取出接收到的数据
	virtual bool GetOutRecvData(BusinessDataCacheList&rDataOutList, void *&pData, long &lDataLen, I64&rRecvCacheDataSize);
	//接收数据包
	virtual bool RecvDataFromSrc(BusinessDataCacheList&rDataOutList, BusinessDataCacheMap&rCacheMap, bool& bLastPackReplyPack, I64&rRecvCacheDataSize, const bool bSocketBlockMode = false, const bool bNeedSocketBlockSet = false, const bool bNeedSendSuccessRecvReturnPack = TRUE,const long lTvPerSec = 1,const long lTvPerUsec = 0, const int iTimeOutCountSet = TIME_OUT_COUNT);
	//解包(并解密)将socket待接收缓冲中数据拼接成业务层数据 得到一个业务逻辑层数据
	virtual bool CreateBussnessLayerToGet(BusinessDataCacheList&rDataOutList, BusinessDataCacheMap&rCacheMap, I64&rRecvCacheDataSize);

	//rsa key设置
protected:
	//设置公钥
	void SetRsaPublicKey(unsigned char *ucKey);
	//设置私钥
	void SetRsaPrivateKey(unsigned char *ucKey);

	//清理组
protected:
	bool ClearAllDataCache();

	//rsa密钥设置
protected:
	std::string					m_strRsaPublicKey;				//公钥
	std::string					m_strRsaPrivateKey;				//私钥
	bool					m_bSetTransferSecurity;			//是否设置传输安全

															//业务层待发送数据缓冲区 1区
protected:
//	long					m_lCacheDataSize;				//业务逻辑层数据总长度
	boost::atomic<I64>		m_nPackBatch{ 0 };				//包批次
//	gtool::CSmartPtr<CNetDataPack> m_pNetDataPack;			//封包器


//	gtool::CSmartPtr<BusinessDataCacheList> m_pBusinessDataCacheList;	//业务层数据缓冲

															//业务层待接收数据缓冲区	2区
protected:
//	long					m_lRecvCacheDataSize;				//业务逻辑层数据总长度
//	gtool::CSmartPtr<BusinessDataCacheList> m_pRecvCacheDataQueue;	//业务层数据缓冲		
	//CUXByte *					m_pRecvDataOut;						//取出来的数据需要释放 要加锁

																//socket包缓冲 3区
protected:

//	gtool::CSmartPtr<BusinessDataCacheMap> m_pBusinessDataCacheToSendMap;	//发送缓冲区 用于发送失败等重发 key为包序号
//	gtool::CSmartPtr<BusinessDataCacheMap> m_pBusinessDataCacheToRecvMap;	//接收缓冲区 用于接收数据包 key为包序号


																//目标属性
protected:
	std::string				m_strServerAddr;			//目的地址
	unsigned short		m_ServerPort;				//目的端口
	bool				m_bIsDomainName;			//目的是否域名

};

//客户端 需要设置公私钥
class CNetworkTcpClient :public CNetworkService
{
	NO_COPY(CNetworkTcpClient);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CNetworkTcpClient);

	//网络服务
public:
	//链接服务器
	virtual bool Link(const bool bClearDataCache = true);
	//关闭链接
	virtual bool CloseLink();
	//发送
	virtual bool SendDataTo(const void *pData, const long lDataLen);
	//接收 接收后需要手动释放空间
	virtual bool RecvDataFrom(void *&pData, long &lDataLen);
	//更新配置（必须断开链接）
	virtual bool RefreshSvrAddr(const char *host/*主机*/, int hostlen/*长度*/, unsigned short port/*端口*/, bool bIsDomainName/*是否域名*/);
	//rsa	key :本地保存server端可以解密的加密锁，以及一把专用于客户端的解密锁，服务端保存客户端可以解密的加密锁以及专用于服务端的解密锁
	void GetSvrAddr(void *pBuffer, const U16 nBufferSize);	//地址字符串

	bool Init();
	void Fini();

	bool IsInit() { return m_bInit; }
	bool IsDestroy() { return m_bDestroy; }

	//监听服务器消息
	bool StartMonitor();
	bool Lock();
	bool Unlock();

protected:
	//链接远程
	virtual bool Connect();
	
protected:
	boost::atomic_bool m_bInit{ false };
	static boost::atomic_bool m_bDestroy;

	gtool::CSmartPtr<CThreadPool> m_pThreadPool;
};

typedef gtool::CSingleton<CNetworkTcpClient> CNetworkTcpClientInstance;
extern CNetworkTcpClient *g_pTcpClientMgr;
#define TcpClientMgr  ( (g_pTcpClientMgr = CNetworkTcpClientInstance::GetInstance()), ASSERT(g_pTcpClientMgr), *g_pTcpClientMgr)

//服务器


#endif

