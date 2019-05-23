#ifndef MEMORY_POOL_STRUCT_H_
#define MEMORY_POOL_STRUCT_H_

#pragma once

#include <BaseCode/MemoryPoolApi.h>
class CMemPage2;


struct MEMORY_POOL_API MemBlock
{
	MemBlock()
	:atszPageID(0), atszBlockID(0), pAddrBegin(NULL), pAddrEnd(NULL)
	{
		szErrBuffer[0] = 0;
	}

	atmc_sizet atszPageID{ 0 };			//页编号
	atmc_sizet atszBlockID{ 0 };			//块编号
	atmc_memaddr	pAddrBegin{ NULL };			//起始地址
	atmc_memaddr	pAddrEnd{ NULL };			//结束地址
	BUFFER256		szErrBuffer;				//内存泄露信息
	msize_t GetSize()
	{
		if (pAddrBegin == 0 || pAddrEnd == 0)
			return 0;

		void *pBegin = pAddrBegin;
		void *pEnd = pAddrEnd;

		return msize_t(pEnd) - msize_t(pBegin);
	}
};

//内存池总控制头
struct MEMORY_POOL_API MemPoolControlHead
{
	atmc_sizet atszTotalPageNums{ 0 };				//内存页数
	atmc_sizet atszTotalMemSize{ 0 };				//内存总大小
	atmc_sizet atszCurUsed{ 0 };					//已用内存大小
};

typedef MEMORY_POOL_API gtool::CLitPtrMap<MemBlock, U32, DEL_WAY_DELETE>	BusyMemBlockAreaMap;		//使用中内存块map key：该块内存块起始地址, val:MemBlock
typedef MEMORY_POOL_API gtool::TPtrList<MemBlock, DEL_WAY_DELETE, false>	BusyMemBlockAreaList;		//使用中内存块链表
typedef MEMORY_POOL_API gtool::TPtrList<MemBlock, DEL_WAY_DELETE, false>	WaitForAllocAreaList;		//待分配区内存块链表
typedef MEMORY_POOL_API gtool::CLitPtrMap<MemBlock, U32, DEL_WAY_DELETE>	RecycleMemBlockAreaMap;		//回收区map key：该块内存块起始地址, val:MemBlock
typedef MEMORY_POOL_API gtool::CLitPtrMap<MemBlock, U32, DEL_WAY_DELETE>	TempMemBlockShiftMap;		//回收区到融合区过渡，应该作为局部对象，避免资源竞争
typedef MEMORY_POOL_API gtool::CLitPtrMap<MemBlock, U32, DEL_WAY_DELETE>	MemBlockMergeAreaMap;		//内存块融合区map key：该块内存块起始地址, val:MemBlock


#endif

