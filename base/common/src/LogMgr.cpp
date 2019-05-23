#include <BaseCode/LogMgr.h>
#include <BaseCode/CheckNoLog.h>
#include <BaseCode/File2.h>
#include<BaseCode/ThreadPool.h>
#include <BaseCode/TaskBase.h>
#include <BaseCode/HeapMgr.h>
#include <BaseCode/Singleton.hpp>
#include <BaseCode/WinLocker.h>
#include<thread>
#include <BaseCode/MyLogger.h>

#include <string>
#include <BaseCode/FileDirSys.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOG_THREAD_MAX		10					//最大线程数
#define LOG_THREAD_MIN		4					//最小线程数



/////////////////////////////////////////////////////////////////////////////////////////////////////
//写日志任务
class CWriteLogTask :public CTaskBase
{
	HEAP_MEM_MGR(s_Heap);
	NO_COPY(CWriteLogTask);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CWriteLogTask);

public:
	//任务执行体
	virtual int Run();

public:
	gtool::CFmtIOString m_strData;
	std::string m_strFileName;
};

HEAPMEMMGR_IMPLEMENTATION(CWriteLogTask, s_Heap)

void CWriteLogTask::Cleans()
{
	SetArg(NULL);
}

int CWriteLogTask::Run()
{
	//打印日志

	//查找file指针
	FILE *fp = NULL;
	LogMgr2.LockRes();

	//文件锁
	auto pLockerMap = LogMgr2.GetFileLockerMap();
	IF_NOT_NL(pLockerMap)
	{
		LogMgr2.UnLockRes();
		return -1;
	}

	auto IterLockerMap = pLockerMap->find(m_strFileName);
	IF_NOT_NL(IterLockerMap != pLockerMap->end())
	{
		gtool::CSmartPtr<CDataLocker> pLocker = CDataLocker::CreateNew();
		IF_NOT_NL(pLocker)
		{
			LogMgr2.UnLockRes();
			return -1;
		}

		pLockerMap->insert(std::make_pair(m_strFileName, pLocker));

		IF_NOT_NL((IterLockerMap = pLockerMap->find(m_strFileName)) != pLockerMap->end())
		{
			LogMgr2.UnLockRes();
			return -1;
		}
		pLocker.pop();
	}
	IF_NOT_NL(IterLockerMap->second)
	{
		LogMgr2.UnLockRes();
		return -1;
	}

	auto pLogFileLocker = IterLockerMap->second;

	//文件指针
	auto pMap = LogMgr2.GetFilePtrMap();
	IF_NOT_NL(pMap)
	{
		LogMgr2.UnLockRes();
		return -1;
	}
	auto IterMap = pMap->find(m_strFileName);
	IF_NOT_NL(IterMap != pMap->end())
	{
		gtool::CSmartPtr<CFile2> pFile2 = CFile2::CreateNew();
		IF_NOT_NL(pFile2)
		{
			LogMgr2.UnLockRes();
			return -1;
		}

		pMap->insert(std::make_pair(m_strFileName, pFile2));
		IF_NOT_NL( (IterMap=pMap->find(m_strFileName)) != pMap->end())
		{
			LogMgr2.UnLockRes();
			return -1;
		}
		pFile2.pop();
	}
	IF_NOT_NL(IterMap->second)
	{
		LogMgr2.UnLockRes();
		return -1;
	}

	LogMgr2.UnLockRes();


	//写文件
	pLogFileLocker->Lock();

	fp = IterMap->second->m_fptr;
	IF_NOT_NL (fp)
	{
		IterMap->second->m_fptr = CFileDirSys::OpenFile(m_strFileName, true, "ab+");
		fp = IterMap->second->m_fptr;
		IF_NOT_NL(fp)
		{
			pLogFileLocker->UnLock();
			return -1;
		}
	}
	IF_NOT_NL(fp)
	{
		pLogFileLocker->UnLock();
		return -1;
	}

	ASSERT(CFileDirSys::WriteFile(*fp, m_strData.c_str(), m_strData.GetLength())>0);

	pLogFileLocker->UnLock();

	return 0;
}

//刷新日志任务
class CFlushLogTask :public CTaskBase
{
	HEAP_MEM_MGR(s_Heap);
	NO_COPY(CFlushLogTask);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CFlushLogTask);

public:
	//任务执行体
	virtual int Run();
};

int CFlushLogTask::Run()
{
	while (true)
	{
		LogMgr2.LockRes();
		LogMgr2.UnLockRes();
		Sleep(1000);
	}

}


////////////////////////////////////////////////////////////////////////////////////////

CLogMgr::CLogMgr()
{
	m_pThreadPool = CThreadPool::CreateNew(LOG_THREAD_MIN, LOG_THREAD_MAX);
	m_pFileNameKeyLockerMap = FileNameKeyLockerMap::CreateNew();
	m_pFilePtrValStrKeyMap = StrKeyFilePtrValLitMap::CreateNew();
	m_pDataSafeGuard = CDataLocker::CreateNew();	
	m_pStringList = StringList::CreateNew();
}

CLogMgr::~CLogMgr()
{

}

