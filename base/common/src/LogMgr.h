#ifndef LOG_MGR_H_
#define LOG_MGR_H_
#pragma once

#include <BaseCode/MyBaseMacro.h>
#include <BaseCode/LitPtrMap.hpp>
#include <BaseCode/SmartPtr.hpp>
#include <BaseCode/TPtrList.hpp>
#include <BaseCode/FmtIOString.h>
#include <boost/timer.hpp>

class CFile2;
class CThreadPool;
class CDataLocker;

struct LogDataInfo
{
	std::string strFullLogName;			//文件名
	gtool::CFmtIOString strData;		//数据
};

typedef gtool::CLitPtrMap<CDataLocker, std::string> FileNameKeyLockerMap;	//key:filename value: locker
typedef gtool::CLitPtrMap<CFile2, std::string> StrKeyFilePtrValLitMap;		//key:filename value: FilePtr
typedef gtool::TPtrList<LogDataInfo, DEL_WAY_DELETE> StringList;

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
	void YieldSchedule();
	bool AddTask(gtool::CFmtIOString& strData, const char *& strFileName);
	bool IsInit();

public:
	StrKeyFilePtrValLitMap* GetFilePtrMap();
	FileNameKeyLockerMap* GetFileLockerMap();
	StringList *GetLogDataList();

	//api接口
public:
	void SetTimerFlush(int nTimeDiff);																//设置刷新时间（落磁盘时间）
	void FlushLog();																				//落磁盘(线程池来做)
	bool AddLog(const char *szLogPath, const char *szFileName, FILE* fp);										//日志文件初始化

	template<typename... Aargs> 	//组装成字符串并交由线程池来写文件（不立即落磁盘）
	bool WriteLog(bool bNoFmt, const char *szLogFullName, const char *fmt, const Aargs&... rest)
	{
		gtool::CFmtIOString strData;
		if (bNoFmt)
		{
			strData.MulArgInput(rest);
		}
		else
		{
			strData.FormatEx(fmt, rest);
		}
		CHECKF_NL(AddTask(strData, szLogFullName));

		return true;
	}

private:
	gtool::CSmartPtr<FileNameKeyLockerMap>	m_pFileNameKeyLockerMap;			//文件名对应的锁map
	gtool::CSmartPtr<StrKeyFilePtrValLitMap> m_pFilePtrValStrKeyMap;		//文件名和文件指针map
	gtool::CSmartPtr<CDataLocker>			m_pDataSafeGuard;				//数据保护
	gtool::CSmartPtr<CThreadPool>			m_pThreadPool;					//线程池	
	gtool::CSmartPtr<StringList>			m_pStringList;					//日志数据
	volatile bool							m_bInit = false;
	volatile int							m_nTimeDiff = TIMER_FLUSH_DIFF_DEF;				//单位s <=0表示手动落磁盘，>0表示定时落磁盘 默认1s落一次磁盘
	boost::timer							m_Timer;
};

typedef gtool::CSingleton<CLogMgr> LogMgrInstance;
#define LogMgr2 (*LogMgrInstance::GetInstance())


#endif
