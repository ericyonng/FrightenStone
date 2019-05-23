#include <Net/NetStruct.h>

MEMPOOL_CREATE_IMPLEMENTATION(_RW_CONTEXT, m_MemPoolAlloctor)
_RW_CONTEXT::_RW_CONTEXT()
{
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
	memset(m_szBuffer, 0, MAX_BUFFER_LEN);
	m_wsaBuf.buf = m_szBuffer;
	m_wsaBuf.len = MAX_BUFFER_LEN;
	m_OpType = NULL_POSTED;
	m_pParrentObj = NULL;
	m_u64DataToHandle = 0;
}

_RW_CONTEXT::~_RW_CONTEXT()
{
	m_pParrentObj = NULL;
}

bool _RW_CONTEXT::operator==(const _RW_CONTEXT& Other)
{
	return this == &Other;
}

void _RW_CONTEXT::ResetBuffer()
{
	m_u16FailHandleCount = 0;
	m_u64DataToHandle = 0;
	m_idPackBatchID = -1;
	m_nSeqNum = -1;
	m_nTranType = TRAN_PROTOCOL::TRAN_TYPE_MSG;
	memset(m_szBuffer, 0, MAX_BUFFER_LEN);
}

SOCKET _RW_CONTEXT::GetSocket()
{
	PER_SOCKET_CONTEXT *pSocketContext = (PER_SOCKET_CONTEXT *)m_pParrentObj;
	CHECKF(pSocketContext);

	return pSocketContext->GetSocket();
}

const SOCKADDR_IN* _RW_CONTEXT::GetAddr()
{
	PER_SOCKET_CONTEXT *pSocketContext = (PER_SOCKET_CONTEXT *)m_pParrentObj;
	IF_NOT(pSocketContext)
		return NULL;

	return pSocketContext->GetAddrInfo();
}

const CAddr* _RW_CONTEXT::GetAddrObj()
{
	PER_SOCKET_CONTEXT *pSocketContext = (PER_SOCKET_CONTEXT *)m_pParrentObj;
	IF_NOT(pSocketContext)
		return NULL;

	return &pSocketContext->GetAddrInfo();
}

const char * _RW_CONTEXT::GetIp()
{
	PER_SOCKET_CONTEXT *pSocketContext = (PER_SOCKET_CONTEXT *)m_pParrentObj;
	IF_NOT(pSocketContext)
		return "";

	return pSocketContext->GetIp();
}

const U16 _RW_CONTEXT::GetPort()
{
	PER_SOCKET_CONTEXT *pSocketContext = (PER_SOCKET_CONTEXT *)m_pParrentObj;
	IF_NOT(pSocketContext)
		return 0;

	return pSocketContext->GetPort();
}

/////////////////////////////////////////////////////////////////////////////////////////
MEMPOOL_CREATE_IMPLEMENTATION(_PER_SOCKET_CONTEXT, m_MemPoolAlloctor)

_PER_SOCKET_CONTEXT::_PER_SOCKET_CONTEXT()
{
	m_pListReadContext = NetRWContextList::CreateNew();
	m_pListWriteContext = NetRWContextList::CreateNew();
	ASSERTNET(m_pListReadContext);
	ASSERTNET(m_pListWriteContext);
	m_bCloseSocket = false;

	m_Socket = INVALID_SOCKET;
}

bool _PER_SOCKET_CONTEXT::operator==(const _PER_SOCKET_CONTEXT& Other)
{
	return this == &Other;
}

int _PER_SOCKET_CONTEXT::Release()
{
	if (m_Socket != INVALID_SOCKET)
	{
		if (m_bCloseSocket)
		{
			ASSERTNET(closesocket(m_Socket) == 0);
		}
		m_Socket = INVALID_SOCKET;
	}

	IF_OK(m_pListReadContext)
		m_pListReadContext->Clear();

	IF_OK(m_pListWriteContext)
		m_pListWriteContext->Clear();

	delete this;

	return 0;
}

// 获取一个新的IoContext
_RW_CONTEXT* _PER_SOCKET_CONTEXT::GetNewReadContext()
{
	IF_NOT(m_pListReadContext)
		return NULL;

	gtool::CSmartPtrC<_RW_CONTEXT> p = new _RW_CONTEXT;
	IF_NOT(p)
		return NULL;

	p->m_pParrentObj = this;

	IF_NOT(!m_pListReadContext->IsExist(p))	//保证 io上下文对象唯一性
		return NULL;

	_RW_CONTEXT *pTmp = NULL;
	IF_NOT(m_pListReadContext->PushBack(pTmp = p.pop()))
	{
		SAFE_DELETE(pTmp);
		return NULL;
	}

	return pTmp;
}

