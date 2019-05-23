#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

/*	内存池													*/
/*	作用：提高程序性能										*/
/*	设计思路：												*/
/*	1.内存池包括一个总控制头页，n个大内存块，每个内存块带一个控制头页，一个未分割区（list），一个正在使用区（list），一个回收区域（list），一个融合区（map）	，一条维护线程*/
/*	2.所有内存申请优先从未分割区开始，未分割区用完的，可申请一个同等大小的新的大内存块（根据当前可用内存申请内存），并把其放置于页链表最前端,			*/
/*	3.每个从未分割区获取的内存，都带有一个子编号和一个主编号，并带有一个由主编号和自编号组成的全局唯一标识id用于快速的访问	*/
/*	4.正在使用区中内存释放，把释放的内存放置于回收区等待融合				*/
/*	5.维护线程定时检查回收区域是否有可回收的内存块，并将其转移到融合区，融合区只融合“相邻的内存块”直到第一个不相邻的截止，（相邻是指：物理地址上的相邻（可利用相邻的编号来进行优化）），融合完的内存应该被pushback到未分割区域内存，循环利用								*/
/*	6.维护线程还需要把最大可用内存的页放置到，页链表的最前端，*/
/*	6.每次内存的操作都应该更新相应的控制头页信息*/
/*	7.内存分配原则必须遵循内存对齐以及最小分配单元原则，并从页链表的最前端开始分配内存，并从未分割区获取最终的内存*/
/*	8.分配：计算出所需的最终内存，选择一个内存页，选择一个合适的未分割区内存，进行分配，每次申请空间时候都要判断当前内存页是否分配完，分配完则把该页面放到内存页面管理链表的最后一个节点*/
/*	9.融合区以块的其实地址作为key*/

/*	内存池使用准则															*/
/*	非类或结构体数据需要直接调用 memorypool的Alloc								*/
/*	类或结构体数据需要重载new和delete操作符 并在其中调用memorypool的Alloc		*/

//为提高性能，尽可能的减少new free的时间，new时候应判断线程释放lock住融合区，锁住则不宜将融合区内存转移至可分割区
//free 时回收线程应判断回收区锁是否被锁住，并尽量减少回收时候的时间片，时间片分配使用高精度时间戳


#pragma once

#include <BaseCode/MemoryPoolApi.h>
#include <PoolSrc/MemoryPoolStruct.h>

struct MemPageHead;
typedef gtool::TPtrList<MemPageHead, DEL_WAY_RELEASE, false>		MemPageList;				//内存页链表
class CThreadPool;

class MEMORY_POOL_API CMemPage2
{
	NO_COPY_NO_MOVE(CMemPage2);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CMemPage2);

public:
	//创建内存页
	bool Create(const msize_t nPageId, void *&pBegin, void *&pEnd, const msize_t nByte=MEMPAGE_MINIUM_SIZE);

	//内存操作
public:
	void *Alloc(const msize_t nByte, const char *& szErrBuffer);	//申请内存空间
	void FreeAlloc(void *pObj);	//释放内存
	bool IsValid(const void *& p);		//是否有效地址
	void *GetBeginOfBuffer();	//内存起始地址
	void *GetEndOfBuffer();	//内存结束地址
	msize_t GetSize();	//获取内存页大小

	//融合
public:
	bool LockMerge();
	void MergeMem();
	bool UnlockMerge();
	bool IsLockMerge();
	bool LockBusyMap();
	bool UnlockBusyMap();

	//辅助
protected:
	bool MoveToMergeMap(MemBlock& rBeginMerge, RecycleMemBlockAreaMap::iterator&IterMap);


	//状态信息
protected:
	//WaitForAllocAreaList::Iterator& GetAllocableMaxBlock();		//获取当前可用内存最大块

