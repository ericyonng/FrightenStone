#include"NetDataPack.h"
#include <Net/SocketProtocol.h>
#include <openssl/md5.h>

MEMPOOL_CREATE_IMPLEMENTATION(CNetDataPack, m_MemPoolObj)

CNetDataPack::CNetDataPack()
{
}

CNetDataPack::~CNetDataPack()
{
}

void CNetDataPack::Cleans()
{
}

I64 CNetDataPack::GetSectionSize(const I64 u64Size)
{
	CHECKF(u64Size >= 0);

	//单包可容纳的数据最大值
	I64 i64DataMax = PACKAGE_SIZE_MAX - I64(sizeof(TRAN_PROTOCOL::PackHead) + sizeof(TRAN_PROTOCOL::PackAccess) + sizeof(TRAN_PROTOCOL::PackData) + sizeof(TRAN_PROTOCOL::PackTail));
	i64DataMax = gtool::Max(i64DataMax, I64(0));

	return gtool::Min(I64(i64DataMax), u64Size);
}

bool CNetDataPack::Init(const void *pData, const U64 u64Size, bool bIsPack /*= true*/, const U64 u64Timestamp /*= 0*/, const U64 u64RandFeature /*= 0*/, TRAN_PROTOCOL::TRAN_TYPE eTranType /*= TRAN_PROTOCOL::TRAN_TYPE_MSG*/, TRAN_PROTOCOL::PACK_TYPE ePackType /*= TRAN_PROTOCOL::PACK_TYPE_SEND*/, const I64 nPackBatchID/*=-1 */)
{
	//校验数据
	if(!bIsPack)
		CHECKF(pData&&u64Size);

	//舒适化参数
	m_pData = const_cast<void *>(pData);
	m_i64SizeOfData = u64Size;
	m_bPack = bIsPack;

	//封包所需参数
	if (m_bPack)
	{
		CHECKF((TRAN_PROTOCOL::TRAN_TYPE_NONE < eTranType) && (eTranType < TRAN_PROTOCOL::TRAN_TYPE_END));
		CHECKF((ePackType > TRAN_PROTOCOL::PACK_TYPE_NULL) && (ePackType < TRAN_PROTOCOL::PACK_TYPE_END));
		const auto nSizeOne = GetSectionSize(m_i64SizeOfData);

		//接入层数据
		const auto nCountPack = (nSizeOne == 0) ? 1 : (m_i64SizeOfData / nSizeOne + (((m_i64SizeOfData% nSizeOne) != 0) ? 1 : 0));
		m_PackAccess.nPackType = ePackType;
		m_PackAccess.nRandFeature = u64RandFeature;
		m_PackAccess.nSeqNum = 0;
		m_PackAccess.nTimestamp = u64Timestamp;
		m_PackAccess.nTotalNum = nCountPack;
		m_PackAccess.nPackBatchID = nPackBatchID;
		m_PackAccess.nTranType = eTranType;
	}
	

	ZERO_CLEAN_PTR(m_szBuffer, sizeof(m_szBuffer));
	m_bInit = true;
	return true;
}

