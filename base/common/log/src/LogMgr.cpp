


//#include <BaseCode/CheckNoLog.h>
//#include <BaseCode/File2.h>
//#include<BaseCode/ThreadPool.h>
//#include <BaseCode/TaskBase.h>
//#include <BaseCode/HeapMgr.h>
//#include <BaseCode/Singleton.hpp>
//#include <BaseCode/WinLocker.h>

//#include <BaseCode/MyLogger.h>

//#include <BaseCode/FileDirSys.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOG_THREAD_MAX		1					//最大线程数
#define LOG_THREAD_MIN		1					//最小线程数



/////////////////////////////////////////////////////////////////////////////////////////////////////

//刷新日志任务
class CFlushLogTask :public CTaskBase
{
	NO_COPY(CFlushLogTask);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CFlushLogTask);

public:
	//任务执行体
	virtual int Run();

protected:
	bool MigrationLogToCache(CLogMgr& rLogMgr, LogDataListMap *&pMainMap, LogDataListMap *&pCacheMap);
	bool WriteCacheLog(CLogMgr& rLogMgr, LogDataListMap *&pCacheMap);
};

CFlushLogTask::CFlushLogTask()
{
}

CFlushLogTask::~CFlushLogTask()
{
}

int CFlushLogTask::Run()
{
	auto pLogMgr = (CLogMgr *)m_pArg;
	CHECKF_NL(pLogMgr);
	bool bFini = false;
	I32 nTimerDiff = 0;
	LogDataListMap *pMainMap = NULL;
	gtool::CSmartPtr<LogDataListMap> pCacheMap = LogDataListMap::CreateNew();
	CHECKF_NL(pCacheMap);
	LogDataListMap *pCachePoint = pCacheMap;

	DOTRY_B_NL
	{
		pLogMgr->StartFlushTask();
		while (!bFini)
		{
			DOTRY_B_NL
			{
				bFini = pLogMgr->IsFinishLog();
 			nTimerDiff = pLogMgr->GetFlushTimerDiff();

			pLogMgr->LockRes();/////////////////////临界区

			//日志迁移
			ASSERT(MigrationLogToCache(*pLogMgr, pMainMap, pCachePoint));
			ASSERT(pCachePoint);

			ASSERT(pLogMgr->UnLockRes());/////////////////////临界区
			}
			DOCATCH_ANY_NL("CFlushLogTask::Run() CRASH")
			{
				if (pLogMgr->IsLock())
				{
					ASSERT(pLogMgr->UnLockRes());/////////////////////临界区
				}
			}
			DOCATCH_ANY_NL_END

			//写日志
			DOTRY_B_NL
			{
				ASSERT(WriteCacheLog(*pLogMgr, pCachePoint));
				ASSERT(pCachePoint);
			}
			DOCATCH_ANY_NL("CFlushLogTask::Run() 写日志 crash")
			{
			}
			DOCATCH_ANY_NL_END

			ASSERT(pLogMgr->LockRes());
			ASSERT(pLogMgr->TimeWait(nTimerDiff));
			ASSERT(pLogMgr->UnLockRes());
		}

		pLogMgr->FinishFlushTask();
	}
	DOCATCH_ANY_NL("int CFlushLogTask::Run()")
	{
		pLogMgr->FinishFlushTask();
	}
	DOCATCH_ANY_NL_END	

	return 0;
}

bool CFlushLogTask::MigrationLogToCache(CLogMgr& rLogMgr, LogDataListMap *&pMainMap, LogDataListMap *&pCacheMap)
{
	pMainMap = rLogMgr.GetLogDataListMap();
	CHECKF_NL(pMainMap);
	CHECKF_NL(pCacheMap);

	bool bEraseNode = false;
	const auto nNodeNum = pMainMap->size();
	for (auto IterMap = pMainMap->begin(); IterMap != pMainMap->end(); (bEraseNode ? IterMap : (++IterMap)))
	{
		bEraseNode = false;
		IF_OK_NL(!pCacheMap->IsExist(IterMap->first))
		{
			pCacheMap->insert(std::make_pair(IterMap->first, IterMap->second));
			IF_OK_NL(pCacheMap->IsExist(IterMap->first))
			{
				IterMap = pMainMap->PopObj(IterMap, true);
				bEraseNode = true;
			}
		}
	}

	ASSERT(pCacheMap->size() == nNodeNum);
	const auto nRestNum = pMainMap->size();
	ASSERT(nRestNum == 0);
	if (nRestNum > 0)
		pMainMap->Cleans();

	return true;
}

