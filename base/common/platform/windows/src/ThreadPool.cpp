#include "StdAfx.h"

#include <BaseCode/Win/ThreadPool.h>
#include <BaseCode/Win/WinLocker.h>
#include<string.h>
#include <iostream>

#include<BaseCode/TaskBase.h>
#include <BaseCode/FileDirSys.h>
#include <BaseCode/CheckNoLog.h>
#define CLEAR_GROUP_WAIT_TIME_DEF	5000		//5ms等待任务池完成任务

#if defined(NEED_WINDOWS_COMMON_API)

#include <thread>
//线程池

//构造
CThreadPool::CThreadPool(int nMinNum, int nMaxNum)
{
	//初始化参数
	m_nMinNum = nMinNum;		//初始线程数
	m_nMaxNum = nMaxNum;		//最大线程数
	m_bIsDestroy = false;				//线程池销毁
	m_nCurNum = 0;					//当前线程数
	m_nWaitNum = 0;					//空闲线程数
	m_bIsStopAdd = false;				//是否停止增加任务
	m_nCacheUsed = 0;
	m_nCacheTopLimit = POOL_DEF_CACHE_SIZE;
	m_bEnableCache = false;
	m_nRePauseCachePercentage = POOL_DEF_CACHE_RE_PAUSE_PERCENTAGE;

	m_pCSynLockerObj = CDataLocker::CreateNew(true);
	m_plistTasklist = TASK_LIST::CreateNew();

	//初始化现场
	ASSERT(this->CreateThread(m_nMinNum));
}

CThreadPool::CThreadPool(CThreadPool&& rValue)
{
	m_pCSynLockerObj = std::move(rValue.m_pCSynLockerObj);
	m_plistTasklist = std::move(rValue.m_plistTasklist);
}

CThreadPool::CThreadPool()
{
	//初始化参数
	m_nMinNum = 0;		//初始线程数
	m_nMaxNum = 0;		//最大线程数
	m_bIsDestroy = false;				//线程池销毁
	m_nCurNum = 0;					//当前线程数
	m_nWaitNum = 0;					//空闲线程数
	m_bIsStopAdd = false;				//是否停止增加任务
	m_nCacheUsed = 0;
	m_nCacheTopLimit = POOL_DEF_CACHE_SIZE;
	m_bEnableCache = false;
	m_nRePauseCachePercentage = POOL_DEF_CACHE_RE_PAUSE_PERCENTAGE;

	m_pCSynLockerObj = CDataLocker::CreateNew(true);
	m_plistTasklist = TASK_LIST::CreateNew();
}

//析构
CThreadPool::~CThreadPool()
{

}

void CThreadPool::ClearPool()
{
	CHECK_NL(m_pCSynLockerObj&&m_plistTasklist);
	if (m_bClearPool)
		return;

	m_bClearPool = true;

	try
	{
		//停止添加任务
		StopAdd();							//停止任务添加

		//等待线程完成任务
		WIN_WAIT_EVENT nWaitRet = WIN_WAIT_EVENT_FAILED;
		bool bEmpty = false;
		const I64 nLoopMax = 1000;
		I64 nCountLoop = 0;
		while (true)
		{
			DEAD_LOOP_ASSERT(nCountLoop, nLoopMax);
			ASSERT(m_pCSynLockerObj->Lock());
			m_pCSynLockerObj->SinalOne();	//唤醒线程清理任务池
			m_pCSynLockerObj->UnLock();
			Sleep(CLEAR_GROUP_WAIT_TIME_DEF);
			ASSERT(m_pCSynLockerObj->Lock());
			bEmpty = m_plistTasklist->IsEmpty();
			ASSERT(m_pCSynLockerObj->UnLock());
			if (bEmpty)
				break;
		}

		IF_NOT_NL(m_plistTasklist->IsEmpty())
			m_plistTasklist->Clear();

		//线程退出
		m_bIsDestroy = true;

		bool bNoneThread = false;
		nCountLoop = 0;
		ASSERT(m_pCSynLockerObj->Broadcast());
// 		while (true)
// 		{
// 			DEAD_LOOP_ASSERT(nCountLoop, nLoopMax);
// 			ASSERT(m_pCSynLockerObj->Lock());
// 			m_pCSynLockerObj->SinalOne();
// 			nWaitRet = m_pCSynLockerObj->WaitEvent(CLEAR_GROUP_WAIT_TIME_DEF);		//5ms超时
// 			bNoneThread = m_nCurNum <= 0;
// 			ASSERT(m_pCSynLockerObj->UnLock());
// 			if (bNoneThread)
// 				break;
// 		}	
	}
	catch (...)
	{
		ASSERT(!"清理线程池发生CRASH*******");
	}
}

