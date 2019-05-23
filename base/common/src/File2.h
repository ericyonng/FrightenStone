#ifndef FILE_2_H_
#define FILE_2_H_

#pragma once
//#include <BaseCode/BaseCodeApi.h>
//#include <BaseCode/myclassmacro.h>

class CDataLocker;

//线程安全 不可拷贝不可转移
class  CFile2
{
	NO_COPY_NO_MOVE(CFile2);
	HEAP_CREATE_NEED_CLEANS_DEFINE(CFile2);


	//打开，读，写，刷新，关闭
public:
	virtual bool Open(const char *szFileName, bool bCreate=false, const char *szOpenMode="ab+", bool bUseTimestampTailer = false);
	virtual bool Reopen();
	virtual U64 Write(const void *pBuffer, const U64& u64WriteDataLen);
	virtual U64 Read(void *&pBuffer, const U64& u64ReadLen);
	virtual U64 ReadOneLine(void *&pBuffer, const U64& u64ReadLen);
	virtual bool Flush();
	virtual bool Close();
	bool Lock();
	bool UnLock();
	const char *GetPath();
	const char *GetFileName();
	bool IsOpen() const;
	//void EnableDivision(bool bEnable) { m_bDivision = bEnable; }
	//bool IsTooLarge() const;

public:
	operator bool();
	operator FILE *();
	operator const FILE *();

	//内部接口
protected:
	//bool IsFileTooLarge(const char *szFile) const;
	bool GetSuitableSizeFile(std::string& strFile, I64& i64PartId);

protected:
	FILE *			m_fptr = NULL;
	time_t			m_CreateFileTimestamp = 0;
	time_t			m_ModifyFileTimestamp = 0;
	bool			m_bUseTimestampTailer = false;
	std::string		m_strPath;
	std::string		m_strFileName;
	std::string		m_strOpenMode = "ab+";
	CDataLocker		m_Locker;
};

//每个日志文件1个GB 1024*1024*1024*1024
//#define LOGFILE_SIZE_LIMIT I64(1073741824)	//1GB
#define LOGFILE_SIZE_LIMIT I64(1024)	//1KB
class CLogFile :public CFile2
{
	NO_COPY_NO_MOVE(CLogFile);
	HEAP_CREATE_NEED_CLEANS_DEFINE(CLogFile);

public:
	bool IsDayPass(const time_t tLastModify) const;
	void UpdateLastTimestamp();


protected:
	time_t	m_tLastTimestamp = 0;

};

#endif
