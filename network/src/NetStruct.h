#ifndef NET_STRUCT_H_
#define NET_STRUCT_H_

#pragma once

#include <BaseCode/NetworkServiceApi.h>
//#include <BaseCode/MemPoolObj.h>

#pragma MEM_ALIGN_BEGIN(1)

#ifndef _WS2DEF_

typedef struct _WSABUF {
	ULONG len;     /* the length of the buffer */
	_Field_size_bytes_(len) CHAR FAR *buf; /* the pointer to the buffer */
} WSABUF, FAR * LPWSABUF;

#endif

typedef I64 PACK_UNIQUE_ID;		//包唯一标识 nPackBatchID 包的批次
struct SocketMsgInfo
{
	SOCKET_IDD idSocket;		//套接字唯一id
	SOCKET_STATUS_TYPE eType;	//套接字状态
	TRAN_PROTOCOL::TRAN_TYPE eTranType;	//协议类型
	I64		 nPackBatchID;		//批次
};

typedef gtool::TPtrList<SocketMsgInfo, DEL_WAY_DELETE, false> SocketMsgInfoList;	//套接字消息信息队列

//地址信息类
struct NETWORK_SERVICE_API CAddr
{
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CAddr)
	MEMPOOL_CREATE(m_MemPoolAlloctor)

	//资源创建与释放
public:
	CAddr(char *szip, unsigned short usPort, unsigned short usFamily = AF_INET);
	CAddr(const SOCKADDR_IN& rOther);
	void Reset();	//复位

	//配置函数
public:
	//重载赋值运算符
	CAddr & operator =(const CAddr &addr);	//
	CAddr & operator =(CAddr &addr);
	 operator const struct sockaddr *() const;	//获取旧版地址
	 operator const struct sockaddr_in *() const;	//获取新版地址
	 operator struct sockaddr *();	//获取旧版地址
	 operator struct sockaddr_in *();	//获取新版地址
	 operator const char *() const;			//获取ip
	 operator const U16() const;			//获取端口号
	
	 //设置值
public:
	void SetAddrInfoEx(const struct sockaddr_in &stAddr_Info);	//设置地址类结构体
	void SetAddrInfo(const unsigned short family, const char *szip = NULL
		,const char *domainname = NULL, bool bUserIp = true
		, const unsigned short* port = NULL, bool bUseBind =true
		, bool bUserSockStream = true);		//地址

	//读取信息
public:	
	const char *GetDomainName() const;					//域名	
	U16 GetFamily() const;				//获取网络协议类型	

protected:
	bool GetAddrInfo(bool bReload = false) const;

protected:
	mutable SOCKADDR_IN				m_stAddr_Info;						//地址类信息结构体新版
	BUFFER1024						m_DomainName;						//域名
	mutable BUFFER128				m_szCache;							//缓冲 脏数据
	U16								m_uCache;							//缓冲 脏数据
};

//====================================================================================
//
//				单IO数据结构体定义(用于每一个重叠操作的参数)
//
//====================================================================================

//最好堆区创建
typedef NETWORK_SERVICE_API struct _RW_CONTEXT
{
	OVERLAPPED     m_Overlapped;                               // 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	WSABUF         m_wsaBuf;                                   // WSA类型的缓冲区，用于给重叠操作传参数的
	char           m_szBuffer[MAX_BUFFER_LEN];                 // 这个是WSABUF里具体存字符的缓冲区
	U64			   m_u64DataToHandle;							//待处理的数据长度
	U16			   m_u16FailHandleCount;						//失败次数
	OPERATION_TYPE m_OpType;                                   // 标识网络操作的类型(对应上面的枚举)
	void *			m_pParrentObj = NULL;							//当前节点所属的 _PER_SOCKET_CONTEXT
	PACK_UNIQUE_ID  m_idPackBatchID = -1;							//包批次
	I64				m_nSeqNum = -1;								//包序号
	TRAN_PROTOCOL::TRAN_TYPE m_nTranType = TRAN_PROTOCOL::TRAN_TYPE_MSG;	//包协议

	//创建与释放
	_RW_CONTEXT();
	~_RW_CONTEXT();
	bool operator==(const _RW_CONTEXT& Other);	//操作符重载

	
	void ResetBuffer();	// 重置缓冲区内容
	SOCKET GetSocket();	//获取套接字
	const SOCKADDR_IN* GetAddr();	//获取套接字地址信息
	const CAddr* GetAddrObj();
	const char *GetIp();
	const U16 GetPort();
	MEMPOOL_CREATE(m_MemPoolAlloctor);	//创建于内存池

} PER_RW_CONTEXT,  *PPER_RW_CONTEXT;




