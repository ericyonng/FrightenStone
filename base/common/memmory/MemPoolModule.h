#ifndef MEM_POOL_MODULE_H_
#define MEM_POOL_MODULE_H_

#pragma once
#include <BaseCode/MemoryPoolApi.h>

//ÄÚ´æ³Ø½Ó¿Ú
NAMESPACE_BEGIN(gtool)

extern MEMORY_POOL_API bool MemPoolInitModule();

extern MEMORY_POOL_API void MemPoolFiniModule();

extern MEMORY_POOL_API bool MemPoolIsInit();

extern MEMORY_POOL_API void *MemPoolAlloc(const msize_t nByte, const char * szErrBuffer);

extern MEMORY_POOL_API void MemPoolFreeAlloc(void *pObj);

extern MEMORY_POOL_API bool	MemPoolIsValidPt(const void* p);

NAMESPACE_END(gtool)

#endif

