#include <PoolSrc/MemoryPool.h>
#include <BaseCode/Win/ThreadPool.h>




////////////////////////////////////////////////


//////////////////////////////////////////////////

CMemPoolMgr *g_pMemPoolMgr = CCMemPoolMgrInstance::GetInstance();

//维护内存池线程
class CMaintainTask :public CTaskBase
{
	NO_COPY_NO_MOVE(CMaintainTask);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CMaintainTask);

public:
	//任务执行体
	virtual int Run();
};

CMaintainTask::CMaintainTask()
{
}

CMaintainTask::~CMaintainTask() 
{

}

void CMaintainTask::Cleans()
{
}

int CMaintainTask::Run()
{
	//将回收区移至融合区
	//并将融合区内存与可分割区内存融合
	auto pMemMgrObj= (CMemPoolMgr*)(this->m_pArg);
	CHECKF(pMemMgrObj);
	CHECKF(pMemMgrObj->IsInit());

	gtool::CCpuInfo rCpu;
	rCpu.Startup();

	while (!pMemMgrObj->IsDestroy())
	{
		IF_NOT(pMemMgrObj->GCLock())
			continue;

		ASSERTEX(pMemMgrObj->Wait());
		ASSERTEX(pMemMgrObj->GCUnlock());

		//闲时垃圾回收
		if (rCpu.IsIdleStatus())
		{
			pMemMgrObj->MemMergeAll();
		}		
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////
boost::atomic_bool CMemPoolMgr::m_bIsDestroy{ false };
CMemPoolMgr::CMemPoolMgr()
{
	m_pLocker = CDataLocker::CreateNew(true);
	m_pMemPageList = MemPageList::CreateNew();
	m_pThreadPool = CThreadPool::CreateNew();
	m_pGarbageCollectorGuard = CDataLocker::CreateNew(true);
}

CMemPoolMgr::~CMemPoolMgr()
{
	Cleans();
}

void CMemPoolMgr::Cleans()
{
	Fini();
}

bool CMemPoolMgr::Init()
{
	CHECKF(m_pThreadPool);
	CHECKF(m_pLocker);
	CHECKF(m_pMemPageList);
	CHECKF(m_pGarbageCollectorGuard);

	m_pThreadPool->SetThreadNumLimit(1, 1);

	CHECKF(m_pLocker->Lock());
	IF_NOT(!m_bInit)
	{
		ASSERTEX(m_pLocker->UnLock());
		return false;
	}	

	MEMORYSTATUS rMemInfo;
	msize_t nMemNeeded = 0;	

	//创建n个内存页
	void *pBegin = NULL;
	void *pEnd = NULL;
	for (I32 i = 0; i < MEMPAGE_INIT_NUM; ++i)
	{
		gtool::CSmartPtrC<MemPageHead> pMemPageStruct = new MemPageHead;
		IF_NOT(pMemPageStruct)
		{
			ASSERTEX(m_pLocker->UnLock());
			return false;
		}		

		nMemNeeded = gtool::GetSysAlignByte(MEMPAGE_MINIUM_SIZE);
		IF_NOT(nMemNeeded > 0)
			break;
		
		gtool::CSmartPtr<CMemPage2> pPage2 = CMemPage2::CreateNew();
		IF_NOT(pPage2)
		{
			ASSERTEX(m_pLocker->UnLock());
			return false;
		}	

		IF_NOT(pPage2->Create(i + 1, pBegin, pEnd, nMemNeeded))
		{
			ASSERTEX(m_pLocker->UnLock());
			continue;
		}

		pMemPageStruct->atszPageID = i + 1;
		pMemPageStruct->atszAddrBegin = pPage2->GetBeginOfBuffer();
		pMemPageStruct->atszAddrEnd = pPage2->GetEndOfBuffer();
		pMemPageStruct->pPage2 = pPage2.pop();
		MemPageHead *pMemStructTmp = NULL;
		IF_NOT(m_pMemPageList->PushBack(pMemStructTmp=pMemPageStruct.pop()))
		{
			SAFE_DELETE(pMemStructTmp);
			ASSERTEX(m_pLocker->UnLock());
			return false;
		}
		m_atszMaxPageID = i + 1;
	}

	IF_NOT(!m_pMemPageList->IsEmpty())
	{
		ASSERTEX(m_pLocker->UnLock());
		return false;
	}

	gtool::CSmartPtr<CMaintainTask> pMaintainTask = CMaintainTask::CreateNew();
	IF_NOT(pMaintainTask)
	{
		ASSERTEX(m_pLocker->UnLock());
		return false;
	}

	CTaskBase *pTask = pMaintainTask.pop();
	pTask->SetArg(this);

	m_bInit = true;
	m_bIsDestroy = false;
	IF_NOT(m_pThreadPool->AddTask(*pTask))
	{
		ASSERTEX(m_pLocker->UnLock());
		m_bInit = false;
		return false;
	}

	ASSERTEX(m_pLocker->UnLock());

	return true;
}

void CMemPoolMgr::Fini()
{
	if (m_bIsDestroy)
		return;

	m_bInit = false;

	m_bIsDestroy = true;

	//唤醒垃圾回收线程
	ASSERTEX(Wakeup());

	IF_OK(m_pThreadPool)
		m_pThreadPool->Destroy();

	CHECK(m_pLocker);
	CHECK(m_pLocker->Lock());

	IF_OK(m_pMemPageList)
		m_pMemPageList->Clear();

	ASSERTEX(m_pLocker->UnLock());

	//内存池分配空间计数情况打印
	U64 nAllocCount = m_nAllocCount;
	U64 nFreeCount = m_nFreeCount;
	LOGSYSFMT_C512("[MemPoolInfo] AllocCount[%I64u]. FreeCount[%I64u].", nAllocCount, nFreeCount);
}

void * CMemPoolMgr::Alloc(const msize_t& nByte,const char *& szErrBuffer)
{
	CHECKF(m_bInit);
	CHECKF(m_pLocker);
	CHECKF(m_atszMaxPageID);
	CHECKF(m_pMemPageList);
	CHECKF(m_pLocker->Lock());
	
	//寻找可用的内存页并分配
	void *pObj = NULL;
	auto IterList = m_pMemPageList->NewEnum();
	msize_t nNeedByte = 0;
	while (IterList.Next())
	{
		MemPageHead *pPage = IterList;
		IF_NOT(pPage)
			continue;

		CMemPage2 *pPage2 = pPage->pPage2;

		IF_NOT(pPage2)
			continue;

		IF_NOT(pPage->atszPageID)
			continue;
		
		auto pObjTmp = pPage2->Alloc(nNeedByte = gtool::GetMemPoolAlignByte(nByte, pPage2->GetSize()), szErrBuffer);
		if (pObjTmp)
		{			
			pObj = pObjTmp;
			break;
		}
	}

	//当前内存池内存不够用开辟新的内存页
	if (!pObj)
	{
		auto pPageStruct = CreateNewPage(gtool::GetMemPoolAlignByte(nByte, -1));
		if (pPageStruct)
		{
			IF_OK(pPageStruct->pPage2)
			{
				pObj = pPageStruct->pPage2->Alloc(gtool::GetMemPoolAlignByte(nByte, pPageStruct->pPage2->GetSize()), szErrBuffer);
			}
		}
	}

	ASSERTEX(m_pLocker->UnLock());

	//分配空间计数
	if (pObj)
		++m_nAllocCount;

	return pObj;
}

void CMemPoolMgr::FreeAlloc(void *pObj)
{
	CHECK(m_bInit);
	CHECK(m_pLocker);
	CHECK(m_pMemPageList);
	CHECK(m_pLocker->Lock());

	bool bFree = false;
	auto IterList = m_pMemPageList->NewEnum();
	void * addrBegin =NULL;
	void * addrEnd = NULL;
	while (IterList.Next())
	{
		MemPageHead *pPage = IterList;
		IF_NOT(pPage)
			continue;

		CMemPage2 *pPage2 = pPage->pPage2;
		IF_NOT(pPage2)
			continue;

		IF_NOT(pPage->atszPageID)
			continue;

		IF_NOT(pPage->atszAddrBegin)
			continue;

		IF_NOT(pPage->atszAddrEnd)
			continue;

		addrBegin = (void *)pPage->atszAddrBegin;
		addrEnd = (void *)pPage->atszAddrEnd;
		if (msize_t(addrBegin) <= msize_t(pObj)
			&& msize_t(addrEnd) >= msize_t(pObj))
		{
			pPage2->FreeAlloc(pObj);
			bFree = true;
			break;
		}
	}
	ASSERTEX(bFree);
	ASSERTEX(m_pLocker->UnLock());

	if (bFree)
		++m_nFreeCount;
}

bool CMemPoolMgr::IsValidPt(const void* p)
{
	CHECKF(m_bInit);
	CHECKF(m_pLocker);
	CHECKF(m_pMemPageList);
	if (!p)
		return false;

	const U32 u32PtrAddr = U32(const_cast<void *>(p));
	CHECKF(m_pLocker->Lock());
	auto IterList = m_pMemPageList->NewEnum();
	U32 u32Begin = 0, u32End = 0;
	while (IterList.Next())
	{
		MemPageHead *pPage = IterList;
		IF_NOT(pPage)
			continue;

		CMemPage2* ptr = pPage->pPage2;
		IF_NOT(ptr)
			continue;
	
		u32Begin = U32(ptr->GetBeginOfBuffer());
		u32End = U32(ptr->GetEndOfBuffer());

		if (u32PtrAddr >= u32Begin && u32PtrAddr <= u32End)
		{
			ptr->LockBusyMap();
			auto bValid = ptr->IsValid(p);
			ptr->UnlockBusyMap();
			ASSERTEX(m_pLocker->UnLock());
			return bValid;
		}
	}

	ASSERTEX(m_pLocker->UnLock());

	return false;
}

bool CMemPoolMgr::GCLock()
{
	CHECKF(m_pGarbageCollectorGuard);
	CHECKF(m_pGarbageCollectorGuard->Lock());

	return true;
}

bool CMemPoolMgr::GCUnlock()
{
	CHECKF(m_pGarbageCollectorGuard);
	CHECKF(m_pGarbageCollectorGuard->UnLock());

	return true;
}

bool CMemPoolMgr::Wait()
{
	CHECKF(m_pGarbageCollectorGuard);

	CHECKF(m_pGarbageCollectorGuard->WaitEvent(m_atm32GCPeriod) != WIN_WAIT_EVENT_FAILED);

	return true;
}

bool CMemPoolMgr::Wakeup()
{
	CHECKF(m_pGarbageCollectorGuard);

	CHECKF(m_pGarbageCollectorGuard->Broadcast());

	return true;
}

void CMemPoolMgr::MemMergeAll()
{
	CHECK(m_pMemPageList);
	CHECK(m_pLocker);

	//让渡
	if (m_pLocker->Islock())
		return;

	DOTRY_B
	{
		//分配每个执行单元时间片
		gtool::CTimerMS rTimerMs;
		

		CHECK(m_pLocker->Lock());
		auto IterList = m_pMemPageList->NewEnum();
		rTimerMs.Startup(MEM_POOL_MERGE_TIME_OUT);
		while (IterList.Next())
		{
			MemPageHead *pPageStruct = IterList;
			IF_NOT(pPageStruct)
				continue;

			CMemPage2 *pPage2 = pPageStruct->pPage2;
			IF_NOT(pPage2)
				continue;

			DOTRY_B
			{
				pPage2->MergeMem();

			}
			DOCATCH_ANY("CMemPoolMgr::MemMergeAll()::while (IterList.Next())")
			{
// 				if (pPage2->IsLockMerge())
// 					pPage2->UnlockMerge();
			}
			DOCATCH_ANY_END

			//让渡时间片
			if (rTimerMs.IsTimeOut())
			{
				ASSERTEX(m_pLocker->WaitEvent(m_atm32GCPeriod) != WIN_WAIT_EVENT_FAILED);
				rTimerMs.Startup(MEM_POOL_MERGE_TIME_OUT);
			}
		}

		ASSERTEX(m_pLocker->UnLock());
	}
	DOCATCH_ANY("void CMemPoolMgr::MemMergeAll()")
	{
		if (m_pLocker->Islock())
			m_pLocker->UnLock();
	}
	DOCATCH_ANY_END
}

MemPageHead * CMemPoolMgr::CreateNewPage(const msize_t nByte)
{
	CHECKF(nByte > 0);
	gtool::CSmartPtrC<MemPageHead> pMemPageStruct = new MemPageHead;
	CHECKF(pMemPageStruct);

	msize_t nMemNeeded = ( ((nByte%MEMPAGE_MINIUM_SIZE)==0) ? nByte : ((nByte/ MEMPAGE_MINIUM_SIZE+1)*MEMPAGE_MINIUM_SIZE));
	nMemNeeded = gtool::GetSysAlignByte(nMemNeeded);
	CHECKF(nMemNeeded > 0);

	gtool::CSmartPtr<CMemPage2> pPage2 = CMemPage2::CreateNew();
	CHECKF(pPage2);

	pMemPageStruct->atszPageID = m_atszMaxPageID + 1;

	//*(const_cast<atmc_sizet *>(&(pMemPageStruct->atszPageID))) = 

	void *pBegin = NULL;
	void *pEnd = NULL;
	CHECKF(pPage2->Create(m_atszMaxPageID + 1, pBegin, pEnd, nMemNeeded));

	pMemPageStruct->atszAddrBegin = pPage2->GetBeginOfBuffer();
	pMemPageStruct->atszAddrEnd = pPage2->GetEndOfBuffer();
	pMemPageStruct->pPage2 = pPage2.pop();

	MemPageHead *pMemStructTmp = NULL;
	IF_NOT(m_pMemPageList->PushBack(pMemStructTmp = pMemPageStruct.pop()))
	{
		SAFE_DELETE(pMemStructTmp);
		return NULL;
	}

	if (pMemStructTmp)
	{
		++m_atszMaxPageID;
	}

	return pMemStructTmp;
}

CMemPage2::CMemPage2()
{
	m_pSafeLocker = CDataLocker::CreateNew(true);
	m_pBusyMap = BusyMemBlockAreaMap::CreateNew();
	m_pAllocAreaList = WaitForAllocAreaList::CreateNew();
	m_pRecycleAreaMap = RecycleMemBlockAreaMap::CreateNew();
	m_pMergeMapLocker = CDataLocker::CreateNew(true);
	m_pMergeAreaMap = MemBlockMergeAreaMap::CreateNew();
}

CMemPage2::~CMemPage2()
{
}

void CMemPage2::Cleans()
{
	m_bInit = false;

	IF_OK(m_pSafeLocker)
	{
		ASSERTEX(m_pSafeLocker->Lock());
	}

	if (m_bCleans)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return;
	}
	m_bCleans = true;

// 	IF_OK(m_pMergeMapLocker)
// 	{
// 		ASSERTEX(m_pMergeMapLocker->Lock());
// 	}

	//融合区先清除
	IF_OK(m_pRecycleAreaMap)
	{
		//ASSERTEX(m_pRecycleAreaMap->empty());
		m_pRecycleAreaMap->Cleans();
	}

	ASSERTEX(LockMerge());
	IF_OK(m_pMergeAreaMap)
	{
		//ASSERTEX(m_pMergeAreaMap->empty());
		m_pMergeAreaMap->Cleans();
	}
	ASSERTEX(UnlockMerge());


// 	IF_OK(m_pMergeMapLocker)
// 	{
// 		ASSERTEX(m_pMergeMapLocker->UnLock());
// 	}



	//使用区内存
	IF_OK(m_pBusyMap)
	{
		IF_NOT(m_pBusyMap->empty())
		{
			//内存泄露
			MemBlock *pBlock = NULL;
			BUFFER1024 szBuffer;
			szBuffer[0] = 0;
			I32 nStrLen = 0;
			msize_t mszPageid = 0, mszBlockId = 0;
			void *pAddrBegin = NULL, *pAddrEnd = NULL;
			I64 nCountByte = 0, nCountObj = 0;
			LOGMEMLEAK("【内存池内存泄露信息】");
			for (auto Iter = m_pBusyMap->begin(); Iter != m_pBusyMap->end(); ++Iter)
			{
				pBlock = Iter->second;
				IF_NOT(pBlock)
					continue;
				mszPageid = pBlock->atszPageID;
				mszBlockId = pBlock->atszBlockID;
				pAddrBegin = pBlock->pAddrBegin;
				pAddrEnd = pBlock->pAddrEnd;
				szBuffer[0] = 0;
				nStrLen = sprintf(szBuffer, "内存池 pageid[%I64d] blockid[%I64d] addrbegin[0x%p] addrend[0x%p] 泄露信息【%s】"
					, mszPageid, mszBlockId, pAddrBegin, pAddrEnd, pBlock->szErrBuffer);
				nStrLen = ((nStrLen < BUFFER_LEN1024) ? gtool::Max(nStrLen, 0) : (BUFFER_LEN1024 - 1));
				szBuffer[nStrLen] = 0;
				LOGMEMLEAK(szBuffer);
				nCountByte += (I64(pAddrEnd) - I64(pAddrBegin));
				++nCountObj;
			}
			LOGMEMLEAK("泄露字节[", nCountByte, "]. 泄露对象总数[", nCountObj, "]");
			LOGMEMLEAK("【内存池内存泄露信息】 END.");
		}

		m_pBusyMap->Cleans();
	}

	IF_OK(m_pAllocAreaList)
	{
		m_pAllocAreaList->Clear();
	}

	IF_OK(m_pSafeLocker)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
	}
		
}
bool CMemPage2::Create(const msize_t nPageId, void *&pBegin, void *&pEnd, const msize_t nByte/*=MEMPAGE_MINIUM_SIZE*/)
{
	CHECKF(nPageId > 0);
	CHECKF(nByte != 0);
	CHECKF(!m_bCleans);
	CHECKF(!m_bInit);
	CHECKF(m_pSafeLocker);
	CHECKF(m_pSafeLocker->Lock());	//临界区

	IF_NOT(!m_pPhysicBuffer)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}

	IF_NOT(m_pBusyMap)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}

	IF_NOT(m_pAllocAreaList)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}

	IF_NOT(m_pRecycleAreaMap)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}

