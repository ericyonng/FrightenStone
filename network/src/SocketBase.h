#ifndef SOCKET_BASE_H_
#define SOCKET_BASE_H_
#pragma once

#include <BaseCode/NetworkServiceApi.h>

interface NETWORK_SERVICE_API ISocketEngine
{
	NO_COPY_NO_MOVE(ISocketEngine);
public:
	virtual bool Init() = NULL;
	virtual void Fini();
	virtual bool Send(void *&pData, U64 u64DataLen) = NULL;
	virtual bool Recv(void *&pData, U64& u64DataLen) = NULL;

protected:
	//∞Û∂®µÿ÷∑
	//virtual BOOL Bind(char *ip, int iplen, unsigned short port, unsigned short family) = NULL;
	//virtual BOOL Connect() = NULL;
};

ISocketEngine::ISocketEngine()
{
}

ISocketEngine::~ISocketEngine()
{
}

#endif