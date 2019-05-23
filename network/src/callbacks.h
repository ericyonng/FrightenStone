#ifndef _callbacks_h_
#define _callbacks_h_
#include <functional>
#include "connection.h"
namespace Callback
{
	typedef std::function<void(const Connection*, std::size_t)> OnWrite;
	typedef std::function<void(const Connection*, void*, std::size_t)> OnRead;
	typedef std::function<void(const Connection*)> OnConnected;
	typedef std::function<void(const Connection&)> OnDisconnected;
}

class CallbacksExecutor
{
	NO_COPY(CallbacksExecutor);
public:
	CallbacksExecutor();
	virtual ~CallbacksExecutor() {}

public:
	void SetOnWriteCallback(Callback::OnWrite callback);
	void SetOnReadCallback(Callback::OnRead callback);
	void SetOnConnectedCallback(Callback::OnConnected callback);
	void SetOnDisconnectedCallback(Callback::OnDisconnected callback);

public:
	Callback::OnWrite OnWrite = NULL;
	Callback::OnRead OnRead = NULL;
	Callback::OnConnected OnConnected = NULL;
	Callback::OnDisconnected OnDisconnected = NULL;
};


#endif