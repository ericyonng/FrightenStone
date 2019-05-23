#include <Net/SocketRecv.h>

MEMPOOL_CREATE_IMPLEMENTATION(CSocketRecv, m_MemObj)
CSocketRecv::CSocketRecv()
{
}

CSocketRecv::~CSocketRecv()
{
}

void CSocketRecv::Cleans()
{
	m_ps = NULL;
}

bool CSocketRecv::Recv()
{
	CHECKF(m_pBuffer);
	CHECKF(m_ps);
	CHECKF(m_nCacheSizeMax);
	CHECKF(IS_VALID_SOCKET_IDD(*m_ps));

	//清除缓冲
	ResetCache();

	char *pBuffer = (char *)(m_nCurDataSize + (char *)m_pBuffer);
	I32 nRet = 0;
	I32 nCountByte = 0;
	while (true)
	{
		IF_NOT(m_nCurDataSize < m_nCacheSizeMax)
			break;

		nRet = 0;
		nRet = recv(*m_ps, (char *)((char *)pBuffer + nCountByte), m_nCacheSizeMax - (m_nCurDataSize + nCountByte), 0);	//一次尽量把socket缓冲区读干净
		if (nRet <= 0 || nRet == SOCKET_ERROR)
		{
			if(nRet== SOCKET_ERROR)
				m_nLastErr = GetLastError();

			break;
		}
			
		//接收正常
		nCountByte += nRet;
	}

	ASSERTEX((nCountByte + m_nCurDataSize) <= m_nCacheSizeMax);
	m_nCurDataSize += nCountByte;

	return true;
}

bool CSocketRecv::Bind(const SOCKET& sbind, const I64 nCacheSizeSet /* = 0*/)
{
	CHECKF(IS_VALID_SOCKET_IDD(sbind));
	CHECKF(!m_ps);

	//初始化缓冲区
	I64 nSize = 0;
	SOCKET* sTmp = const_cast<SOCKET *>(&sbind);
	CHECKF(gtool::GetSocketCacheSize(*sTmp, SOCKET_CACHE_TYPE_RECV, nSize));	//获取缓冲大小
	CHECKF(nSize > 0);
	if (nCacheSizeSet > 0)
		nSize = nCacheSizeSet;

	m_pBuffer = new char[nSize];	//缓冲
	CHECKF(m_pBuffer);
	ZERO_CLEAN_PTR(m_pBuffer, nSize);
	m_nCacheSizeMax = nSize;

	m_ps = const_cast<SOCKET *>(&sbind);
	CHECKF(m_ps);

	return true;
}

void CSocketRecv::ResetCache()
{
	CHECK(m_pBuffer);
	CHECK(m_nCacheSizeMax);
	ZERO_CLEAN_PTR(m_pBuffer, m_nCacheSizeMax);
	m_nCurDataSize = 0;
	m_nLastErr = 0;
}
