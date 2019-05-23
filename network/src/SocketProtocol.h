#ifndef SOCKET_PROTOCOL_H_
#define SOCKET_PROTOCOL_H_

#pragma once
#include <BaseCode/NetworkServiceApi.h>
#include <openssl/md5.h>
#include <PoolObj/xByte.h>
#include <PoolObj/xUByte.h>

//协议
//格式
//

#pragma MEM_ALIGN_BEGIN(1)

NAMESPACE_BEGIN(TRAN_PROTOCOL)

typedef I32 PROTOCOL_VER;

#define NETPACK_ORDERID_LEN	64		//订单号字符串长度

#define PACKAGE_SIZE_MAX I64(1024)		//单个包限制大小1024

#undef TIMEOUT_RESEND_COUNT
#define  TIMEOUT_RESEND_COUNT 10			//超时重发次数

//协议类型
enum TRAN_TYPE
{
	TRAN_TYPE_NONE=0,		//未知的
	TRAN_TYPE_MSG,			//消息类型 支持长连接
	TRAN_TYPE_FILE,			//传输文件
	TRAN_TYPE_HTTP,			//http类型
	TRAN_TYPE_HTTPS,		//https
	TRAN_TYPE_FTP,			//FTP
	TRAN_TYPE_SEC_MSG,		//安全的消息协议
	TRAN_TYPE_END,
};

//特征值
enum FEATURE_NUM
{
	FEATURE_NUM_PACK_HEADER=0x0404,	//包头特征值
	FEATURE_NUM_PACK_TAIL=0x0505,	//包尾特征值
};

enum PACK_TYPE
{
	PACK_TYPE_NULL = -1,	//无效
	PACK_TYPE_SEND=0,		//发送包
	PACK_TYPE_REPLY=1,		//发送后的应答包 用于请求下一个包
	PACK_TYPE_END,
};

//校验：PackHead+PackAccess+PackData+PackTail+key

//包头
struct PackHead
{
	U16 nMark;		//特征值
};

//一组完整包唯一标识：nTranType-nPackType-nTimestamp-nPackBatchID-nTotalNum-nRandFeature
//校验参数：nMark+nTranType+nTotalNum+nPackBatchID+nSeqNum+nPackSize+nPackType+nTimestamp+nRandFeature+Len+Stream+nMark
//接入层
struct PackAccess
{
	U16			nTranType;	//协议类型 TRAN_TYPE *
	U32			nTotalNum;	//包总数
	I64			nPackBatchID;	//包的批次 生命周期为socket生命期 每一批递增 由包发起方维护计数 socket生命期唯一 只递增
	U32			nSeqNum;	//当前序号 序号必须从1开始
	U16			nPackSize;	//当前整个包大小
	U8			nPackType;	//包类型 PACK_TYPE *
	U64			nTimestamp;	//时间戳以毫秒为单位 *
	U64			nRandFeature;	//特征值 boost随机数发生器产生 *
	U8			Md5Str[MD5_VERIFY_SIZE];	//md5字符串
};

//真实的数据
struct PackData
{
	U16 nLen;		//数据流长度 *
	U8	Stream[0];	//可变长数据流数据流 *
};

//包尾
struct PackTail
{
	U16 nMark;		//特征值
};

//数据流
struct NETWORK_SERVICE_API NetDataStream
{
	U16 nLen;
	char rData[0];			//任意的数据流 在服务器接收的时候可能为 PackData.Stream
};

struct  BusinessDataCache
{
	U16 nLen;
	char rData[0];			//任意的数据流 在服务器接收的时候可能为 PackData.Stream
};

//typedef NetDataStream BusinessDataCache;

// struct NetCache
// {
// 	union
// 	{
// 		void *m_ptr;
// 		BusinessDataCache *m_pBusinessDataCache;
// 		CXByte *m_pXByte;
// 		CUXByte m_pUXByte;
// 	};
// 	NetCache(void *ptr) :m_ptr(ptr){}
// };

DECLEAR_UNION_PTR_BEGIN(NetCache)
ADD_UNION_PTR_VAR(BusinessDataCache, m_pBusinessDataCache)
ADD_UNION_PTR_VAR(CXByte, m_pXByte)
ADD_UNION_PTR_VAR(CUXByte, m_pUXByte)
ADD_UNION_PTR_VAR(char, m_pstr)
DECLEAR_UNION_PTR_END(NetCache)

// struct NetCache
// {
// 	union
// 	{
// 		void*				m_ptr;
// 		NetDataStream *		m_pNetDataStream;
// 		CXByte *			m_pXByte;
// 	};
// 	NetCache(void* pBuf = NULL) :m_ptr((void*)pBuf) {}
// };

//流量统计
typedef  struct DataFlowCount
{
	long	lRecvTotal;				//总的接收量	byte
	long	lSendTotal;				//总的发送量	byte
}DATA_FLOW_COUNT_T;

NAMESPACE_END(TRAN_PROTOCOL)
#define S_DATA_FLOW_COUNT_T		sizeof(TRAN_PROTOCOL::DATA_FLOW_COUNT_T)
#pragma MEM_ALIGN_END(1)


#endif