#include <stdafx.h>
#include"BaseCode/MyLogger.h"
#include "BaseCode/FileDirSys.h"

#if !(defined(linux)|defined(__CYGWIN__))

CDataLocker  g_SuperLogLocker;
CDataLocker  g_SimpleLogLocker;
CDataLocker  g_GenLogLocker;
CDataLocker  g_ProgCrashLogLocker;
CDataLocker  g_ProgErrLogLocker;

#define  LOG_CACHE_DEF_SIZE 128*1024

volatile bool gtool::CLogger::_logger_is_on = true;


gtool::CLogger::CLogger()
{
}

gtool::CLogger::~CLogger()
{
}

gtool::CSafeLogMap::CSafeLogMap()
{
	m_pLogFileMap = StrKeyFilePtrValLitMap::CreateNew();
}

gtool::CSafeLogMap::~CSafeLogMap()
{
}

//设置调试开关
void gtool::CLogger::makeLoggerOn()
{
	_logger_is_on = true;
}

void gtool::CLogger::makeLoggerOff()
{
	_logger_is_on = false;
}

const bool gtool::CLogger::loggerState()
{
	return _logger_is_on;
}

bool gtool::CLogger::FlushLog(const char *szLogFile)
{
	if (!szLogFile)
	{
		ASSERT(!szLogFile);
		return false;
	}

	auto pMap = &(SafeLogFileMap2);
	if (!pMap)
	{
		ASSERT(pMap);
		return false;
	}

	pMap->Lock();
	if (!pMap->m_pLogFileMap)
	{
		ASSERT(pMap->m_pLogFileMap);
		pMap->UnLock();
		return false;
	}

	std::string strLog = szLogFile;
	FileInsertTime(strLog);

	auto IterMap= pMap->m_pLogFileMap->find(strLog);
	if (IterMap != pMap->m_pLogFileMap->end())
	{
		auto pFile2= IterMap->second;
		if (!pFile2)
		{
			ASSERT(pFile2);
			pMap->UnLock();
			return false;
		}

		if (!pFile2->m_fptr)
		{
			ASSERT(pFile2->m_fptr);
			pMap->UnLock();
			return false;
		}

		if (!CFileDirSys::FlushFile(*(pFile2->m_fptr)))
		{
			ASSERT("CFileDirSys::FlushFile fail");
			pMap->UnLock();
			return false;
		}
	}

	pMap->UnLock();

	return true;
}

bool gtool::CLogger::CloseLog(const char *szLogFile)
{

	if (!szLogFile)
	{
		ASSERT(!szLogFile);
		return false;
	}

	auto pMap = &(SafeLogFileMap2);
	if (!pMap)
	{
		ASSERT(pMap);
		return false;
	}

	pMap->Lock();
	if (!pMap->m_pLogFileMap)
	{
		ASSERT(pMap->m_pLogFileMap);
		pMap->UnLock();
		return false;
	}

	std::string strLog = szLogFile;
	auto IterMap = pMap->m_pLogFileMap->find(strLog);
	if (IterMap != pMap->m_pLogFileMap->end())
	{
		auto pFile2 = IterMap->second;
		if (!pFile2)
		{
			ASSERT(pFile2);
			pMap->UnLock();
			return false;
		}

		if (!pFile2->m_fptr)
		{
			ASSERT(pFile2->m_fptr);
			pMap->UnLock();
			return false;
		}

		if (!pMap->m_pLogFileMap->DelObj(strLog))
		{
			ASSERT(!"关闭失败");
			pMap->UnLock();
			return false;
		}
	}

	pMap->UnLock();

	return true;
}

// 
// bool gtool::CLogger::openLogfile(std::fstream &file, std::string strFileName)
// {
// 	file.open((char *)strFileName.c_str(), ios::ate);
// 	if (!file.is_open())
// 	{
// 		file.open((char *)strFileName.c_str(), ios::app);
// 	}
// 
// 	if (!file.is_open()) return false;
// 
// 	return true;
// }

#if  defined(_WIN32) || defined(_WIN64)

void gtool::CLogger::InsertCurrentTime(CFmtIOString& fos)
{
	//获取时间
	SYSTEMTIME tm_now = { 0 };
	GetLocalTime(&tm_now);

	fos << tm_now.wYear;
	tm_now.wMonth < 10 ? (fos << '0' << tm_now.wMonth) : (fos << (tm_now.wMonth));
	tm_now.wDay < 10 ? (fos << '0' << tm_now.wDay) : (fos << (tm_now.wDay));
	fos << " ";
	tm_now.wHour < 10 ? (fos << '0' << tm_now.wHour) : (fos << tm_now.wHour);
	fos << ":";
	tm_now.wMinute < 10 ? (fos << '0' << tm_now.wMinute) : (fos << tm_now.wMinute);
	fos << ":";
	tm_now.wSecond < 10 ? (fos << '0' << tm_now.wSecond) : (fos << tm_now.wSecond);
	fos << ".";
	tm_now.wMilliseconds < 10 ? (fos << "00" << tm_now.wMilliseconds) : (tm_now.wMilliseconds < 100 ? (fos << '0' << tm_now.wMilliseconds) : (fos << tm_now.wMilliseconds));
	fos << " ";
}

//给日志添加日期
void gtool::CLogger::FileInsertTime(std::string &strFileName)
{
	//获取时间
	SYSTEMTIME tm_now = {0};
	GetLocalTime(&tm_now);
	char szDay[16] = { 0 };
	sprintf_s(szDay, sizeof(szDay), "%d-%02d-%02d", tm_now.wYear, tm_now.wMonth, tm_now.wDay);

	//搜索‘.’
	auto nEndPos=strFileName.rfind('.', strFileName.length()-1);
	if (nEndPos == std::string::npos)
	{
		strFileName += szDay;
		strFileName += ".log";
		return;
	}
	strFileName.insert(nEndPos, szDay);
}

#else
void gtool::CLogger::FileInsertTime(std::string &strFileName)
{
	//获取时间
	struct tm *tm_now;
	time_t tnow = time(NULL);
	tm_now = localtime(&tnow);
	char szDay[16] = { 0 };
	sprintf_s(szDay, sizeof(szDay), "%d-%02d-%02d", tm_now->tm_year +1900, tm_now->tm_mon+1, tm_now->tm_mday);

	//搜索‘.’
	auto nEndPos = strFileName.rfind('.', strFileName.length() - 1);
	if (nEndPos == std::string::npos)
	{
		strFileName += szDay;
		strFileName += ".log";
		return;
	}
	strFileName.insert(nEndPos, szDay);
}
void gtool::CLogger::InsertCurrentTime(CFmtIOString& fos)
{
	struct tm *tm_now;
	time_t tnow = time(NULL);
	tm_now = localtime(&tnow);

	fos << tm_now->tm_year + 1900;
	tm_now->tm_mon + 1 < 10 ? (fos << '0' << tm_now->tm_mon + 1) : (fos << (tm_now->tm_mon + 1));
	fos << tm_now->tm_mday << " ";
	tm_now->tm_hour < 10 ? (fos << '0' << tm_now->tm_hour) : (fos << tm_now->tm_hour);
	fos << ":";
	tm_now->tm_min < 10 ? (fos << '0' << tm_now->tm_min) : (fos << tm_now->tm_min);
	fos << ":";
	tm_now->tm_sec < 10 ? (fos << '0' << tm_now->tm_sec) : (fos << tm_now->tm_sec);
	fos << " ";
}

#endif


gtool::CLogger::CLogger(CLogger&&rValue)
{
	_logger_is_on = rValue._logger_is_on;
}

#endif