// 	IF_NOT(m_pMergeMapLocker)
// 	{
// 		ASSERTEX(m_pSafeLocker->UnLock());
// 		return false;
// 	}

	IF_NOT(m_pMergeAreaMap)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}

	//检验内存对齐，以及内存单元的整数倍
	auto nFactByte = gtool::GetMemPoolAlignByte(nByte,-1);
	IF_NOT(nFactByte > 0)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}
	
	m_pPhysicBuffer = new char[nFactByte];
	IF_NOT(m_pPhysicBuffer)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}

	m_atszBufferSize = nFactByte;
	m_atszPageID = nPageId;

	//第一个可分配block
	gtool::CSmartPtrC<MemBlock> pBlock = new MemBlock;
	IF_NOT(pBlock)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return false;
	}
	pBlock->atszBlockID = 1;			//初始编号为1
	pBlock->atszPageID = nPageId;
	void *pBuffer = m_pPhysicBuffer;
	pBlock->pAddrBegin = (void *)(pBuffer);
	pBlock->pAddrEnd = (void *)((char *)(pBuffer) +nFactByte - 1);
	MemBlock *pBlockTmp = NULL;

	IF_NOT(m_pAllocAreaList->PushBack(pBlockTmp = pBlock.pop()))
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		SAFE_DELETE(pBlockTmp);		
		return false;
	}
	msize_t nCurMax = m_atszMaxBlockID;
	m_atszMaxBlockID =gtool::Max(nCurMax, msize_t(pBlockTmp->atszBlockID));
	pBegin = pBlockTmp->pAddrBegin;
	pEnd = pBlockTmp->pAddrEnd;
	m_patszBufferBegin = pBegin;
	m_patszBufferEnd = pEnd;
	ASSERTEX(m_pSafeLocker->UnLock());

	m_bInit = true;
	return true;
}

