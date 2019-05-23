#include<WinSock2.h>
#include"connection.h"

struct Overlapped
{
	enum Type
	{
		Connect_type,
		Accept_type,
		Read_type,
		Write_type,
	};
	WSAOVERLAPPED overlapped;

	Type type;

	WSABUF wsa_buf;

//	SOCKET _accepted_socket;

	Connection* connection;

	MEMPOOL_CREATE(m_MemObj);
};

inline Overlapped* CreateOverlapped(Overlapped::Type type)
{
	gtool::CSmartPtrC<Overlapped> pOverLapped = new Overlapped;
	CHECKF(pOverLapped);
	ZERO_CLEAN_PTR(pOverLapped, sizeof(Overlapped));
	pOverLapped->type = type;
	return pOverLapped.pop();
}