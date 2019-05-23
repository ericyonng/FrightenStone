#ifndef NET_DATA_PACK_H_
#define NET_DATA_PACK_H_

#pragma once

class NETWORK_SERVICE_API IPackager
{
public:
	virtual int	Release() = 0;		//
	virtual bool IsMemPoolSwitchActive() = NULL;	//需要内存池创建
	virtual bool PackNext(void *pDataOut, U64& u64SizeOut) = NULL;						//封包
	virtual bool Unpack(void *&pDataOut, U64& u64SizeOut, TRAN_PROTOCOL::PackAccess * & pAccessOut, I64& rIdKey)=NULL;						//解包
	IPackager() {}
	virtual ~IPackager() {}

private:
	IPackager(const IPackager&) = delete;
	const IPackager& operator=(const IPackager&) = delete;
};


class NETWORK_SERVICE_API CNetDataPack :IPackager
{
	NO_COPY_NO_MOVE(CNetDataPack);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CNetDataPack);
	MEMPOOL_CREATE(m_MemPoolObj);

public:
	I64 GetSectionSize(const I64 u64Size);	//估计单包所需裁剪的数据大小
	bool Init(const void *pData, const U64 u64Size, bool bIsPack = true, const U64 u64Timestamp = 0, const U64 u64RandFeature = 0,  TRAN_PROTOCOL::TRAN_TYPE eTranType = TRAN_PROTOCOL::TRAN_TYPE_MSG, TRAN_PROTOCOL::PACK_TYPE ePackType = TRAN_PROTOCOL::PACK_TYPE_SEND, const I64 nPackBatchID=-1 );
	bool PackNext(void *pDataOut, U64& u64SizeOut);	//封包 u64SizeOut传入pDataOut大小，传出真实数据大小
	I64 GetCurPackModeSeqNum();
	bool GetCurPack(void *&pDataOut, U64& u64SizeOut);
	bool Unpack( void *&pDataOut, U64& u64SizeOut, TRAN_PROTOCOL::PackAccess * & pAccessOut, I64& rIdKey);	//各个输出参数声明周期跟随NetDataPack 需要及时拷贝数据 拆包 u64SizeOut传入pDataOut大小，传出真实数据大小
	void ResetBuffer();
	I64 GetCurDataSize() { return m_i64SizeOfData; }
	bool IsInit() const { return  m_bInit; }
	I64 GetCurPackModeTotalNum() { return m_PackAccess.nTotalNum; }
	TRAN_PROTOCOL::TRAN_TYPE GetCurTranType() { return TRAN_PROTOCOL::TRAN_TYPE(m_PackAccess.nTranType); }

protected:
	bool MakeMd5Str(char * Buffer, const U64 u64SizeOutIn, TRAN_PROTOCOL::PackHead& rHead, TRAN_PROTOCOL::PackAccess& rAccess, TRAN_PROTOCOL::PackData* pPackData, TRAN_PROTOCOL::PackTail& rTail);

protected:
	bool		m_bInit = false;			//初始化
	BUFFER2048	m_szBuffer = { 0 };			//缓冲
	U16			m_nBufferUsed = 0;			//缓冲已使用
	void *		m_pData = NULL;				//要封包/解包的数据
	I64			m_i64SizeOfData = 0;		//要封包/解包数据大小
	I64			m_i64HandledDataSize = 0;	//已经处理的数据长度
	bool		m_bPack = true;				//是否封包模式
	TRAN_PROTOCOL::PackAccess m_PackAccess;	//接入层数据
};

// class NETWORK_SERVICE_API NetDataPackMgr
// {
// 	NO_COPY_NO_MOVE(NetDataPackMgr);
// 	HEAP_CREATE_NEED_CLEANS_PUBLIC(NetDataPackMgr);
// 	MEMPOOL_CREATE(m_MemPoolObj);
// 
// public:
// 	bool AddPack(const void*pData, const U64 u64Size);
// 	bool Unpack(void *&pData, U64& u64Size);
// 
// 	typedef gtool::CLitPtrMap<CNetDataPack, U32, DEL_WAY_RELEASE> NetDataPackMap;	//key:SeqNo
// 	gtool::CSmartPtr<NetDataPackMap> m_pNetDataPackMap;	//数据包map
// };



#endif