void CThreadPool::SetThreadNumLimit(const I32& nMin, const I32& nMax)
{
	CHECK_NL(nMin <= nMax && (!m_bIsDestroy) && (!m_bIsStopAdd));

	I32 nMaxNum = m_nMaxNum;
	m_nMaxNum = gtool::Max(nMaxNum, nMax);		//最大线程数

	auto nDiffMin = nMin - m_nMinNum;
	if (nDiffMin > 0)
	{
		m_nMinNum = nMin;		//初始线程数

		ASSERT(CreateThread(nDiffMin));
	}
}

//创建线程 需要加锁保护
bool CThreadPool::CreateThread(int nThreadNum)
{

	//校验参数
	CHECKF_NL (nThreadNum>=0 && nThreadNum<=m_nMaxNum);

	//参数
	unsigned int uThreadID;
	HANDLE ret;

	for (int i = 0; i < nThreadNum; i++)
	{
		//参数初始化
		ret = (HANDLE)::_beginthreadex(NULL, 0, ThreadHandler, (void *)(this), 0, &uThreadID);	//创建线程
		if (ret != INVALID_HANDLE_VALUE)
			ASSERT(CloseHandle(ret));	//释放内核资源
		else
		{
			ASSERT(ret != INVALID_HANDLE_VALUE);
		}

		//错误判断
		if (ret == INVALID_HANDLE_VALUE)
		{
			IF_NOT_NL(i != 0)
				return false;
			else
				break;
		}

		m_nCurNum++;	//当前线程数
	}

	return true;
}

//添加任务
bool CThreadPool::AddTask(CTaskBase &rTask, int iThreadPerNum /*= 1*/, const bool bCreateNewThread/*=false*/)
{
	IF_NOT_NL(m_pCSynLockerObj)
		return false;

	IF_NOT_NL(iThreadPerNum>0)
		return false;

	bool bDestroy = false;
	IF_NOT_NL (CheckTask(bDestroy, iThreadPerNum) == RES_SUCCESS)
		return false;

	if (m_bIsStopAdd)
	{
		return false;
	}

	//添加任务
	if (!bDestroy)
	{
		IF_NOT_NL(m_pCSynLockerObj->Lock())
			return false;//---临界区

		//添加任务
		if (m_plistTasklist->PushBack(&rTask))
		{
			m_nCacheUsed += rTask.GetCacheSize();
		}

		if (m_nWaitNum > 0&&!bCreateNewThread)
		{
			//唤醒线程			
			m_pCSynLockerObj->SinalOne();
			ASSERT(m_pCSynLockerObj->UnLock());
		}
		else
		{
			//创建线程
			if (m_nCurNum+ iThreadPerNum <= m_nMaxNum)
			{
				IF_NOT_NL(CreateThread(((iThreadPerNum > (m_nMaxNum - m_nCurNum)) ? (m_nMaxNum - m_nCurNum) : iThreadPerNum)))//创建线程
				{
					ASSERT(m_pCSynLockerObj->UnLock());
					return false;
				}
			}
			ASSERT(m_pCSynLockerObj->UnLock());
		}
	}
	else
	{
		return false;
	}

	return true;
}

