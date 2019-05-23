#include "callbacks.h"

CallbacksExecutor::CallbacksExecutor()
{
// 	OnWrite = nullptr;
// 	OnRead = nullptr;
// 	OnConnected = nullptr;
// 	OnDisconnected = nullptr;
}

void CallbacksExecutor::SetOnWriteCallback(Callback::OnWrite callback)
{
	OnWrite = callback;
}

void CallbacksExecutor::SetOnReadCallback(Callback::OnRead callback)
{
	OnRead = callback;
}

void CallbacksExecutor::SetOnConnectedCallback(Callback::OnConnected callback)
{
	OnConnected = callback;
}

void CallbacksExecutor::SetOnDisconnectedCallback(Callback::OnDisconnected callback)
{
	OnDisconnected = callback;
}