#ifndef G_TOOL_H_
#define G_TOOL_H_

#pragma once

//#include <boost/timer.hpp>
//#include <string>
//#include <BaseCode/FmtIOString.h>
//#include <BaseCode/FileDirSys.h>

NAMESPACE_BEGIN(gtool)
class CFmtIOString;

template<typename T>
static inline T Max(const T& t1, const T& t2)
{
	if (t1 > t2)
		return t1;

	return t2;
}

template<typename T>
static inline T Min(const T& t1, const T& t2)
{
	if (t1 > t2)
		return t2;

	return t1;
}

static inline time_t FileInsertTime(std::string &strFileName, time_t tCurTimestamp = 0) //给日志添加日期
{
	//获取时间
	struct tm *tm_now;

	if (tCurTimestamp == 0)
		tCurTimestamp = time(NULL);

	tm_now = localtime(&tCurTimestamp);

	char szDay[16] = { 0 };
	sprintf_s(szDay, sizeof(szDay), "%04d-%02d-%02d", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday);

	//搜索‘.’
	auto nEndPos = strFileName.rfind('.', strFileName.length() - 1);
	if (nEndPos == std::string::npos)
	{
		strFileName += szDay;
		strFileName += ".log";
		return tCurTimestamp;
	}
	strFileName.insert(nEndPos, szDay);

	if (strFileName.find(szDay, 0) == std::string::npos)
		return -1;

	return tCurTimestamp;
}


static inline bool FileInsertTail(std::string &strFileName, const char * strTail) //给日志添加尾部字符
{

	//搜索‘.’
	auto nEndPos = strFileName.rfind('.', strFileName.length() - 1);
	if (nEndPos == std::string::npos)
	{
		strFileName += strTail;
		strFileName += ".log";
		return true;
	}
	strFileName.insert(nEndPos, strTail);

	if (strFileName.find(strTail, 0) == std::string::npos)
		return false;

	return true;
}

static inline bool IsWindowsSys()
{
	return OS_TYPE & OS_TYPE_MICRO_WINDOWS;
}

static inline bool IsLinuxSys()
{
	return OS_TYPE & OS_TYPE_LINUX;
}

static inline I64 GetYYYYMMDDHHMMSS(time_t tTimestamp = 0)
{
	if (tTimestamp == 0)
		tTimestamp = time(NULL);

	struct tm *tm_now;
	tm_now = localtime(&tTimestamp);

	I64 i64YYYYMMDDHHMMSS = (tm_now->tm_year + 1900) * 10000000000 \
		+ (tm_now->tm_mon + 1) * 100000000 + tm_now->tm_mday * 1000000 \
		+ tm_now->tm_hour * 10000 + tm_now->tm_min * 100 + tm_now->tm_sec;

	return i64YYYYMMDDHHMMSS;
}

static inline time_t GetTimestampFromYYYYMMDDHHMMSS(const I64 i64YYYYMMDDHHMMSS)
{
	tm	tmMkTime;
	memset(&tmMkTime, 0, sizeof(tm));
	tmMkTime.tm_year = I32(i64YYYYMMDDHHMMSS / 10000000000 - 1900);
	tmMkTime.tm_mon = I32((i64YYYYMMDDHHMMSS / 100000000) % 100 - 1);
	tmMkTime.tm_mday = I32((i64YYYYMMDDHHMMSS / 1000000) % 100);
	tmMkTime.tm_hour = I32((i64YYYYMMDDHHMMSS / 10000) % 100);
	tmMkTime.tm_min = I32(i64YYYYMMDDHHMMSS / 100 % 100);
	tmMkTime.tm_sec = I32(i64YYYYMMDDHHMMSS % 100);

	return mktime(&tmMkTime);
}

static inline time_t GetZeroTimestamp(const I64 i64YYYYMMDDHHMMSS)
{
	I64 nZeroYYYYMMDDHHMMSS = i64YYYYMMDDHHMMSS / 1000000 * 1000000;

	return GetTimestampFromYYYYMMDDHHMMSS(nZeroYYYYMMDDHHMMSS);
}

static inline time_t Get24Timestamp(const I64 i64YYYYMMDDHHMMSS)
{
	I64 n24YYYYMMDDHHMMSS = i64YYYYMMDDHHMMSS / 1000000 * 1000000 + 24 * 10000;

	return GetTimestampFromYYYYMMDDHHMMSS(n24YYYYMMDDHHMMSS);
}