bool CNetDataPack::PackNext(void *pDataOut, U64& u64SizeOut)
{
	CHECKF(m_bPack == true);
	CHECKF(m_nBufferUsed <= PACKAGE_SIZE_MAX);
	CHECKF(PACKAGE_SIZE_MAX <= sizeof(m_szBuffer));
	CHECKF(pDataOut && u64SizeOut && (m_i64SizeOfData <= GetSectionSize(m_i64SizeOfData)) && (u64SizeOut >= m_i64SizeOfData));
	ZERO_CLEAN_PTR(pDataOut, u64SizeOut);

	//定义变量
	auto pCache = m_szBuffer;
	I32 i32SizeUsed = 0;	

	//包头
	ASSERTEX(sizeof(TRAN_PROTOCOL::PackHead) < PACKAGE_SIZE_MAX);
	TRAN_PROTOCOL::PackHead* pHead = (TRAN_PROTOCOL::PackHead*)((char *)(pCache + i32SizeUsed));
	pHead->nMark = TRAN_PROTOCOL::FEATURE_NUM_PACK_HEADER;	
	i32SizeUsed += sizeof(TRAN_PROTOCOL::PackHead);
	ASSERTEX(i32SizeUsed <= PACKAGE_SIZE_MAX);

	//接入层
	ASSERTEX(sizeof(m_PackAccess) < PACKAGE_SIZE_MAX);
	TRAN_PROTOCOL::PackAccess *pAccess = (TRAN_PROTOCOL::PackAccess *)(pCache + i32SizeUsed);
	memmove(pAccess, &m_PackAccess, sizeof(m_PackAccess));
	i32SizeUsed += sizeof(m_PackAccess);
	ASSERTEX(i32SizeUsed <= PACKAGE_SIZE_MAX);

	//数据层
	TRAN_PROTOCOL::PackData* pDataPack = (TRAN_PROTOCOL::PackData*)((char *)(pCache + i32SizeUsed));
	pDataPack->nLen = GetSectionSize(m_i64SizeOfData - m_i64HandledDataSize);

	//判断是否应打包
	if ((m_PackAccess.nPackType != TRAN_PROTOCOL::PACK_TYPE_REPLY) && pDataPack->nLen <= 0)
		return false;

	//包总数到了
	if (pAccess->nSeqNum >= pAccess->nTotalNum)
		return false;

	if(m_pData)
		memmove(pDataPack->Stream, (char *)(m_pData) +m_i64HandledDataSize, pDataPack->nLen);
	m_i64HandledDataSize += pDataPack->nLen;
	ASSERTEX(m_i64HandledDataSize <= m_i64SizeOfData);
	i32SizeUsed += (sizeof(TRAN_PROTOCOL::PackData) + pDataPack->nLen);
	ASSERTEX(i32SizeUsed<= PACKAGE_SIZE_MAX);

	//包尾
	TRAN_PROTOCOL::PackTail* pPackTail = (TRAN_PROTOCOL::PackTail*)((char *)(pCache + i32SizeUsed));
	pPackTail->nMark = TRAN_PROTOCOL::FEATURE_NUM_PACK_TAIL;
	i32SizeUsed += (sizeof(TRAN_PROTOCOL::PackTail));
	ASSERTEX(i32SizeUsed <= PACKAGE_SIZE_MAX);

	//接入层
	pAccess->nPackSize = i32SizeUsed;
	pAccess->nSeqNum += 1;
	ASSERTEX(pAccess->nSeqNum <= pAccess->nTotalNum);
	m_PackAccess.nSeqNum = pAccess->nSeqNum;

	U64 u64Md5StrSize = MD5_VERIFY_SIZE;
	CHECKF(MakeMd5Str((char *)(pAccess->Md5Str), u64Md5StrSize, *pHead, *pAccess, pDataPack, *pPackTail));

	//数据拷贝
	m_nBufferUsed = i32SizeUsed;
	u64SizeOut = i32SizeUsed;
	memmove(pDataOut, m_szBuffer, i32SizeUsed);

	return true;
}

I64 CNetDataPack::GetCurPackModeSeqNum()
{
	return m_PackAccess.nSeqNum;
}

bool CNetDataPack::GetCurPack(void *&pDataOut, U64& u64SizeOut)
{
	CHECKF(m_bInit);
	memmove(pDataOut, m_szBuffer, m_nBufferUsed);
	u64SizeOut = m_nBufferUsed;

	return true;
}

