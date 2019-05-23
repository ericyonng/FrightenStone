#include "stdafx.h"
#include <BaseCode/File2.h>
#include <BaseCode/FileDirSys.h>
#include <BaseCode/MyBaseMacro.h>
#include <BaseCode/Win/WinLocker.h>

CFile2::CFile2()
:m_fptr(NULL)
{
}

CFile2::~CFile2()
{
	Cleans();
}

void CFile2::Cleans()
{
	m_Locker.Lock();
	if (m_fptr)
	{
		ASSERT(gtool::CFileDirSys::FlushFile(*m_fptr));
		ASSERT(gtool::CFileDirSys::CloseFile(*m_fptr));
		m_fptr = NULL;
	}
	m_Locker.UnLock();
}

bool CFile2::Open(const char *szFileName, bool bCreate/*=false*/, const char *szOpenMode/*="ab+"*/, bool bUseTimestampTailer /*= false*/)
{
	//确保只有关闭文件后才能再打开或者文件指针被转移
	CHECKF_NL(m_fptr == NULL);
	std::string strFileName = szFileName;
	m_bUseTimestampTailer = bUseTimestampTailer;
	if (m_bUseTimestampTailer)
	{
		m_CreateFileTimestamp = gtool::FileInsertTime(strFileName);
		m_ModifyFileTimestamp = m_CreateFileTimestamp;
	}

	m_fptr = gtool::CFileDirSys::OpenFile(strFileName.c_str(), bCreate, szOpenMode);
	if (m_fptr)
	{
		std::string strWhole = szFileName;
		m_strFileName = gtool::CFileDirSys::GetFileNameInPath(strWhole);
		m_strPath = gtool::CFileDirSys::GetFilePathInWholePath(strWhole);
		m_strOpenMode = szOpenMode;
	}

	return m_fptr != NULL;
}

bool CFile2::Reopen()
{
	CHECKF_NL(Close());

	std::string strWholeName = m_strPath + m_strFileName;
	return Open(strWholeName.c_str(), true, m_strOpenMode.c_str(), m_bUseTimestampTailer);
}

U64 CFile2::Write(const void *pBuffer, const U64& u64WriteDataLen)
{
	CHECKF_NL(m_fptr);

	auto u64WrLen = gtool::CFileDirSys::WriteFile(*m_fptr, pBuffer, u64WriteDataLen);
	if (u64WrLen != 0)
	{
		m_ModifyFileTimestamp = time(NULL);
	}

	return u64WrLen;
}

U64 CFile2::Read(void *&pBuffer, const U64& u64ReadLen)
{
	CHECKF_NL(m_fptr);

	return gtool::CFileDirSys::ReadFile(*m_fptr, pBuffer, u64ReadLen);
}

U64 CFile2::ReadOneLine(void *&pBuffer, const U64& u64ReadLen)
{
	CHECKF_NL(m_fptr);

	return gtool::CFileDirSys::ReadOneLine(*m_fptr, pBuffer, u64ReadLen);
}

bool CFile2::Flush()
{
	CHECKF_NL(m_fptr);
	return gtool::CFileDirSys::FlushFile(*m_fptr);
}

bool CFile2::Close()
{
	if (m_fptr)
	{
		bool bSuc = true;
		bSuc &= gtool::CFileDirSys::FlushFile(*m_fptr);
		ASSERT(bSuc);
		bSuc &= gtool::CFileDirSys::CloseFile(*m_fptr);
		ASSERT(bSuc);

		m_fptr = NULL;
		return bSuc;
	}

	m_fptr = NULL;
	return true;
}

bool CFile2::Lock()
{
	return m_Locker.Lock();
}

bool CFile2::UnLock()
{
	return m_Locker.UnLock();
}

const char * CFile2::GetPath()
{
	if (!m_fptr) return "";

	return m_strPath.c_str();
}

const char * CFile2::GetFileName()
{
	if (!m_fptr) return "";

	return m_strFileName.c_str();
}

bool CFile2::IsOpen() const
{
	return m_fptr != NULL;
}

// bool CFile2::IsTooLarge() const
// {
// 	CHECKF_NL(m_strCurFileNameTmp.length() > 0);
// 	return IsFileTooLarge(m_strCurFileNameTmp.c_str());
// }

CFile2::operator bool()
{
	return m_fptr != NULL;
}

CFile2::operator FILE *()
{
	return m_fptr;
}

CFile2::operator const FILE *()
{
	return m_fptr;
}


// bool CFile2::IsFileTooLarge(const char *szFile) const
// {
// 	auto nFileSize = gtool::CFileDirSys::GetFileSizeEx(szFile);
// 	return I64(nFileSize + m_u64FileSizeAdd) >= LOGFILE_SIZE_LIMIT;
// }
// 
// bool CFile2::GetSuitableSizeFile(std::string& strFile, I64& i64PartId)
// {
// 	CHECKF_NL(strFile.length());
// 
// 	std::string strTmp = strFile;
// 
// 	gtool::CFmtIOString strTail;
// 	strTail << "Part" << i64PartId;
// 	strTmp = strFile;
// 	ASSERT(gtool::FileInsertTail(strTmp, strTail.c_str()));	
// 	while (IsFileTooLarge(strTmp.c_str()))
// 	{
// 		++i64PartId;
// 		strTail.Clear();
// 		strTail << "Part" << i64PartId;
// 		strTmp = strFile;
// 		ASSERT(gtool::FileInsertTail(strTmp, strTail.c_str()));
// 	}
// 	strFile = strTmp;
// 
// 	return true;
// }

////////////////////////////////////////////log

CLogFile::CLogFile()
{
}

CLogFile::~CLogFile()
{
}

void CLogFile::Cleans()
{
	CFile2::Cleans();
}

bool CLogFile::IsDayPass(const time_t tLastModify) const
{
	auto nDiff = gtool::GetDiffDayByStamp(m_tLastTimestamp, tLastModify);
	if (nDiff <= 0)
		return false;

	return true;
}


void CLogFile::UpdateLastTimestamp()
{
	m_tLastTimestamp = time(NULL);
}
