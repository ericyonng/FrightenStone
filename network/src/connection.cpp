#include "connection.h"
#include "overlapped.h"



class Connection::Impl
{
	MEMPOOL_CREATE(m_MemObj);
public:
	Impl(const SOCKET& socket, Connection* owner);
	~Impl();

public:
	std::atomic<SOCKET> _socket{ INVALID_SOCKET };

	BUFFER10240 _read_buffer;	//读缓冲
	BUFFER128K _msg_buff;		//做消息缓冲区 100k
	std::atomic<int> _pos{ 0 };					//当前指向的缓冲区的位置

	std::atomic<std::size_t> _write_buffer_size{ 0 };								//发送缓冲区大小

	gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> _write_buffer;	//发送缓冲区

	std::atomic<std::size_t> _sent_bytes{ 0 };	//已发送字节数

	std::atomic<std::size_t> _total_bytes{ 0 };	//总共的字节数

	gtool::CSmartPtrC<Overlapped> _connect_overlapped;
	gtool::CSmartPtrC<Overlapped> _accept_overlapped;
	gtool::CSmartPtrC<Overlapped> _read_overlapped;
	gtool::CSmartPtrC<Overlapped> _write_overlapped;

};

MEMPOOL_CREATE_IMPLEMENTATION(Connection::Impl, m_MemObj)


Connection::Impl::Impl(const SOCKET& socket, Connection* owner)
	: _socket(socket)
	, _connect_overlapped(CreateOverlapped(Overlapped::Connect_type))
	, _accept_overlapped(CreateOverlapped(Overlapped::Accept_type))
	, _read_overlapped(CreateOverlapped(Overlapped::Read_type))
	, _write_overlapped(CreateOverlapped(Overlapped::Write_type))
	, _sent_bytes(0)
	, _total_bytes(0)
	, _write_buffer(NULL)
	, _write_buffer_size()
	, _pos(0)
{
	_connect_overlapped->connection = owner;
	_accept_overlapped->connection = owner;
	_read_overlapped->connection = owner;
	_write_overlapped->connection = owner;
}

Connection::Impl::~Impl()
{
	RELEASE_SOCKET(_socket);
	_connect_overlapped->connection = NULL;
	_accept_overlapped->connection = NULL;
	_read_overlapped->connection = NULL;
	_write_overlapped->connection = NULL;
	//ASSERT(IS_VALID_SOCKET_IDD(_socket));
}

MEMPOOL_CREATE_IMPLEMENTATION(Connection, m_MemObj)
Connection::Connection(const SOCKET& socket)
	: _impl(new Impl(socket, this))
{
	ASSERTNET(IS_VALID_SOCKET_IDD(socket));
	ASSERTNET(_impl);
}

Connection::~Connection()
{
}

Overlapped* Connection::GetWriteOverlapped() const
{
	CHECKF(_impl);
	return _impl->_write_overlapped;
}

Overlapped* Connection::GetReadOverlapped() const
{
	return _impl->_read_overlapped;
}

Overlapped* Connection::GetAcceptOverlapped() const
{
	CHECKF(_impl);
	return _impl->_accept_overlapped;
}

Overlapped* Connection::GetConnectOverlapped() const
{
	CHECKF(_impl);
	return _impl->_connect_overlapped;
}

void Connection::SetTotalBytes(std::size_t value)
{
	CHECK(_impl);
	_impl->_total_bytes = value;
}

std::size_t Connection::GetTotalBytes() const
{
	CHECKF(_impl);
	return _impl->_total_bytes;
}

void Connection::SetSentBytes(std::size_t value)
{
	CHECK(_impl);
	_impl->_sent_bytes = value;
}

std::size_t Connection::GetSentBytes() const
{
	CHECKF(_impl);
	return _impl->_sent_bytes;
}

void Connection::ResizeWriteBuffer(std::size_t new_size)
{
	CHECK(_impl);
	CHECK(_impl->_write_buffer.New(new_size));
	_impl->_write_buffer_size = new_size;
}

std::size_t Connection::GetWriteBufferSize() const
{
	CHECKF(_impl);
	return _impl->_write_buffer_size;
}

void* Connection::GetWriteBuffer()
{
	CHECKF(_impl);
	return _impl->_write_buffer;
}

void* Connection::GetReadBuffer()
{
	CHECKF(_impl);
	return &_impl->_read_buffer;
}

std::size_t Connection::GetReadBufferSize()
{
	CHECKF(_impl);
	return sizeof(_impl->_read_buffer);
}

char* Connection::GetMsgBuffer()
{
	CHECKF(_impl);
	return _impl->_msg_buff;
}

SOCKET Connection::GetSocket() const
{
	CHECKF(_impl);
	//ASSERTNET(IS_VALID_SOCKET_IDD(_impl->_socket));
	return _impl->_socket;
}

void Connection::GetPeerAddrInfo(char*&ip, const I64 nSize, U16& nPort)
{
	gtool::GetPeerAddr(GetSocket(), ip, nSize, nPort);
}

int Connection::GetMsgPos()
{
	CHECKF(_impl);
	return _impl->_pos;
}

void Connection::SetMsgPos(int pos)
{
	CHECK(_impl);
	_impl->_pos = pos;
}