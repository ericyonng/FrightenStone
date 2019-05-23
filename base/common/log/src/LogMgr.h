#ifndef LOG_MGR_H_
#define LOG_MGR_H_
#pragma once
// #include <BaseCode/MyBaseMacro.h>
// #include <BaseCode/LitPtrMap.hpp>
// #include <BaseCode/SmartPtr.hpp>
// #include <BaseCode/TPtrList.hpp>

//#include <BaseCode/WinLocker.h>

class CFile2;
class CThreadPool;
class CLogFile;
//class CDataLocker;


typedef gtool::CLitPtrMap<CLogFile, I32> FilePtrMap;		//key:fileindex value: FilePtr
typedef gtool::TPtrList<gtool::CFmtIOString, DEL_WAY_RELEASE> LogDataList;
typedef gtool::CLitPtrMap<LogDataList, I32> LogDataListMap;	//key:fileindex value:LogDataList


#define TIMER_FLUSH_DIFF_DEF		1			//默认每1s落磁盘一次

//使用一条线程的异步方式
//一个文件名对应一把锁
//写文件用文件锁，使用map等公共资源用另一把锁
//LogMgr需要全局唯一，使用单例 多线程日志

class CLogMgr
{
	NO_COPY(CLogMgr);
public:
	CLogMgr();
	virtual ~CLogMgr();

	bool Init();
	bool Fini();
	bool LockRes();
	bool UnLockRes();
	bool IsLock();
	bool TimeWait(MYULONG ulMilliseconds=TIMER_FLUSH_DIFF_DEF);
	void YieldSchedule();
	bool AddLogTask(const I32 nFileIndex, gtool::CFmtIOString* pData,const time_t tCurData, bool & bNeedDataRelease);
	bool IsInit();
	void FinishFlushTask() { m_bFlushStarted = false; }
	void StartFlushTask() { m_bFlushStarted = true; }

public:
	LogDataListMap * GetLogDataListMap();
	CLogFile* QueryFilePtr(const I32 nFileIndex);
	bool IsFinishLog() const;
	int GetFlushTimerDiff() { return m_nTimeDiff; }
	bool IsFiniDone()const;

	//api接口
public:
	//设置刷新时间（落磁盘时间） 线程安全
	void SetTimerFlush(int nTimeDiff);		

	//落磁盘(线程池来做) 线程安全
	void FlushLog();	

	//日志文件初始化 线程安全 初始化时候调用，其他情况请谨慎使用
	bool AddLog(const I32 nFileIndex, const char *szLogPath, const char *szFileName);										

private:
	gtool::CSmartPtr<FilePtrMap>			m_pFilePtrMap;				//文件名和文件指针map
	gtool::CSmartPtr<LogDataListMap>		m_pLogDataListMap;
	gtool::CSmartPtr<CDataLocker>			m_pDataSafeGuard;				//数据保护
	gtool::CSmartPtr<CThreadPool>			m_pThreadPool;					//线程池	
	boost::atomic_bool						m_bInit{ false };
	boost::atomic<I32>						m_nTimeDiff{ TIMER_FLUSH_DIFF_DEF };				//单位s <=0表示手动落磁盘，>0表示定时落磁盘 默认1s落一次磁盘
	boost::atomic_bool						m_bFiniLog{ false };
	static boost::atomic_bool				m_bFiniDone ;
	boost::atomic_bool						m_bFlushStarted{ false };
};

typedef gtool::CSingleton<CLogMgr> LogMgrInstance;

extern CLogMgr * g_pLogMgr;
#define LogMgr2 (ASSERT(g_pLogMgr), *g_pLogMgr)


#endif
