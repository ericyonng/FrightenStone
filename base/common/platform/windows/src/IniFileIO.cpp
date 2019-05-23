#include"stdafx.h"
#include"IniFileIO.h"
#include<io.h>
#include<stdio.h>

CIniFileIO::CIniFileIO()
{
	m_strFilePath[0] = 0;
}

//设置路径(绝对路径并附上文件名)
bool CIniFileIO::SetFilePath(const char * szFilePath)
{
	//设置变量
	m_strFilePath[0] = 0;
	auto nLen = sprintf(m_strFilePath, "%s", szFilePath);
	nLen = gtool::Max(nLen, 0);
	m_strFilePath[(nLen< INI_FILE_NAME_LEN - 1)? nLen:(INI_FILE_NAME_LEN - 1)] = 0;

	return TRUE; 
}

//获取路径
const char * CIniFileIO::GetFilePath()
{
	return m_strFilePath;
}

//获取字符串
const char * CIniFileIO::ReadString(const char * pszItem, const char * pszSubItem, const char * pszDefault, char *& pszString, const U16& u16MaxCount)
{
	IF_NOT_NL(strlen(m_strFilePath) != 0)
		return NULL;

	//读取字符
	GetPrivateProfileString(pszItem, pszSubItem, pszDefault, pszString, u16MaxCount, m_strFilePath);
	return pszString;
}

//获取整型值
U32 CIniFileIO::ReadInt(const char *pszItem, const char *pszSubItem, const I32 &nDefault)
{
	CHECKF_NL(strlen(m_strFilePath) != 0);

	//读取数值
	U32 uReadData = GetPrivateProfileInt(pszItem, pszSubItem, nDefault, m_strFilePath);

	return uReadData;
}

//写入字符串
bool CIniFileIO::WriteString(const char *lpAppName, const char *lpKeyName, const char *lpString, const char *lpFileName)
{ 
	auto pFile = gtool::CFileDirSys::OpenFile(lpFileName, true);
	CHECKF_NL(pFile);
	CHECKF_NL(gtool::CFileDirSys::CloseFile(*pFile));

	return WritePrivateProfileString(lpAppName, lpKeyName, lpString, lpFileName);
}

