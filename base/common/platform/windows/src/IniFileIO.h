#ifndef INI_FILE_IO_H_		//IniFileIO
#define INI_FILE_IO_H_

#pragma once



class  CIniFileIO
{
	NO_COPY_NO_MOVE(CIniFileIO);
public:
	CIniFileIO();
	virtual ~CIniFileIO() {}

	//文件路径
public:
	//设置路径(绝对路径并附上文件名)
	bool SetFilePath(const char * szFilePath);
	//获取路径
	const char *GetFilePath();
	
	//文件IO
public:
	//获取字符串
	const char * ReadString(const char * pszItem, const char * pszSubItem, const char * pszDefault, char *& pszString, const U16& u16MaxCount);
	//获取整型值
	U32 ReadInt(const char *pszItem, const char *pszSubItem, const I32 &nDefault);
	//写入字符串
	bool WriteString(const char *lpAppName, const char *lpKeyName, const char *lpString, const char *lpFileName);

protected:
	char  m_strFilePath[INI_FILE_NAME_LEN];
};
#endif