bool CNetDataPack::Unpack( void *&pDataOut, U64& u64SizeOut, TRAN_PROTOCOL::PackAccess * & pAccessOut, I64& rIdKey)
{
	CHECKF(m_bInit);
	CHECKF(m_bPack == false);
	CHECKF(m_pData&&m_i64SizeOfData);
	pAccessOut = NULL;
	pDataOut = NULL;
	u64SizeOut = 0;
	rIdKey = 0;

	//定义变量
	char * pCache = (char *)m_pData;
	I32 i32SizeUsed = 0;

	//定位包头
	TRAN_PROTOCOL::PackHead* pHead = NULL;
	TRAN_PROTOCOL::PackAccess *pAccess = NULL;
	TRAN_PROTOCOL::PackTail *pTail = NULL;
	TRAN_PROTOCOL::PackData *pPackData = NULL;
	const I32 i32PackSizeNoData = sizeof(TRAN_PROTOCOL::PackHead) + sizeof(TRAN_PROTOCOL::PackAccess) + sizeof(TRAN_PROTOCOL::PackData) + sizeof(TRAN_PROTOCOL::PackTail);
	bool bVerifySuc = false;
	I32 nPackMaxSize = 0;
	char szBufferVerify[MD5_VERIFY_SIZE] = { 0 };
	for (I32 i = 0; i < m_i64SizeOfData; ++i)
	{
		nPackMaxSize = m_i64SizeOfData - i;
		pHead = (TRAN_PROTOCOL::PackHead*)((char *)(pCache + i));

		//判断不完整包
		IF_NOT((m_i64SizeOfData - i) >= i32PackSizeNoData)
			return false;

		//定位包头
		if(pHead->nMark != TRAN_PROTOCOL::FEATURE_NUM_PACK_HEADER)
			continue;

		//尝试解析
		pAccess = (TRAN_PROTOCOL::PackAccess *)((char *)(pCache + i + sizeof(TRAN_PROTOCOL::PackHead)));
		IF_NOT( PACKAGE_SIZE_MAX>= pAccess->nPackSize)
			continue;

		//不能超过整个包的大小
		IF_NOT(nPackMaxSize>= pAccess->nPackSize)
			continue;

		pTail = (TRAN_PROTOCOL::PackTail *)((char *)(pCache + i + pAccess->nPackSize - sizeof(TRAN_PROTOCOL::PackTail)));

		//不是包尾
		IF_NOT(pTail->nMark== TRAN_PROTOCOL::FEATURE_NUM_PACK_TAIL)
			continue;

		//校验数据体
		pPackData = (TRAN_PROTOCOL::PackData *)((char *)(pCache + i + sizeof(TRAN_PROTOCOL::PackHead) + sizeof(TRAN_PROTOCOL::PackAccess)));
		
		//包大小校验
		IF_NOT(pAccess->nPackSize == (i32PackSizeNoData + pPackData->nLen))
			continue;

		IF_NOT((m_i64SizeOfData - i) >= (i32PackSizeNoData + pPackData->nLen))
			continue;

		//校验数据
		//校验字符串的合法性
		IF_NOT((pAccess->Md5Str[MD5_VERIFY_SIZE-1]==0) 
			&& (strlen( (char *)(pAccess->Md5Str))!=0))
			continue;

		szBufferVerify[0] = 0;
		IF_NOT(MakeMd5Str(szBufferVerify, MD5_VERIFY_SIZE, *pHead, *pAccess, pPackData, *pTail))
			continue;

		if (strcmp(szBufferVerify, (char *)(pAccess->Md5Str)) == 0)
		{
			bVerifySuc = true;
			break;
		}
	}

	//校验
	CHECKF(pHead&&pAccess&&pTail&&pPackData);

	//数据出错校验是否成功
	if (bVerifySuc)
	{
		//重新定位数据
		char *pNewDataPos = (char *)((char *)(pHead)+pAccess->nPackSize);
		I32 i32Diff = I32(pNewDataPos - m_pData);
		CHECKF(i32Diff >= 0);
		CHECKF(m_i64SizeOfData >= i32Diff);
		m_i64SizeOfData -= i32Diff;
		m_pData = pNewDataPos;
		rIdKey = pAccess->nPackBatchID;

		//输出
		pDataOut = NULL;
		if(pPackData->nLen>0)
			pDataOut = pPackData->Stream;
		
		u64SizeOut = pPackData->nLen;
		pAccessOut = pAccess;
		return true;
	}
	
	return false;
}