bool CFlushLogTask::WriteCacheLog(CLogMgr& rLogMgr, LogDataListMap *&pCacheMap)
{
	CHECKF_NL(pCacheMap);
	if (pCacheMap->size() <= 0)
		return true;

	I32 nFileIndex = 0;
	LogDataList *pLogList = NULL;
	CLogFile *pLogFile = NULL;
	for (auto IterMap = pCacheMap->begin(); IterMap != pCacheMap->end(); ++IterMap)
	{
		nFileIndex = IterMap->first;
		pLogList = IterMap->second;
		IF_NOT_NL(pLogList)
			continue;

		ASSERT(rLogMgr.LockRes());
		pLogFile = rLogMgr.QueryFilePtr(nFileIndex);
		ASSERT(rLogMgr.UnLockRes());

		IF_NOT_NL(pLogFile)
			continue;

		pLogFile->Lock();/////////////////////临界区

		//校验打开文件
		IF_NOT_NL(pLogFile->IsOpen()){

			IF_NOT_NL(pLogFile->Reopen()){
				ASSERT(pLogFile->UnLock());
				continue;
			}

			//刷新时间戳
			pLogFile->UpdateLastTimestamp();
		}

		//写文件
		auto IterList = pLogList->NewEnum();
		while (IterList.Next())
		{
			gtool::CFmtIOString *pIOString = IterList;
			IF_NOT_NL(pIOString)
				continue;

			//跨天
			if (pLogFile->IsDayPass(pIOString->GetLastModifyTimestamp()))
			{
				//跨天需要重新打开文件
				ASSERT(pLogFile->Reopen());
				IF_NOT_NL(pLogFile->IsOpen())
					break;

				pLogFile->UpdateLastTimestamp();
			}

// 			//文件过大
// 			if (pLogFile->IsTooLarge())
// 			{
// 				ASSERT(pLogFile->Reopen());
// 				IF_NOT_NL(pLogFile->IsOpen())
// 					break;
// 
// 				pLogFile->UpdateLastTimestamp();
// 			}

			auto nWrLen = pLogFile->Write(pIOString->c_str(), pIOString->GetLength());
			ASSERT(nWrLen == pIOString->GetLength());
			ASSERT(pLogFile->Flush());
		}

		pLogFile->UnLock();/////////////////////临界区
	}

	pCacheMap->Cleans();

	return true;
}

void CFlushLogTask::Cleans()
{
}


////////////////////////////////////////////////////////////////////////////////////////
boost::atomic_bool	CLogMgr::m_bFiniDone{ false };
CLogMgr * g_pLogMgr = LogMgrInstance::GetInstance();
CLogMgr::CLogMgr()
{
	m_pThreadPool = CThreadPool::CreateNew();
	m_pFilePtrMap = FilePtrMap::CreateNew();
	m_pLogDataListMap = LogDataListMap::CreateNew();
	m_pDataSafeGuard = CDataLocker::CreateNew(true);	
}

CLogMgr::~CLogMgr()
{
	Fini();
}

bool CLogMgr::Init()
{
	CHECKF_NL(m_pFilePtrMap);
	CHECKF_NL(m_pLogDataListMap);
	CHECKF_NL(m_pDataSafeGuard);
	CHECKF_NL(m_pThreadPool);
	CHECKF_NL(!m_bInit);

	//初始化线程
	m_pThreadPool->SetThreadNumLimit(LOG_THREAD_MIN, LOG_THREAD_MAX);

	m_bInit = true;
	m_bFiniDone = false;

	return true;
}