//====================================================================================
//
//				单句柄数据结构体定义(用于每一个完成端口，也就是每一个Socket的参数)
//
//====================================================================================


typedef gtool::TPtrList<_RW_CONTEXT,DEL_WAY_DELETE, false> NetRWContextList;	//套接字读写上下文List

typedef gtool::CLitPtrMap<char, I32, DEL_WAY_DELETE_MULTI> PackDataMap;		//网络数据 必须使用 NetDataStream key:nSeqNum

struct PackDataMapInfo
{
	PackDataMapInfo()
	{
		m_pPackDataMap = PackDataMap::CreateNew();
		ASSERTEX(m_pPackDataMap);
	}
	~PackDataMapInfo() {}

	I32 Release()
	{
		IF_OK(m_pPackDataMap)
			m_pPackDataMap->Cleans();

		return 0;
	}

	gtool::CSmartPtr<PackDataMap> m_pPackDataMap = NULL;
	I64 m_nTotalByte = 0;
};

typedef gtool::CLitPtrMap<PackDataMapInfo, U16, DEL_WAY_RELEASE> TransTypePackDataMap;	//key: TRAN_TYPE val:PackDataMap
typedef gtool::CLitPtrMap<TransTypePackDataMap, PACK_UNIQUE_ID, DEL_WAY_RELEASE> AllPackDataWaitHandleMap;		//所有完整的网络包数据 key:PACK_UNIQUE_ID socket生命周期唯一