void * CMemPage2::Alloc(const msize_t nByte, const char *& szErrBuffer)
{
	CHECKF(m_bInit);
	CHECKF(m_pSafeLocker);
	CHECKF(m_pAllocAreaList);
	CHECKF(m_pMergeAreaMap);
	CHECKF(m_pBusyMap);
	CHECKF(szErrBuffer);
//	CHECKF(m_pMergeMapLocker);
	CHECKF(m_pSafeLocker->Lock());

	//从融合区将融合完的内存载入待分割区
//	CHECKF(m_pMergeMapLocker->Lock());

	//判断是否锁住，避免上下文切换
	if (!IsLockMerge())
	{
		ASSERTEX(LockMerge());

		if (!m_pMergeAreaMap->empty())
		{
			for (auto IterMap = m_pMergeAreaMap->begin(); IterMap != m_pMergeAreaMap->end(); )
			{
				//全部转到待分割区
				MemBlock *pBlock = IterMap->second;
				IF_NOT(pBlock)
					continue;

				IF_OK(m_pAllocAreaList->PushBack(pBlock))
				{
					//从融合区移除
					IterMap = m_pMergeAreaMap->PopObj(IterMap, true);
					if (IterMap == m_pMergeAreaMap->end())
						break;

					if (m_pMergeAreaMap->empty())
						break;

					continue;
				}

				++IterMap;
			}
		}

		ASSERTEX(UnlockMerge());
	}
	
	

//	ASSERTEX(m_pMergeMapLocker->UnLock());

//	CHECKF(m_pSafeLocker->Lock());

	//寻找合适单元
	auto IterList = m_pAllocAreaList->NewEnum();
	MemBlock *pBlockOld = NULL;
	WaitForAllocAreaList::Iterator IterOld;
	msize_t nBlockBytes = 0;
	msize_t nFactBytes = gtool::GetMemPoolAlignByte(nByte, m_atszBufferSize);
	MemBlock * pBlockTmp = NULL;
	void *pBegin = NULL, *pEnd = NULL;
	while (IterList.Next())
	{
		pBlockTmp = IterList;
		IF_NOT(pBlockTmp)
			continue;

		pBegin = pBlockTmp->pAddrBegin;
		pEnd = pBlockTmp->pAddrEnd;
		nBlockBytes = (msize_t)(pEnd)-(msize_t)(pBegin);
		IF_NOT(nBlockBytes > 0)
			continue;

		if (nBlockBytes >= nFactBytes)
		{
			IterOld = IterList;
			pBlockOld = pBlockTmp;
			break;
		}		
	}

	//块地址校验
	if (!pBlockOld)
	{
		//没有合适内存从融合区找合适内存
		ASSERTEX(m_pSafeLocker->UnLock());
		return NULL;
	}
		
	//块大小校验
	IF_NOT(nBlockBytes > 0)
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return NULL;
	}

	//将旧的内存块放入已使用
	void *pBlockBeginAddr = pBlockOld->pAddrBegin;
	IF_NOT (!m_pBusyMap->IsExist(U32(pBlockBeginAddr)))
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return NULL;
	}
	m_pBusyMap->insert(std::make_pair(U32(pBlockBeginAddr), pBlockOld));
	IF_NOT (m_pBusyMap->IsExist(U32(pBlockBeginAddr)))
	{
		ASSERTEX(m_pSafeLocker->UnLock());
		return NULL;
	}

	//旧的弹出
	IF_OK(IterOld)
	{
		IF_NOT(m_pAllocAreaList->PopObj(IterOld))
		{
			ASSERTEX(m_pSafeLocker->UnLock());
			return NULL;
		}
	}

	//切割n字节空间 将大的空间切割成两份，新的块插入链表的前端
	if (nBlockBytes > nFactBytes)
	{
		//空间比要分配的大切割成两份
		gtool::CSmartPtrC<MemBlock> pBlockNew = new MemBlock;
		IF_NOT(pBlockNew)
		{
			ASSERTEX(m_pSafeLocker->UnLock());
			return NULL;
		}			

		//新块信息
		pBlockNew->atszPageID = (msize_t)pBlockOld->atszPageID;
		++m_atszMaxBlockID;
		pBlockNew->atszBlockID = msize_t(m_atszMaxBlockID);
		void *pBlockOldBegin = pBlockOld->pAddrBegin;
		pBlockNew->pAddrBegin = (char *)(pBlockOldBegin)+nFactBytes;
		pBlockNew->pAddrEnd = (void *)(pBlockOld->pAddrEnd);

		MemBlock *pNewTmp = NULL;
		IF_NOT(m_pAllocAreaList->PushFront(pNewTmp = pBlockNew.pop()))
		{
			ASSERTEX(m_pSafeLocker->UnLock());
			SAFE_DELETE_NL(pNewTmp);
			return NULL;
		}

		//旧块信息变更
		pBlockOld->pAddrEnd = (char *)((void *)(pNewTmp->pAddrBegin)) - 1;
	}

	//内存泄露信息写入
	auto nStrlen = sprintf(pBlockOld->szErrBuffer, "%s", szErrBuffer);
	nStrlen = (nStrlen >= BUFFER_LEN256) ? (BUFFER_LEN256 - 1) : gtool::Max(nStrlen, 0);
	pBlockOld->szErrBuffer[nStrlen] = 0;

	ASSERTEX(m_pSafeLocker->UnLock());

	return pBlockOld->pAddrBegin;
}