_RW_CONTEXT* _PER_SOCKET_CONTEXT::GetNewWriteContext()
{
	IF_NOT(m_pListWriteContext)
		return NULL;

	gtool::CSmartPtrC<_RW_CONTEXT> p = new _RW_CONTEXT;
	IF_NOT(p)
		return NULL;

	p->m_pParrentObj = this;

	IF_NOT(!m_pListWriteContext->IsExist(p))	//保证 io上下文对象唯一性
		return NULL;

	_RW_CONTEXT *pTmp = NULL;
	IF_NOT(m_pListWriteContext->PushBack(pTmp = p.pop()))
	{
		SAFE_DELETE(pTmp);
		return NULL;
	}

	return pTmp;
}



void _PER_SOCKET_CONTEXT::DelContext(_RW_CONTEXT* pContext)
{
	CHECK(pContext);
	bool bDel = false;
	IF_OK(m_pListWriteContext)
	{
		if (m_pListWriteContext->IsExist(pContext))
		{
			ASSERTNET(m_pListWriteContext->DelFirstObj(*pContext));
			bDel = true;
		}
	}

	IF_OK(m_pListReadContext)
	{
		if (m_pListReadContext->IsExist(pContext))
		{
			ASSERTNET(m_pListReadContext->DelFirstObj(*pContext));
			bDel = true;
		}
	}

	ASSERTNET(bDel);
	pContext = NULL;
}

void _PER_SOCKET_CONTEXT::DelReadContext(_RW_CONTEXT* pContext)
{
	CHECK(pContext);
	bool bDel = false;
	IF_OK(m_pListReadContext)
	{
		if (m_pListReadContext->IsExist(pContext))
		{
			ASSERTNET(m_pListReadContext->DelFirstObj(*pContext));	//由于唯一性固删除一个即可
			bDel = true;
		}
	}

	ASSERTNET(bDel);
	pContext = NULL;
}

void _PER_SOCKET_CONTEXT::DelWriteContext(_RW_CONTEXT* pContext)
{
	CHECK(pContext);
	bool bDel = false;
	IF_OK(m_pListWriteContext)
	{
		if (m_pListWriteContext->IsExist(pContext))
		{
			ASSERTNET(m_pListWriteContext->DelFirstObj(*pContext));
			bDel = true;
		}
	}

	ASSERTNET(bDel);
	pContext = NULL;
}

_RW_CONTEXT * _PER_SOCKET_CONTEXT::FrontToSend()
{
	CHECKF(m_pListWriteContext);
	if (m_pListWriteContext->IsEmpty())
		return NULL;

	return m_pListWriteContext->Front();
}

_RW_CONTEXT* _PER_SOCKET_CONTEXT::GetReadFront()
{
	CHECKF(m_pListReadContext);
	if (m_pListReadContext->IsEmpty())
		return NULL;

	return m_pListReadContext->Front();
}


void _PER_SOCKET_CONTEXT::SetSocketCloseFinal(bool bClose)
{
	m_bCloseSocket = bClose;
}



const char * _PER_SOCKET_CONTEXT::GetIp()
{
	CHECKF((m_Socket) && (m_Socket != INVALID_SOCKET));
	return m_Addrinfo;
}

const U16 _PER_SOCKET_CONTEXT::GetPort()
{
	CHECKF((m_Socket) && (m_Socket != INVALID_SOCKET));
	return m_Addrinfo;
}

///////////////////////////////////////////////////////////////////////////////////////////

MEMPOOL_CREATE_IMPLEMENTATION(CAddr, m_MemPoolAlloctor)

//地址类
CAddr::CAddr()
{
	Reset();
}

CAddr::CAddr(char *szip, unsigned short usPort, unsigned short usFamily /*= AF_INET*/)
{
	Reset();
	CHECK(szip&&strlen(szip) > 0);

	CHECK(gtool::FillTcpAddrInfo(m_stAddr_Info, szip, usPort, usFamily));
}

CAddr::CAddr(const SOCKADDR_IN& rOther)
{
	Reset();
	m_stAddr_Info = rOther;
}

CAddr::~CAddr()
{
}

void CAddr::Cleans()
{

}