typedef NETWORK_SERVICE_API struct  _PER_SOCKET_CONTEXT
{
protected:
	SOCKET      m_Socket;                                  // 每一个客户端连接的Socket
	CAddr		m_Addrinfo;									//地址信息

public:	
	gtool::CSmartPtr<NetRWContextList> m_pListReadContext;             // 向客户端读操作上下文数据，
	gtool::CSmartPtr<NetRWContextList> m_pListWriteContext;				//向客户端写操作上下文数据

// 	gtool::CSmartPtr<AllPackDataWaitHandleMap> m_pAllPackDataRecvMap;			//所有网络包数据解析完毕的
// 	gtool::CSmartPtr<AllPackDataWaitHandleMap> m_pAllPackDataSendMap;			//所有网络包数据已封包等待发送的

protected:
	bool						m_bCloseSocket;					//释放时候是否关闭套接字
// 	fd_set						m_fs_read;							//可读检测
// 	fd_set						m_fs_write;							//可写检测
// 	boost::atomic<I64>			m_nSendPackBatchID{ -1 };			//发送的批次id

	//构造
public:
	_PER_SOCKET_CONTEXT();
	~_PER_SOCKET_CONTEXT() {}
	int Release();

	//操作符
	bool operator==(const _PER_SOCKET_CONTEXT& Other);

	// io缓冲操作
public:
	_RW_CONTEXT* GetNewReadContext();	//获取新的待IOCp接收的数据缓冲
	_RW_CONTEXT* GetNewWriteContext();	//获取新的待iocp发送的数据缓冲
// 	PackDataMapInfo *GetNetDataRecvMap(const PACK_UNIQUE_ID& Id, const U16 nTransType, bool bCreate = false);	//获取解析完毕包数据
// 	PackDataMapInfo *GetNetDataSendMap(const PACK_UNIQUE_ID& Id, const U16 nTransType, bool bCreate = false);	//获取封包完毕包数据
// 	bool DelNetDataSendMap(const PACK_UNIQUE_ID& Id, const U16 nTransType);
// 	bool DelNetDataRecvMap(const PACK_UNIQUE_ID& Id, const U16 nTransType);
	void DelContext(_RW_CONTEXT* pContext);			// 从数组中移除一个指定的IoContext
	void DelReadContext(_RW_CONTEXT* pContext);
	void DelWriteContext(_RW_CONTEXT* pContext);
	_RW_CONTEXT* FrontToSend();
	_RW_CONTEXT* GetReadFront();
	SOCKET GetSocket() const { return m_Socket; }
	SOCKET& GetSocketObj() { return m_Socket; }
	void SetSocket(const SOCKET sNetSocket) {m_Socket = sNetSocket;}
	const CAddr& GetAddrInfo() const { return m_Addrinfo; }
	template<typename T>
	void SetAddrInfo(const T tObj)
	{
		m_Addrinfo.SetAddrInfoEx(tObj);
	}

// 	bool IsExistNetPackDataRecv(const PACK_UNIQUE_ID& Id);
// 	bool IsExistNetPackDataRecv(const PACK_UNIQUE_ID& Id, const U16 nTransType, const I32 nSeqNum);
// 	bool IsExistNetPackDataSend(const PACK_UNIQUE_ID& Id);
// 	bool IsExistNetPackDataSend(const PACK_UNIQUE_ID& Id, const U16 nTransType, const I32 nSeqNum);

	//辅助
protected:
//	TransTypePackDataMap * GetTransTypeSendMap(const PACK_UNIQUE_ID& Id);



	//辅助
public:
	void SetSocketCloseFinal(bool bClose);
//	bool IsDetectTimeOut(long tv_sec, long tv_usec, bool bReadableDetect = true, bool bWriteableDetect = false, int *pErrOut = NULL, bool bSetOneAtLeast = false, bool bInfiniteWaiting = false);
	const char *GetIp();
	const U16 GetPort();
//	I64 GetCurPackBatchID() { return m_nSendPackBatchID; }
//	void SetCurPackBatchID(const I64 nPackBatchID) { m_nSendPackBatchID = nPackBatchID; }

	MEMPOOL_CREATE(m_MemPoolAlloctor);

} PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;

struct NetDataBuffer
{
	HEAP_CREATE_NEED_CLEANS_PUBLIC(NetDataBuffer);
	MEMPOOL_CREATE(m_MemObj);

public:
	BUFFER8192 szBuffer;
	I64 nSizeData = 0;
};

//发送或者接收缓冲数据类型
struct NetCacheData
{
	NET_CACHE_TYPE ePackCacheType;		//包类型 正常包 回复包

};

//用于网络缓冲2 使用了内存池更高效
struct TimestampXByte
{
	I64		nTimestamp = 0;			//由发送线程填充 精确到毫秒
	I64		nSizeStream = 0;		//数据流大小
	gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> pdata_stream = NULL;
};

typedef gtool::CLitPtrMap<TimestampXByte, I32, DEL_WAY_DELETE> XByteStreamMap;		//网络数据 必须使用 NetDataStream key:nSeqNum
struct PackCacheMapInfo
{
	MEMPOOL_CREATE(m_MemObj);

public:
	PackCacheMapInfo()
	{
		m_pXByteStreamMap = XByteStreamMap::CreateNew();
		ASSERTEX(m_pXByteStreamMap);
	}
	~PackCacheMapInfo() {}

	I32 Release()
	{
		IF_OK(m_pXByteStreamMap)
			m_pXByteStreamMap->Cleans();

		return 0;
	}

	gtool::CSmartPtr<XByteStreamMap> m_pXByteStreamMap = NULL;
	I64 m_nTotalByte = 0;
	U64 m_Timestamp = 0;	//创建批
};

typedef gtool::TPtrList<NetDataBuffer, DEL_WAY_RELEASE, false> NetDataBufferList;


#pragma MEM_ALIGN_END(1)

///////////////////////////////////////////////////////////////////////////////////////

#endif

