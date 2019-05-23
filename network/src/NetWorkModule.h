#ifndef NET_WORK_MODULE_H_
#define NET_WORK_MODULE_H_

#pragma once

#include <BaseCode/NetworkServiceApi.h>

NAMESPACE_BEGIN(gtool)
extern NETWORK_SERVICE_API bool NetWorkInitModule();

extern NETWORK_SERVICE_API void NetWorkFinishModule();

extern NETWORK_SERVICE_API bool NetWorkModulIsInit();

extern NETWORK_SERVICE_API void NetWorkModulGetSvrBindAddr(void *pBuffer, const U16 u16SizeBuffer);

extern NETWORK_SERVICE_API void NetWorkModulSetIp(const char *ip);
extern NETWORK_SERVICE_API void NetWorkModulSetSvrPort(const U16& nPort);

extern NETWORK_SERVICE_API void NetWorkModulSetGetBusinessLogicMainThread(const void * pfn);

extern NETWORK_SERVICE_API const void *NetWorkModulGetSenderThread();

NAMESPACE_END(gtool)

#endif

