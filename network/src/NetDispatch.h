#ifndef NET_DISPATCH_H_
#define NET_DISPATCH_H_

struct MesageHeader;
class Connection;

class CNetDispatch
{
	NO_COPY(CNetDispatch);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CNetDispatch);
public:
	void Init();
	void OnClientConnected(const Connection* conn);
	void OnClientDisconnected(const Connection& /*conn*/);
	void OnRead(const Connection* conn, void* data, std::size_t size);
	void OnWrite(const Connection* conn, std::size_t bytes_transferred);
	virtual void ProcessMessage(MesageHeader *mh, Connection* conn);

};

#endif // !_app_h_