private:
	boost::atomic_bool											m_bInit{ false };				//是否已创建
	boost::atomic_bool											m_bCleans{ false };				//销毁
	atmc_sizet											m_atszPageID{ 0 };				//当前页编号
	atmc_sizet													m_atszMaxBlockID{ 0 };			//本页块编号最大值
	boost::atomic_address										m_patszBufferBegin{ NULL };		//内存起始地址
	boost::atomic_address										m_patszBufferEnd{ NULL };		//内存起始地址
	gtool::CSmartPtr<CDataLocker>								m_pSafeLocker = NULL;			//非融合区锁，保证内存分配的互斥性
	gtool::CSmartPtr<BusyMemBlockAreaMap>						m_pBusyMap = NULL;				//使用中内存块链表 key：该块内存块起始地址, val:MemBlock
	gtool::CSmartPtr<WaitForAllocAreaList>						m_pAllocAreaList = NULL;		//待分配内存块链表
	gtool::CSmartPtr<RecycleMemBlockAreaMap>					m_pRecycleAreaMap = NULL;		//回收内存区域链表 key：该块内存块起始地址, val:MemBlock
	gtool::CSmartPtr<CDataLocker>								m_pMergeMapLocker = NULL;		//融合区锁
	gtool::CSmartPtr<MemBlockMergeAreaMap>						m_pMergeAreaMap = NULL;			//待融合区域map key：该块内存块起始地址, val:MemBlock
	atmc_sizet													m_atszBufferSize{ 0 };			//内存区大小
	gtool::CSmartPtrC<char,char, DEL_WAY_DELETE_MULTI>			m_pPhysicBuffer = NULL;			//物理内存区块
};

struct MEMORY_POOL_API MemPageHead
{
	MemPageHead()
	{
		pPage2 = CMemPage2::CreateNew();
	}

	int Release()
	{
		IF_OK(pPage2)
			pPage2->Release();

		return 0;
	}
	atmc_sizet				atszPageID{ 0 };					//页编号唯一不变
	gtool::CSmartPtr<CMemPage2>		pPage2 = NULL;						//内存页管理者
	boost::atomic_address			atszAddrBegin{ 0 };					//内存起始
	boost::atomic_address			atszAddrEnd{ 0 };					//内存结束
};


class MEMORY_POOL_API CMemPoolMgr
{
	NO_COPY_NO_MOVE(CMemPoolMgr);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CMemPoolMgr);

	friend class CMaintainTask;
	//初始化配置
public:
	//初始化内存池
	bool Init();
	//释放内存池
	void Fini();

public:
	//申请内存空间
	void *Alloc(const msize_t& nByte, const char *& szErrBuffer);
	void FreeAlloc(void *pObj);
	bool		IsValidPt(const void* p);

	//辅助
public:
	bool IsInit()const { return m_bInit; }
	bool IsDestroy()const { return m_bIsDestroy; }

	//垃圾回收机制
protected:
	bool GCLock();
	bool GCUnlock();
	bool Wait();
	bool Wakeup();
	void MemMergeAll();

protected:
	MemPageHead * CreateNewPage(const msize_t nByte);

private:
	gtool::CSmartPtr<CDataLocker>				m_pLocker = NULL;						//内存分页链表锁
	gtool::CSmartPtr<CDataLocker>				m_pGarbageCollectorGuard = NULL;		//垃圾回收
	gtool::CSmartPtr<MemPageList>				m_pMemPageList = NULL;					//内存页链表
	atmc_sizet									m_atszMaxPageID{ 0 };					//最大页面id
	boost::atomic_bool							m_bInit{ false };						//初始化状态
	static boost::atomic_bool					m_bIsDestroy;							//是否释放
	boost::atomic<I32>							m_atm32GCPeriod{ MEM_POOL_GC_PERIOD };	//垃圾回收周期
	gtool::CSmartPtr<CThreadPool>				m_pThreadPool=NULL;
	boost::atomic<U64>							m_nAllocCount{ 0 };						//申请内存对象个数
	boost::atomic<U64>							m_nFreeCount{ 0 };						//释放对象个数
};

typedef MEMORY_POOL_API gtool::CSingleton<CMemPoolMgr> CCMemPoolMgrInstance;
extern MEMORY_POOL_API CMemPoolMgr *g_pMemPoolMgr;

#undef MemPoolMgr
#define MemPoolMgr (ASSERT(g_pMemPoolMgr), *g_pMemPoolMgr)

#endif