bool CLogMgr::Fini()
{
	DOTRY_B_NL
	{
		CHECKF_NL(m_pDataSafeGuard);
		CHECKF_NL(m_pLogDataListMap);

		if (m_bFiniDone)
			return true;

		m_bFiniDone = true;
		m_bInit = false;

		//唤醒写完日志		
		bool bEmpty = false;
		while (true)
		{
			//唤醒
			ASSERT(LockRes());
			m_pDataSafeGuard->SinalOne();
			ASSERT(UnLockRes());
			Sleep(TIMER_FLUSH_DIFF_DEF);
			ASSERT(LockRes());
			bEmpty = m_pLogDataListMap->empty();
			ASSERT(UnLockRes());

			if(bEmpty)
				break;
		}

		//退出任务
		m_bFiniLog = true;

		I32 nLoopCount = 0;
		const I32 nMaxLoop = 100000;
		while (m_bFlushStarted)
		{
			DEAD_LOOP_ASSERT(nLoopCount, nMaxLoop);
			Sleep(10);
			ASSERT(m_pDataSafeGuard->Broadcast());
		}
		

// 		while (true)
// 		{
// 			//唤醒
// 			ASSERT(LockRes());
// 			m_pDataSafeGuard->SinalOne();
// 			m_pDataSafeGuard->WaitEvent(TIMER_FLUSH_DIFF_DEF);
// 			ASSERT(UnLockRes());
// 
// 			if (!m_bFlushStarted)
// 				break;
// 		}
		

		//先结束任务池
		IF_OK_NL(m_pThreadPool)
			m_pThreadPool->Destroy();

		ASSERT(LockRes());
	
		IF_OK_NL(m_pLogDataListMap)
		{
			ASSERT(m_pLogDataListMap->empty());
			m_pLogDataListMap->Cleans();
		}

		IF_OK_NL(m_pFilePtrMap)
			m_pFilePtrMap->Cleans();

		//任务池泄露检查
// 		CWriteLogTask::PrintMemLeak();
// 		CFlushLogTask::PrintMemLeak();

		ASSERT(UnLockRes());
	}
	DOCATCH_ANY_NL("CLogMgr::Fini()")
	{
		UnLockRes();
		return false;
	}
	DOCATCH_ANY_NL_END

	return true;
}

bool CLogMgr::LockRes()
{
	IF_NOT_NL(m_pDataSafeGuard)
	{
		//YieldSchedule();
		if(!m_pDataSafeGuard)
			m_pDataSafeGuard = CDataLocker::CreateNew();
	}
	CHECKF_NL(m_pDataSafeGuard);

	return m_pDataSafeGuard->Lock();
}

bool CLogMgr::UnLockRes()
{
	IF_NOT_NL(m_pDataSafeGuard)
	{
		//YieldSchedule();
		if (!m_pDataSafeGuard)
			m_pDataSafeGuard = CDataLocker::CreateNew();
	}
	CHECKF_NL(m_pDataSafeGuard);

	return m_pDataSafeGuard->UnLock();
}

bool CLogMgr::IsLock()
{
	IF_NOT_NL(m_pDataSafeGuard)
	{
		//YieldSchedule();
		if (!m_pDataSafeGuard)
			m_pDataSafeGuard = CDataLocker::CreateNew();
	}
	CHECKF_NL(m_pDataSafeGuard);

	return m_pDataSafeGuard->Islock();
}

bool CLogMgr::TimeWait(MYULONG ulMilliseconds/*=TIMER_FLUSH_DIFF_DEF*/)
{
	CHECKF_NL(m_pDataSafeGuard);
	return m_pDataSafeGuard->WaitEvent(ulMilliseconds) != WIN_WAIT_EVENT_FAILED;
}

void CLogMgr::YieldSchedule()
{
	std::this_thread::yield();
}
bool CLogMgr::AddLogTask(const I32 nFileIndex, gtool::CFmtIOString* pData, const time_t tCurData, bool & bNeedDataRelease)
{
	bNeedDataRelease = false;
	CHECKF_NL(pData);
	CHECKF_NL(nFileIndex >= 0);
	CHECKF_NL(m_pLogDataListMap);
	CHECKF_NL(m_pFilePtrMap);
	ASSERT(LockRes());
	IF_NOT_NL(m_pFilePtrMap->IsExist(nFileIndex))
	{
		ASSERT(UnLockRes());
		return false;
	}

	auto pList = m_pLogDataListMap->GetObj(nFileIndex);
	if (!pList)
	{
		gtool::CSmartPtr<LogDataList> pList2 = LogDataList::CreateNew();
		IF_NOT_NL(pList2)
		{
			ASSERT(UnLockRes());
			return false;
		}
		m_pLogDataListMap->insert(std::make_pair(nFileIndex, pList=pList2.pop()));
		IF_NOT_NL(m_pLogDataListMap->IsExist(nFileIndex))
		{
			SAFE_RELEASE_NL(pList);
			ASSERT(UnLockRes());
			return false;
		}
	}
	IF_NOT_NL(pList)
	{
		ASSERT(UnLockRes());
		return false;
	}

	pData->UpdateLastModifyTimestamp(tCurData);

	IF_NOT_NL(pList->PushBack(pData))
	{
		bNeedDataRelease = true;
		ASSERT(UnLockRes());
		return false;
	}

	ASSERT(UnLockRes());

	return true;
}