void CMemPage2::FreeAlloc(void *pObj)
{
	CHECK(pObj);
	CHECK(m_bInit);
	CHECK(m_pSafeLocker);
//	CHECK(m_pMergeMapLocker);

	CHECK(m_pSafeLocker->Lock());	//临界区
// 	IF_NOT(m_pMergeMapLocker->Lock())
// 	{
// 		ASSERTEX(m_pSafeLocker->UnLock());
// 		return;
// 	}

	IF_NOT(m_pBusyMap)
	{
//		ASSERTEX(m_pMergeMapLocker->UnLock());
		ASSERTEX(m_pSafeLocker->UnLock());
		return;
	}

	IF_NOT(m_pRecycleAreaMap)
	{
//		ASSERTEX(m_pMergeMapLocker->UnLock());
		ASSERTEX(m_pSafeLocker->UnLock());
		return;
	}

	//该块内存已经在回收区
	IF_NOT(!m_pRecycleAreaMap->IsExist(U32(pObj)))
	{
//		ASSERTEX(m_pMergeMapLocker->UnLock());
		ASSERTEX(m_pSafeLocker->UnLock());
		return;
	}

	//该块内存不在使用区
	IF_NOT(m_pBusyMap->IsExist(U32(pObj)))
	{
//		ASSERTEX(m_pMergeMapLocker->UnLock());
		ASSERTEX(m_pSafeLocker->UnLock());
		return;
	}

	//从使用区弹出
	MemBlock* pBlockOld = m_pBusyMap->PopObj(U32(pObj));
	CHECK(pBlockOld);
	IF_NOT(!m_pBusyMap->IsExist(U32(pObj)))
	{
//		ASSERTEX(m_pMergeMapLocker->UnLock());
		ASSERTEX(m_pSafeLocker->UnLock());		
		return;
	}

	gtool::CSmartPtrC<MemBlock> pOldMemBlock = pBlockOld;
	IF_NOT(pOldMemBlock)
	{
		m_pBusyMap->insert(std::make_pair(U32(pObj), pBlockOld));
		ASSERTEX(m_pBusyMap->IsExist(U32(pObj)));

//		ASSERTEX(m_pMergeMapLocker->UnLock());
		ASSERTEX(m_pSafeLocker->UnLock());		
		return;
	}
	pBlockOld = NULL;
	pOldMemBlock->szErrBuffer[0] = 0;

	//放入回收区
	m_pRecycleAreaMap->insert(std::make_pair(U32(pObj), pBlockOld = pOldMemBlock.pop()));
	IF_NOT(m_pRecycleAreaMap->IsExist(U32(pObj)))
	{
		void *pBeginAddr = pBlockOld->pAddrBegin;
		void *pEndAddr = pBlockOld->pAddrEnd;
		msize_t nPageid = pBlockOld->atszPageID;
		msize_t nBlockid = pBlockOld->atszBlockID;
		LOGERR("内存池内存丢失(有内存暴涨风险)：pageid:[", nPageid, "] blockid:[", nBlockid, "], addressbegin：[", (U32)(pBeginAddr), "],addressend:[", reinterpret_cast<U32>(pEndAddr), "].");
		SAFE_DELETE_NL(pBlockOld);

//		ASSERTEX(m_pMergeMapLocker->UnLock());
		ASSERTEX(m_pSafeLocker->UnLock());		
		return;
	}

//	ASSERTEX(m_pMergeMapLocker->UnLock());
	ASSERTEX(m_pSafeLocker->UnLock());

}