//不足1天的余数秒数不算1天，即得到的是结束天与起始天相隔的天数
static inline I64 GetDiffDay(const I64 nYYYYMMDDHHMMSSStart, const I64 nYYYYMMDDHHMMSSEnd)
{
	auto nStartStamp = GetZeroTimestamp(nYYYYMMDDHHMMSSStart);
	if (nStartStamp < 0)
		return -1;

	auto nEndStamp = GetZeroTimestamp(nYYYYMMDDHHMMSSEnd);
	if (nEndStamp < 0)
		return -1;

	return abs(nEndStamp - nStartStamp) / (3600 * 24);
}

//不足1天的余数秒数不算1天，即得到的是结束天与起始天相隔的天数
static inline I64 GetDiffDayByStamp(const time_t nTimestampStart, const time_t nTimestampEnd)
{
	if (nTimestampStart < 0)
		return -1;

	if (nTimestampEnd < 0)
		return -1;

	auto nStart = GetYYYYMMDDHHMMSS(nTimestampStart);
	auto nEnd = GetYYYYMMDDHHMMSS(nTimestampEnd);
	auto nStartStamp = GetZeroTimestamp(nStart);
	auto nEndStamp = GetZeroTimestamp(nEnd);

	return abs(nEndStamp - nStartStamp) / (3600 * 24);
}

//不足1小时的余数秒数不算1小时，即得到的是结束天与起始天相隔的小时数
static inline I64 GetDiffHour(const I64 nYYYYMMDDHHMMSSStart, const I64 nYYYYMMDDHHMMSSEnd)
{
	auto nStart = nYYYYMMDDHHMMSSStart / 10000 * 10000;
	auto nEnd = nYYYYMMDDHHMMSSEnd / 10000 * 10000;

	auto nStartStamp = GetTimestampFromYYYYMMDDHHMMSS(nStart);
	if (nStartStamp < 0)
		return -1;

	auto nEndStamp = GetTimestampFromYYYYMMDDHHMMSS(nEnd);
	if (nEndStamp < 0)
		return -1;

	return abs(nEndStamp - nStartStamp) / (3600);
}

//不足1分钟的余数秒数不算1分钟，即得到的是结束天与起始天相隔的分钟数
static inline I64 GetDiffMinute(const I64 nYYYYMMDDHHMMSSStart, const I64 nYYYYMMDDHHMMSSEnd)
{
	auto nStart = nYYYYMMDDHHMMSSStart / 100 * 100;
	auto nEnd = nYYYYMMDDHHMMSSEnd / 100 * 100;

	auto nStartStamp = GetTimestampFromYYYYMMDDHHMMSS(nStart);
	if (nStartStamp < 0)
		return -1;

	auto nEndStamp = GetTimestampFromYYYYMMDDHHMMSS(nEnd);
	if (nEndStamp < 0)
		return -1;

	return abs(nEndStamp - nStartStamp) / 60;
}

static inline MYULONG CpuCoreCount()
{
	MYULONG count = 1; // 至少一个  
#if defined (LINUX)  
	count = sysconf(_SC_NPROCESSORS_CONF);
#elif defined(_WINDOWS)  
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	count = si.dwNumberOfProcessors;
#endif  
	return count;
}



static bool Conver2HexStr(const char *szBuffer, const U64 nLen, char *szBufferOut, const U64 nSizeOut)
{
	CHECKF_NL( (2*nLen) < nSizeOut && szBuffer&&szBufferOut);
	char *pPos = szBufferOut;
	char szCache[4] = { 0 };
	I32 nCacheLen = 0;
	for (U64 i = 0; i < nLen; ++i)
	{
		ZERO_CLEAN_OBJ(szCache);
		nCacheLen = sprintf(szCache, "%02x", U8(szBuffer[i]));	
		nCacheLen = gtool::Max(nCacheLen, 0);
		memmove(pPos, szCache, nCacheLen);
		pPos += nCacheLen;
		pPos[0] = 0;
		CHECKF_NL(pPos <= (szBufferOut + nSizeOut));
	}
	CHECKF_NL(strlen(szBufferOut));

	return true;
}

static bool md5encode(const I8* strsrc, const U64 u64SrcLen, I8* strout, const U64 u64SizeOut)
{
	CHECKF_NL(strsrc&&u64SrcLen&&strout && (u64SizeOut >= MD5_VERIFY_SIZE));
	I8 BufferMd5[MD5_DIGEST_LENGTH] = { 0 };

	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, strsrc, u64SrcLen);
	MD5_Final((U8 *)(&(BufferMd5[0])), &c);
	return Conver2HexStr(BufferMd5, MD5_DIGEST_LENGTH, strout, u64SizeOut);
}

NAMESPACE_END(gtool)

#endif