void CNetDataPack::ResetBuffer()
{
	m_bInit = false;
	ZERO_CLEAN_PTR(m_szBuffer, sizeof(m_szBuffer));
	m_nBufferUsed = 0;
	m_pData = NULL;
	m_i64SizeOfData = 0;
	m_bPack = true;
	m_i64HandledDataSize = 0;
	ZERO_CLEAN_OBJ(m_PackAccess);
}

bool CNetDataPack::MakeMd5Str(char * Buffer, const U64 u64SizeOutIn, TRAN_PROTOCOL::PackHead& rHead, TRAN_PROTOCOL::PackAccess& rAccess, TRAN_PROTOCOL::PackData* pPackData, TRAN_PROTOCOL::PackTail& rTail)
{
	CHECKF(Buffer && (MD5_VERIFY_SIZE <= u64SizeOutIn) && pPackData);

	BUFFER4096 szMd5Str = { 0 };

	char *pBufferDataStr = NULL;
	I32 nMd5Len = 0;
	if (pPackData->nLen)
	{
		const I32 i32BufferDataLen = 2 * pPackData->nLen + 1;
		gtool::CSmartPtrC<char, char, DEL_WAY_DELETE_MULTI> pBufferDataTmp = new char[i32BufferDataLen];
		CHECKF(pBufferDataTmp);
		ZERO_CLEAN_PTR((char *)(pBufferDataTmp), i32BufferDataLen);
		char *pBufferDataStr = pBufferDataTmp;
		CHECKF(gtool::Conver2HexStr((char *)pPackData->Stream, pPackData->nLen, pBufferDataStr, i32BufferDataLen));

		nMd5Len = sprintf_s(szMd5Str, sizeof(szMd5Str) - 1, "%u%hu%u%I64d%u%hu%u%I64u%I64u%hu%s%hu"
			, rHead.nMark, rAccess.nTranType, rAccess.nTotalNum, rAccess.nPackBatchID, rAccess.nSeqNum, rAccess.nPackSize
			, rAccess.nPackType, rAccess.nTimestamp, rAccess.nRandFeature, pPackData->nLen, pBufferDataStr
			, rTail.nMark);
	}
	else
	{
		nMd5Len = sprintf_s(szMd5Str, sizeof(szMd5Str) - 1, "%u%hu%u%I64d%u%hu%u%I64u%I64u%hu%hu"
			, rHead.nMark, rAccess.nTranType, rAccess.nTotalNum, rAccess.nPackBatchID, rAccess.nSeqNum, rAccess.nPackSize
			, rAccess.nPackType, rAccess.nTimestamp, rAccess.nRandFeature, pPackData->nLen
			, rTail.nMark);
	}

// 	nMd5Len = gtool::Max(nMd5Len, I32(0));
// 	szMd5Str[gtool::Min(nMd5Len, I32(sizeof(szMd5Str) - 1))] = 0;
	ASSERTEX(nMd5Len < sizeof(szMd5Str));
	STRARRAY_ADD_ZERO_TAIL(szMd5Str, nMd5Len);
	nMd5Len = strlen(szMd5Str);

	CHECKF(gtool::md5encode(szMd5Str, strlen(szMd5Str), Buffer, u64SizeOutIn));
	Buffer[MD5_VERIFY_SIZE - 1] = 0;

	IF_NOT(strlen((char *)Buffer) == (MD5_VERIFY_SIZE - 1))
	{
		LOGNET("szMd5Str:", szMd5Str, " Md5Str:", Buffer);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////
// 
// NetDataPackMgr::NetDataPackMgr()
// {
// 	m_pNetDataPackMap = NetDataPackMap::CreateNew();
// }
// 
// NetDataPackMgr::~NetDataPackMgr()
// {
// }
// 
// void NetDataPackMgr::Cleans()
// {
// 	IF_OK(m_pNetDataPackMap)
// 		m_pNetDataPackMap->Cleans();
// }
// 
// bool NetDataPackMgr::AddPack(const void*pData, const U64 u64Size)
// {
// 	return true;
// }
// 
// bool NetDataPackMgr::Unpack(void *&pData, U64& u64Size)
// {
// 	return true;
// }