bool CLogMgr::IsInit()
{
	return m_bInit;
}

LogDataListMap* CLogMgr::GetLogDataListMap()
{
	return m_pLogDataListMap;
}

CLogFile* CLogMgr::QueryFilePtr(const I32 nFileIndex)
{
	IF_NOT_NL(m_pFilePtrMap)
		return NULL;

	return m_pFilePtrMap->GetObj(nFileIndex);
}

bool CLogMgr::IsFinishLog() const
{
	return m_bFiniLog;
}

bool CLogMgr::IsFiniDone() const
{
	return m_bFiniDone;
}

void CLogMgr::SetTimerFlush(int nTimeDiff)
{
	IF_NOT_NL(m_bInit)
	{
		return;
	}

	m_nTimeDiff = nTimeDiff;
}

void CLogMgr::FlushLog()
{
	CHECK_NL(m_pDataSafeGuard);
	m_pDataSafeGuard->SinalOne();
}

//初始化时候调用，其他情况请谨慎使用
bool CLogMgr::AddLog(const I32 nFileIndex, const char *szLogPath, const char *szFileName)
{
	CHECKF_NL(szFileName&&szLogPath);

	IF_NOT_NL(m_bInit)
		return false;

	CHECKF_NL(LockRes());

	//创建文件夹
	gtool::CFileDirSys::CreateRecursiveDir(std::string(szLogPath));

	std::string strLog = szLogPath;
	strLog += szFileName;

	//组合完整的log名称
	auto pLogFile = m_pFilePtrMap->GetObj(nFileIndex);
	if (!pLogFile)
	{
		gtool::CSmartPtr<CLogFile> pLogFile2 = CLogFile::CreateNew();
		IF_NOT_NL(pLogFile2)
		{
			UnLockRes();
			return false;
		}
			

		DOTRY_B_NL
		{
			IF_NOT_NL(pLogFile2->Open(strLog.c_str(), true, "ab+", true))
			{
				UnLockRes();
				return false;
			}
			pLogFile2->UpdateLastTimestamp();
		}
		DOCATCH_ANY_NL("AddLog::OpenFile failed!")
		{
		}
		DOCATCH_ANY_NL_END

		CLogFile *pLogFile3 = NULL;
		m_pFilePtrMap->insert(std::make_pair(nFileIndex, pLogFile3 = pLogFile2.pop()));

		if (m_pFilePtrMap->find(nFileIndex) == m_pFilePtrMap->end())
		{
			SAFE_RELEASE_NL(pLogFile3);
			ASSERT(UnLockRes());
			ASSERT(!"添加日志失败");
			return false;
		}
		pLogFile = pLogFile3;
	}	

	IF_NOT_NL(pLogFile)
	{
		UnLockRes();
		return false;
	}

	if (!m_bFlushStarted)
	{
		m_bFlushStarted = true;
		gtool::CSmartPtr<CFlushLogTask> pFlushTask = CFlushLogTask::CreateNew();
		IF_NOT_NL(pFlushTask)
		{
			ASSERT(UnLockRes());
			return true;
		}

		pFlushTask->SetArg(this);
		CTaskBase *pTaskBase = pFlushTask.pop();
		IF_NOT_NL(m_pThreadPool->AddTask(*pTaskBase))
		{
			SAFE_RELEASE_NL(pTaskBase);
			ASSERT(UnLockRes());
			return true;
		}
	}

	ASSERT(UnLockRes());

	return true;
}

