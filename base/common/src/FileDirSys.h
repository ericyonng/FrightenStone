/*********************************
/*	FileName: FileDirSys.h
/*	Auther:	Feng Yanchen
/*	Creation Date: 2016-05-04
/*	Date Updated : 2016-05-04
*********************************/

#ifndef FILE_DIR_SYS_H_
#define FILE_DIR_SYS_H_

#pragma once 
//#include <BaseCode/BaseCodeApi.h>

//#include <BaseCode/myclassmacro.h>


#ifdef _WIN64


#endif

#ifndef FILE_DIR_SYS_H_FILE_CURSOR_POS_TYPE_
#define  FILE_DIR_SYS_H_FILE_CURSOR_POS_TYPE_

enum FILE_CURSOR_POS
{
	FILE_CURSOR_POS_SET=0,
	FILE_CURSOR_POS_CUR=1,
	FILE_CURSOR_POS_END=2,
};

#endif
///////////////////////////////////////////////////////////////////////

//数据操作
#define BYTE_UNIT_BITS		0x8											//单字节数据位数
#define GET_WDATA_HIGH_BYTE(x)	((( (x)&0x0FFFF )>>BYTE_UNIT_BITS)&0x0FF)		//取WORD数据的高字节
#define GET_WDATA_LOW_BYTE(x)	(( (x)&0x0FFFF)&0x0FF)						//取WORD数据的低字节

///////////////////////////////////////////

NAMESPACE_BEGIN(gtool)

//文件操作
class CFileDirSys
{
	NO_COPY_NEED_RVALUE(CFileDirSys);
	private:
	CFileDirSys(){}
	~CFileDirSys(){}

public:
	static bool SaveDataAsFile(const char *szFileName, const char *szString, const U64 nStrByte);

// #ifdef _WIN64
// 
// 	//字符转换
// 	static bool UnicodeToAnsi(LPTSTR szUnicodeSrc, char *pAnsiStrOut, int nAnsiStrSize, int *pAnsiOutStrlenByte = NULL);
// 
// 	//字符转换
// 	static bool AnsiToUnicode(char *pAnsiStrIn, LPTSTR szUnicodeStrOut, int nUnicodeCacheSize, int *nUnicodeStrLenOutCalculateZeroTail = NULL);
// 
// 	//utf8->ansi 带windows api
// 	static char * Utf8Convert2Ansi(const char *pUtf8);	//返回由堆区创建的字符串
// 	//ansi->utf8 带windows api
// 	static char * AnsiConvert2Utf8(const char *pAnsi);	//返回由堆区创建的字符串
// #endif



	//文件api
public:
	//删除文件
	static void DelFile(const char *szfile);

	//删除文件
	static bool DelFileCStyle(const char *szfile);

	//生成临时文件 以w+b的模式打开返回临时文件的流指针
	static FILE *GenTmpFile();

	//获取随机的唯一的文件名 数组长度至少为 L_tmpnam 
	static char *GenRandFileName(char szRandName[L_tmpnam]);

	static char *GenRandFileNameNoDir(char szRandName[L_tmpnam]);

	//递归创建目录
	static bool CreateRecursiveDir(const std::string& strAbsolutePath);

	//打开文件
	static FILE *OpenFile(const char *szFileName, bool bCreate=false, const char *pOpenType = (char *)"rb+");

	//拷贝文件
	static bool CopyFIle(const char* szSrcFile, const char * szDestFile);

	//读取一行
	static U64 ReadOneLine(FILE& fp, void *& pBuffer, const U64& u64ReadDataLen);

	//读取文件
	static U64 ReadFile(FILE&fp, void *&pBuffer, const U64& u64ReadDataLen);

	//写入文件
	static U64 WriteFile(FILE& fp, const void *pBuffer, const U64& u64WriteDataLen);

	//关闭文件
	static bool CloseFile(FILE& fp);

	//文件是否存在
	static bool IsFileExist(const char * szFileName);

	//路径中截取文件
	static std::string GetFileNameInPath(const std::string& strPath);
	static std::string GetFileNameInPath(const char * szPath);
	static std::string GetFilePathInWholePath(const std::string& strPath);
	static std::string GetFilePathInWholePath(const char *szPath);

	static bool SetFileCursor(FILE& fp, FILE_CURSOR_POS ePos, long lOffset);
	static void ResetFileCursor(FILE& fp);
	static bool FlushFile(FILE &fp);
	static long GetFileSize(FILE& fp);		//文件尺寸需要小于2GB long的取值范围，避免函数出错返回-1
	static I64 GetFileSizeEx(const char * filepath);


// #ifdef _WINNT_
// 
// 	//utf8->ansi 带windows api
// 	static std::string Utf8Convert2Ansi(const char *pUtf8);
// 	//ansi->utf8 带windows api
// 	static std::string AnsiConvert2Utf8(const char *pAnsi);
// #endif

	//Internal fun
protected:
	//创建文件夹
	static bool CreateSubDir(const std::string& strDir);

	//递归创建子目录
	static bool CreateRecursiveSubDir(const std::string& strMasterDir, const std::string& strSubDir);
};

NAMESPACE_END(gtool)

#endif