bool CMemPage2::IsValid(const void *& p)
{
	CHECKF(m_pBusyMap);

	U32 u32Ptr = U32(p);

	return m_pBusyMap->IsExist(u32Ptr);
}

void * CMemPage2::GetBeginOfBuffer()
{
	return m_patszBufferBegin;
}

void * CMemPage2::GetEndOfBuffer()
{
	return m_patszBufferEnd;
}

msize_t CMemPage2::GetSize()
{
	return m_atszBufferSize;
}

bool CMemPage2::LockMerge()
{
	CHECKF(m_pMergeMapLocker);

	return m_pMergeMapLocker->Lock();
}

//设置合并时间片不超过
//先将RecycleMap中的数据转移到临时的Map中，然后再从临时的Map转移到MergeMap中
void CMemPage2::MergeMem()
{
	//这个操作不设定超时
	gtool::CTimerMS tTimer;
	CHECK(m_pRecycleAreaMap);
	CHECK(m_pMergeAreaMap);
	CHECK(m_pSafeLocker);

	//让渡时间片
	if (m_pSafeLocker->Islock())
		return;	

	//变量定义
	gtool::CSmartPtr<TempMemBlockShiftMap> pTempMap = TempMemBlockShiftMap::CreateNew();
	CHECK(pTempMap);

	//将内存recycle先融合一部分放入融合区，融合区再进行融合放入达到对齐标准的放入success区
	MemBlock *pBeginMerge = NULL;

	//粗合并
	CHECK(m_pSafeLocker->Lock());
	tTimer.Startup(MEM_POOL_MERGE_TIME_OUT);
	if (!m_pRecycleAreaMap->empty())
	{
		U32 ToDelRecycle = 0;
		bool bIncreasable = true;
		for (auto IterMap = m_pRecycleAreaMap->begin(); IterMap != m_pRecycleAreaMap->end(); (bIncreasable? (++IterMap) : IterMap))
		{
			bIncreasable = true;

			//首节点
			if (!pBeginMerge)
			{
				pBeginMerge = IterMap->second;
			}
			IF_NOT(pBeginMerge)
				continue;

			IF_NOT(pBeginMerge->pAddrBegin != NULL
				&& pBeginMerge->pAddrEnd != NULL
				&& pBeginMerge->pAddrBegin != pBeginMerge->pAddrEnd)
			{
				pBeginMerge = NULL;
				continue;
			}

			//合并节点
			auto IterNext = IterMap;
			MemBlock *pBlockNext = IterNext->second;
			IF_NOT(pBlockNext)
				continue;

			IF_NOT(pBlockNext->pAddrBegin != NULL
				&& pBlockNext->pAddrEnd != NULL
				&& pBlockNext->pAddrBegin != pBlockNext->pAddrEnd)
			{
				continue;
			}

			if (pBlockNext == pBeginMerge)
				continue;

			pBlockNext->szErrBuffer[0] = 0;
			void *pNextBlockBegin = pBlockNext->pAddrBegin;
			void *pEndCur = pBeginMerge->pAddrEnd;

			//判断两节点连续性 不连续则放入融合区
			if (msize_t(pEndCur) != (msize_t(pNextBlockBegin) - 1))
			{
				//不连续则将下一个节点设置成要合并的节点

				//转移至融合区
				ASSERTEX(LockMerge());
				ASSERTEX(MoveToMergeMap(*pBeginMerge, IterMap));
				ASSERTEX(UnlockMerge());

				if (m_pRecycleAreaMap->empty() || IterMap == m_pRecycleAreaMap->end())
					break;

				//设置下一个合并节点
				pBeginMerge = pBlockNext;

				//超时跳出
				if (tTimer.ToNextTime())
				{
					ASSERTEX(m_pSafeLocker->UnLock());
					return;
				}
				continue;
			}

			//合并：地址空间合并，并释放下一个节点
			void *pNextEnd = pBlockNext->pAddrEnd;
			pBeginMerge->pAddrEnd = pNextEnd;
			pBeginMerge->szErrBuffer[0] = 0;

			//移除原先内存
			IterNext = m_pRecycleAreaMap->DelObj(IterNext);	//合并完真删除节点并获取下一个节点
			if (m_pRecycleAreaMap->empty() || IterNext == m_pRecycleAreaMap->end())
				break;

			IterMap = IterNext;	//重置节点
			bIncreasable = false;

			if (tTimer.ToNextTime())
			{
				ASSERTEX(m_pSafeLocker->UnLock());
				return;
			}
		}
	}


	ASSERTEX(m_pSafeLocker->UnLock());
}

