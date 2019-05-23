#ifndef MEM_POOL_SWITCH_HPP_
#define MEM_POOL_SWITCH_HPP_

#pragma once

//#include <stdlib.h>
//#include <BaseCode/MemoryPool.inl>

NAMESPACE_BEGIN(gtool)

//转化成内存池创建对象
template<POOL_SWITCH_TYPE>
struct HeapHandle
{
	void * AllocObj()
	{
		return NULL;
	}

	void FreeObj()
	{
	}
};

template<>
struct HeapHandle<POOL_SWITCH_TYPE_CLOSE>
{
	HeapHandle(const char *szObjName)
	{
		UNUSED_ARG(szObjName);
	}

	void * AllocObj(const size_t& nByte)
	{
		return malloc(nByte);
	}

	void FreeObj(void * ptr)
	{
		free(ptr);
	}
};

#ifdef USE_MEM_POOL_H_
template<>
struct HeapHandle<POOL_SWITCH_TYPE_OPEN>
{
	HeapHandle(const char *szObjName)
	{
		m_szObjName[0] = 0;
		auto nLen = sprintf(m_szObjName, "%s", szObjName);
		nLen = gtool::Min(nLen, I32(BUFFER_LEN256 - 1));
		STRARRAY_ADD_ZERO_TAIL(m_szObjName, nLen);
	}

	void * AllocObj( const size_t& nByte)
	{
		return gtool::MemPoolAlloc(nByte, m_szObjName);
	}

	void FreeObj(void * ptr)
	{
		gtool::MemPoolFreeAlloc(ptr);
	}

	BUFFER256 m_szObjName;
};
#endif

NAMESPACE_END(gtool)

#undef	MEM_POOL_SWITCH
#define MEM_POOL_SWITCH(eSwitch, m_var)	\
public:	\
		void*  operator new	(size_t nByte)	{ return m_var.AllocObj(nByte); } /*静态成员函数*/	\
		void   operator delete	(void* p)	{ m_var.FreeObj(p);}			\
		void*  operator new[] (size_t nByte)	{ return m_var.AllocObj(nByte);}	\
		void   operator delete[] (void* p)	{ m_var.FreeObj(p); }	\
		bool IsMemPoolSwitchActive(){ return true; }	\
protected:\
static gtool::HeapHandle<POOL_SWITCH_TYPE(eSwitch)>		m_var;

//内存池创建对象
#define	MEM_POOL_SWITCH_IMPLEMENTATION(obj, eSwitch, m_var)									\
	gtool::HeapHandle<POOL_SWITCH_TYPE(eSwitch)>  obj::m_var(#obj);		

//内存池对象标准接口宏	 堆区创建	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//内存池创建对象
#undef MEMPOOL_CREATE
#define MEMPOOL_CREATE(m_var)	\
 MEM_POOL_SWITCH(POOL_SWITCH_TYPE_OPEN, m_var)

//内存池创建对象
#undef MEMPOOL_CREATE_IMPLEMENTATION
#define MEMPOOL_CREATE_IMPLEMENTATION(obj, m_var)	\
	MEM_POOL_SWITCH_IMPLEMENTATION(obj,POOL_SWITCH_TYPE_OPEN, m_var)

#endif

