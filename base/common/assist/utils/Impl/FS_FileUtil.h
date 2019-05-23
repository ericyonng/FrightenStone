#ifndef __Base_Common_Assist_Utils_Impl_FS_FileUtil_H__
#define __Base_Common_Assist_Utils_Impl_FS_FileUtil_H__
/**
* @file STLUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/08
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include<stdio.h>

FS_NAMESPACE_BEGIN

class FS_String;
class Time;

class BASE_EXPORT FS_FileUtil
{
public:
    // 删除文件
    static void DelFile(const char *filePath);
    // 删除文件
    static bool DelFileCStyle(const char *filePath);
    // 生成临时文件 以w+b的模式打开返回临时文件的流指针
    static FILE *CreateTmpFile();
    // 获取随机的唯一的文件名 数组长度至少为 L_tmpnam 
    static const char *GenRandFileName(char randName[L_tmpnam]);
    static const char *GenRandFileNameNoDir(char randName[L_tmpnam]);
    // 打开文件
    static FILE *OpenFile(const char *fileName, bool isCreate = false, const char *openType = "rb+");
    // 拷贝文件
    static bool CopyFIle(const char *srcFile, const char *destFile);
    // 读取一行
    static UInt64 ReadOneLine(FILE &fp, UInt64 bufferSize, char *&buffer);
    static UInt64 ReadOneLine(FILE &fp, FS_String &outBuffer);
    // 读取文件
    static UInt64 ReadFile(FILE &fp, UInt64 bufferSize, char *&buffer);
    static UInt64 ReadFile(FILE &fp, FS_String &outString, Int64 sizeLimit = -1);
    // 写入文件
    static UInt64 WriteFile(FILE &fp, const char *buffer, UInt64 dataLenToWrite);
    static UInt64 WriteFile(FILE &fp, const FS_String &bitData);
    // 关闭文件
    static bool CloseFile(FILE &fp);
    // 文件是否存在
    static bool IsFileExist(const char *fileName);
    // 设置文件指针位置
    static bool SetFileCursor(FILE &fp, Int32 enumPos, long offset);
    // 重置文件指针位置到文件头
    static void ResetFileCursor(FILE &fp);
    // 文件缓冲着盘
    static bool FlushFile(FILE &fp);
    // 获取文件大小
    static long GetFileSize(FILE &fp);  // 文件尺寸需要小于2GB long的取值范围，避免函数出错返回-1
    static Int64 GetFileSizeEx(const char *filepath);       // 

    // 添加时间
    static void InsertFileTime(const FS_String &extensionName, const Time &timestamp, FS_String &fileName);

    // 添加尾部标识
    static void InsertFileTail(const FS_String &extensionName, const char *tail, FS_String &fileName);

    // 获取扩展名
    static FS_String ExtractFileExtension(const FS_String &fileName);
};

FS_NAMESPACE_END

#endif
