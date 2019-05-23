#ifndef _connection_h_
#define _connection_h_
#include<memory>
#include<WinSock2.h>
#define ReadBufferSize 1024*10
struct Overlapped;

class Connection
{
	MEMPOOL_CREATE(m_MemObj);

public:
	Connection(const SOCKET& socket);
	~Connection();

public:

	SOCKET GetSocket() const;
	void GetPeerAddrInfo(char*&ip, const I64 nSize, U16& nPort);

	void* GetReadBuffer();

	std::size_t GetReadBufferSize();

	char* GetMsgBuffer();

	int GetMsgPos();

	void SetMsgPos(int pos);

	void* GetWriteBuffer();

	std::size_t GetWriteBufferSize() const;

	void ResizeWriteBuffer(std::size_t new_size);

	std::size_t GetSentBytes() const;

	void SetSentBytes(std::size_t value);

	std::size_t GetTotalBytes() const;

	void SetTotalBytes(std::size_t value);

	Overlapped* GetConnectOverlapped() const;

	Overlapped* GetAcceptOverlapped() const;

	Overlapped* GetReadOverlapped() const;

	Overlapped* GetWriteOverlapped() const;


	class Impl;
private:	
   mutable  gtool::CSmartPtrC<Impl, Impl> _impl;
};

#endif