void CAddr::Reset()
{
	ZERO_CLEAN_OBJ(m_stAddr_Info);
	ZERO_CLEAN_OBJ(m_DomainName);
	ZERO_CLEAN_OBJ(m_szCache);
	ZERO_CLEAN_OBJ(m_uCache);
}

void CAddr::SetAddrInfoEx(const struct sockaddr_in &stAddr_Info)
{
	m_stAddr_Info = stAddr_Info;
}

void CAddr::SetAddrInfo(
	const unsigned short family
	, const char *szip /*= NULL*/
	, const char *domainname /*= NULL*/
	, bool bUserIp /*= true*/
	, const unsigned short* port /*= NULL*/
	, bool bUseBind /*=true*/
	, bool bUserSockStream /*=true*/ )
{
	m_stAddr_Info.sin_family = family;

	if (port)
		m_stAddr_Info.sin_port = htons(*port);

	if (bUserIp)
	{
		//ip
		if (szip)
			CHECK(inet_pton(m_stAddr_Info.sin_family, szip, &m_stAddr_Info.sin_addr.s_addr) > 0);
	}
	else
	{
		//域名
		if (domainname)
		{
			//端口号
			BUFFER128 szPort = { 0 };
			BUFFER256 szIpStr = { 0 };
			char *pIpStr = szIpStr;

			if (port)
				sprintf_s(szPort, sizeof(szPort) - 1, "%d", *port);

			const char *pService = (const char *)((port == NULL) ? NULL : szPort);

			//转换为ip地址串
			CHECK(gtool::GetIPByDomain(domainname, pService
				, pIpStr, sizeof(szIpStr), 0
				, bUseBind ? NET_SYS::AI_FLAGS_TYPE_AI_PASSIVE : NET_SYS::AI_FLAGS_TYPE_NONE
				, bUserSockStream ? NET_SYS::SOCK_TYPE_SOCK_STREAM : NET_SYS::SOCK_TYPE_SOCK_DGRAM
			));

			CHECK(inet_pton(m_stAddr_Info.sin_family, szIpStr, &m_stAddr_Info.sin_addr.s_addr) > 0);

			strcpy(m_DomainName, domainname);
		}
	}
}

const char * CAddr::GetDomainName() const
{
	return m_DomainName;
}

U16 CAddr::GetFamily() const
{
	return m_stAddr_Info.sin_family;
}

bool CAddr::GetAddrInfo(bool bReload /*= false*/) const
{
	//若缓冲没有则重新载入，若有则bReload决定是否重新载入
	auto nLen = strlen(m_szCache);
	if (nLen <= 0 || m_uCache == 0)
		bReload = true;

	if (bReload)
	{
		ZERO_CLEAN_OBJ(m_szCache);
		char *pCache = m_szCache;
		U16* puCache = const_cast<U16 *>(&m_uCache);
		CHECKF(puCache);
		CHECKF(gtool::GetAddrInfoFromNetInfo(m_stAddr_Info, pCache, sizeof(m_szCache),*puCache));
	}

	return true;
}

CAddr & CAddr::operator=(const CAddr &addr)
{
	COPY_OBJ(*this, addr);
	return *this;
}

CAddr & CAddr::operator =(CAddr &addr)
{
	COPY_OBJ(*this, addr);
	return *this;
}

CAddr::operator const struct sockaddr *() const
{
	return (struct sockaddr *)(&m_stAddr_Info);
}

CAddr::operator const struct sockaddr_in *() const
{
	return (struct sockaddr_in *)(&m_stAddr_Info);
}

CAddr::operator struct sockaddr *()
{
	return (struct sockaddr *)(&m_stAddr_Info);
}

CAddr::operator struct sockaddr_in *()
{
	return (struct sockaddr_in *)(&m_stAddr_Info);
}

//获取ip
CAddr::operator const char *() const
{
	IF_NOT(GetAddrInfo())
		return NULL;

	return m_szCache;
}

//获取端口号
CAddr::operator const U16() const
{
	CHECKF(GetAddrInfo());

	return m_uCache;
}


MEMPOOL_CREATE_IMPLEMENTATION(NetDataBuffer, m_MemObj)

NetDataBuffer::NetDataBuffer()
{
	ZERO_CLEAN_OBJ(szBuffer);
}

NetDataBuffer::~NetDataBuffer()
{
}

void NetDataBuffer::Cleans()
{
}

//网络流map
MEMPOOL_CREATE_IMPLEMENTATION(PackCacheMapInfo, m_MemObj)