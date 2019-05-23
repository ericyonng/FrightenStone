#ifndef SOCKET_INTERFACE_H_
#define SOCKET_INTERFACE_H_

#pragma once
#include <BaseCode/NetworkServiceApi.h>
class NETWORK_SERVICE_API ISocketInterface
{
	NO_COPY_NO_MOVE(ISocketInterface);

public:
	virtual bool SendData() = NULL;
	virtual bool RecvData() = NULL;
	virtual bool Close() = NULL;
};

#endif