bool CMemPage2::UnlockMerge()
{
	CHECKF(m_pMergeMapLocker);

	return m_pMergeMapLocker->UnLock();
}

bool CMemPage2::IsLockMerge()
{
	CHECKF(m_pMergeMapLocker);

	return m_pMergeMapLocker->Islock();
}

bool CMemPage2::LockBusyMap()
{
	CHECKF(m_pSafeLocker);

	return m_pSafeLocker->Lock();
}

bool CMemPage2::UnlockBusyMap()
{
	CHECKF(m_pSafeLocker);

	return m_pSafeLocker->UnLock();
}

bool CMemPage2::MoveToMergeMap(MemBlock& rBeginMerge, RecycleMemBlockAreaMap::iterator&IterMap)
{	
	CHECKF(m_pMergeAreaMap);
	void *nMergeBeginAddr = rBeginMerge.pAddrBegin;

	IF_NOT(!m_pMergeAreaMap->IsExist(U32(nMergeBeginAddr)))
	{
		auto pBlock = &rBeginMerge;
		msize_t MissMemSize = 0;
		IF_OK(pBlock)
		{
			MissMemSize = pBlock->GetSize();
		}
		LOGERR("内存有丢失风险，地址[", IterMap->first, "],大小[", MissMemSize, "]Byte.");
		return false;
	}		
					
	//大小符合要求
	if (rBeginMerge.GetSize() >= MEM_POOL_SUC_MERGE_SIZE)
	{
		m_pMergeAreaMap->insert(std::make_pair(U32(nMergeBeginAddr), &rBeginMerge));

		IF_OK(m_pMergeAreaMap->IsExist(U32(nMergeBeginAddr)))
		{
			auto U32ToDelRecycle = U32(nMergeBeginAddr);
			auto IterToDel = m_pRecycleAreaMap->find(U32ToDelRecycle);
			IF_OK(IterToDel != m_pRecycleAreaMap->end())
			{
				IF_OK(IterToDel != IterMap)
				{
					//ASSERTEX(m_pRecycleAreaMap->DelObj(ToDelRecycle));
					ASSERTEX(m_pRecycleAreaMap->PopObj(U32ToDelRecycle));	//弹出不可真删除 因为节点移交m_pMergeAreaMap管理
				}
			}
		}
	}
	
	return true;
}

// WaitForAllocAreaList::Iterator& CMemPage2::GetAllocableMaxBlock()
// {
// 
// }