//线程处理函数
unsigned __stdcall CThreadPool::ThreadHandler(void* pThreadData)
{
	//mfc无线程分离
	CThreadPool *pPool = (CThreadPool *)(pThreadData);

	//参数校验
	IF_NOT_NL(pPool)
	{
		//中止线程
		_endthreadex(0L);
		return 0L;
	}

	try
	{
		auto pLock = pPool->GetSynLockerObj();
		auto pList = pPool->GetTaskList();

		//线程启动处理
		while (!pPool->m_bIsDestroy)
		{
			ASSERT(pLock->Lock());

			if (pList->IsEmpty() != true)//判断任务是否为空
			{
				//任务执行
				{
					gtool::CSmartPtr<CTaskBase> pTask = std::move(gtool::CSmartPtr<CTaskBase>(pList->PopFront()));//取任务

					ASSERT(pLock->UnLock());

					try
					{						
						pTask->Run();	//执行任务
					}
					catch (...)
					{
						ASSERT(!"TASK CRASH");
					}
				}
			}
			else
			{
				pPool->m_nWaitNum++;
				auto nWaitRet = pLock->WaitEvent();
				ASSERT(nWaitRet == WIN_WAIT_EVENT_SUC);
				pPool->m_nWaitNum--;
				ASSERT(pLock->UnLock());
			}
		}

		pPool->m_nCurNum--;
		auto bNoneThread = pPool->m_nCurNum <= 0;
		if (bNoneThread)
		{
			ASSERT(pLock->Lock());
			pLock->SinalOne();
			ASSERT(pLock->UnLock());
		}		
	}
	catch (...)
	{
		ASSERT(!"CRASH THREAD");
	}

	//中止线程
	_endthreadex(0L);
	return 0L;
}

void CThreadPool::Destroy()
{
	ClearPool();
}

int CThreadPool::StopAdd()
{
	m_bIsStopAdd = true;
	return RES_SUCCESS;
}

int CThreadPool::EnAbleAdd()
{
	m_bIsStopAdd = false;
	return RES_SUCCESS;
}

void CThreadPool::DeductCacheByte(U64 u64Byte)
{
	m_nCacheUsed -= u64Byte;
}

void CThreadPool::AddCacheByte(U64 u64Byte)
{
	m_nCacheUsed += u64Byte;
}

const U64 CThreadPool::GetCurCacheUsed()
{
	auto nCurCache = m_nCacheUsed;

	return nCurCache;
}

const U64 CThreadPool::GetCacheTopLimit()
{
	auto nTopCache = m_nCacheTopLimit;

	return nTopCache;
}

void CThreadPool::SetCacheTopLimit(U64 u64Byte)
{
	m_nCacheTopLimit = u64Byte;
}

void CThreadPool::SetEnableCache(bool bEnable /*= false*/)
{
	m_bEnableCache = bEnable;
}

bool CThreadPool::GetEnableCache()
{
	auto bUsed = m_bEnableCache;

	return bUsed;
}

U16 CThreadPool::GetRePauseCachePercentage()
{
	auto nVal = m_nRePauseCachePercentage;

	return nVal;
}

int CThreadPool::CheckTask(bool &bDestroy, int &iThreadPerNum)
{
	if (!m_pCSynLockerObj)
	{
		ASSERT(m_pCSynLockerObj);
		return RES_FAILED;
	}

	//参数校验
	bDestroy = m_bIsDestroy;
	if (m_bIsStopAdd == true)
	{
		return RES_FAILED;
	}

	if (iThreadPerNum+m_nCurNum > m_nMaxNum) iThreadPerNum = m_nMaxNum- m_nCurNum;

	return RES_SUCCESS;
}

CThreadPool::TASK_LIST* CThreadPool::GetTaskList() const
{
	return m_plistTasklist;
}

bool CThreadPool::Lock()
{
	if (!m_pCSynLockerObj)
	{
		ASSERT(!m_pCSynLockerObj);
		return false;
	}
	return  m_pCSynLockerObj->Lock();
}

bool CThreadPool::UnLock()
{
	if (!m_pCSynLockerObj)
	{
		ASSERT(!m_pCSynLockerObj);
		return false;
	}
	return  m_pCSynLockerObj->UnLock();
}


#endif