bool CLogMgr::Init()
{
	CHECKF_NL(m_pFileNameKeyLockerMap);
	CHECKF_NL(m_pFilePtrValStrKeyMap);
	CHECKF_NL(m_pDataSafeGuard);
	CHECKF_NL(m_pThreadPool);
	CHECKF_NL(m_pStringList);

	//初始化线程
	//初始化路径文件
	//

	m_bInit = true;
}

bool CLogMgr::Fini()
{
	DOTRY_B_NL
	{
		LockRes();
		m_bInit = false;

		//先结束任务池
		IF_OK_NL(m_pThreadPool)
		{
			m_pThreadPool->Destroy();
		}

		IF_OK_NL(m_pFileNameKeyLockerMap)
		{
			m_pFileNameKeyLockerMap->Cleans();
		}

		IF_OK_NL(m_pFilePtrValStrKeyMap)
		{
			m_pFilePtrValStrKeyMap->Cleans();
		}

		IF_OK_NL(m_pStringList)
		{
			ASSERT(m_pStringList->IsEmpty());
			m_pStringList->Clear();
		}

		//任务池泄露检查
		CWriteLogTask::PrintMemLeak();


		UnLockRes();
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
		YieldSchedule();
		if(!m_pDataSafeGuard)
			m_pDataSafeGuard = CDataLocker::CreateNew();
	}
	CHECKF_NL(m_pDataSafeGuard);

	return m_pDataSafeGuard->Lock()== RES_SUCCESS;
}

bool CLogMgr::UnLockRes()
{
	IF_NOT_NL(m_pDataSafeGuard)
	{
		YieldSchedule();
		if (!m_pDataSafeGuard)
			m_pDataSafeGuard = CDataLocker::CreateNew();
	}
	CHECKF_NL(m_pDataSafeGuard);

	return m_pDataSafeGuard->UnLock() == RES_SUCCESS;
}

void CLogMgr::YieldSchedule()
{
	std::this_thread::yield();
}

bool CLogMgr::AddTask(gtool::CFmtIOString& strData, const char *& strFileName)
{
	CHECKF_NL(m_bInit);
	CHECKF_NL(strFileName);
	CHECKF_NL(m_pThreadPool);

	gtool::CSmartPtr<CWriteLogTask> pWrTask = CWriteLogTask::CreateNew();
	CHECKF_NL(pWrTask);
	pWrTask->m_strData = strData.c_str();
	pWrTask->m_strFileName = strFileName;
	CTaskBase *pWrTaskBase = pWrTask.pop();

	LockRes();
	IF_NOT_NL(m_pThreadPool->AddTask(*pWrTaskBase) == RES_SUCCESS) //有可能发生双重release，但概率很小
	{
		pWrTaskBase->Release();
	}
	UnLockRes();
	return true;
}

bool CLogMgr::IsInit()
{
	ASSERT(LockRes());
	auto bInit = m_bInit;
	ASSERT(UnLockRes());

	return bInit;
}

StrKeyFilePtrValLitMap* CLogMgr::GetFilePtrMap()
{
	return m_pFilePtrValStrKeyMap;
}

FileNameKeyLockerMap* CLogMgr::GetFileLockerMap()
{
	return m_pFileNameKeyLockerMap;
}

StringList * CLogMgr::GetLogDataList()
{
	return m_pStringList;
}

void CLogMgr::SetTimerFlush(int nTimeDiff)
{
	ASSERT(LockRes());
	IF_NOT_NL(m_bInit)
	{
		UnLockRes();
		return;
	}

	m_nTimeDiff = nTimeDiff;

	ASSERT(UnLockRes());
}

void CLogMgr::FlushLog()
{
	ASSERT(LockRes());
	IF_NOT_NL(m_bInit)
	{
		UnLockRes();
		return;
	}

	//添加刷新日志任务
	ASSERT(UnLockRes());
}

bool CLogMgr::AddLog(const char *szLogPath, const char *szFileName, FILE* fp)
{
	CHECK_NL(fp&&szFileName&&szLogPath);
	ASSERT(LockRes());
	IF_NOT_NL(m_bInit)
	{
		UnLockRes();
		return false;
	}	

	std::string strLog = szLogPath;
	strLog += szFileName;

	//组合完整的log名称
	auto IterMap=m_pFilePtrValStrKeyMap->find(strLog);
	if (IterMap == m_pFilePtrValStrKeyMap->end())
	{
		gtool::CSmartPtr<CFile2> pFile2 = CFile2::CreateNew();
		CHECK_NL(pFile2);
		pFile2->m_fptr = fp;

		CFile2 *pFile3 = NULL;
		m_pFilePtrValStrKeyMap->insert(std::make_pair(strLog, pFile3=pFile2.pop()));

		if (m_pFilePtrValStrKeyMap->find(strLog) == m_pFilePtrValStrKeyMap->end())
		{
			SAFE_RELEASE_NL(pFile3);
			ASSERT(UnLockRes());
			ASSERT(!"添加日志失败");
			return false;
		}
	}	

	ASSERT(UnLockRes());

	return true;
}

