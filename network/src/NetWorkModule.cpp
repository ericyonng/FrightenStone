#include <Net/NetWorkModule.h>
#include <Net/IOCPModel.h>


NAMESPACE_BEGIN(gtool)

bool NetWorkInitModule()
{
	return IocpMgr.Init();
}

void NetWorkFinishModule()
{
	IocpMgr.Fini();
}

bool NetWorkModulIsInit()
{
	return IocpMgr.IsMgrInit();
}

void NetWorkModulGetSvrBindAddr(void *pBuffer, const U16 u16SizeBuffer)
{
	return IocpMgr.GetBindAddrStr(pBuffer, u16SizeBuffer);
}

void NetWorkModulSetSvrPort(const U16& nPort)
{
	IocpMgr.SetPort(nPort);
}

void NetWorkModulSetIp(const char *ip)
{
	IocpMgr.SetIp(ip);
}

void NetWorkModulSetGetBusinessLogicMainThread(const void * pfn)
{
	IocpMgr.SetfnGetBusinessLogicMainThread(pfn);
}

const void *NetWorkModulGetSenderThread()
{
	return IocpMgr.GetSenderThread();
}

NAMESPACE_END(gtool)



