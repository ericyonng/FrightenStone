#include<PoolSrc/MemPoolModule.h>
#include <BaseCode/Win/ThreadPool.h>
#include <PoolSrc/MemoryPool.h>

NAMESPACE_BEGIN(gtool)

bool MemPoolInitModule()
{
	return MemPoolMgr.Init();
}

void MemPoolFiniModule()
{
	MemPoolMgr.Fini();
}

bool MemPoolIsInit()
{
	return MemPoolMgr.IsInit();
}

void *MemPoolAlloc(const msize_t nByte, const char * szErrBuffer)
{
	return MemPoolMgr.Alloc(nByte, szErrBuffer);
}

void MemPoolFreeAlloc(void *pObj)
{
	MemPoolMgr.FreeAlloc(pObj);
}

bool	MemPoolIsValidPt(const void* p)
{
	return MemPoolMgr.IsValidPt(p);
}

NAMESPACE_END(gtool)