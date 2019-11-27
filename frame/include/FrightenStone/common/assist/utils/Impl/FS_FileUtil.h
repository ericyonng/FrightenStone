/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : FS_FileUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_FS_FileUtil_H__
#define __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_FS_FileUtil_H__

#pragma once

#include<FrightenStone/exportbase.h>
#include "FrightenStone/common/basedefs/DataType/DataType.h"
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
#include<stdio.h>

FS_NAMESPACE_BEGIN

class FS_String;
class Time;

class BASE_EXPORT FS_FileUtil
{
public:
    static void DelFile(const char *filePath);
    static bool DelFileCStyle(const char *filePath);
    static FILE *CreateTmpFile();
    static const char *GenRandFileName(char randName[L_tmpnam]);
    static const char *GenRandFileNameNoDir(char randName[L_tmpnam]);
    static FILE *OpenFile(const char *fileName, bool isCreate = false, const char *openType = "rb+");
    static bool CopyFile(const char *srcFile, const char *destFile);
    static bool CopyFile(FILE &src, FILE &dest);
    static UInt64 ReadOneLine(FILE &fp, UInt64 bufferSize, char *&buffer);
    static UInt64 ReadOneLine(FILE &fp, FS_String &outBuffer);
    static UInt64 ReadFile(FILE &fp, UInt64 bufferSize, char *&buffer);
    static UInt64 ReadFile(FILE &fp, FS_String &outString, Int64 sizeLimit = -1);
    static Int64 WriteFile(FILE &fp, const char *buffer, Int64 dataLenToWrite);
    static Int64 WriteFile(FILE &fp, const FS_String &bitData);
    static bool CloseFile(FILE &fp);
    static bool IsFileExist(const char *fileName);
    static Int32 GetFileCusorPos(FILE &fp);
    static bool SetFileCursor(FILE &fp, Int32 enumPos, long offset);
    static void ResetFileCursor(FILE &fp);
    static bool FlushFile(FILE &fp);
    static long GetFileSize(FILE &fp);
    static Int64 GetFileSizeEx(const char *filepath);
    static void InsertFileTime(const FS_String &extensionName, const Time &timestamp, FS_String &fileName);
    static void InsertFileTail(const FS_String &extensionName, const char *tail, FS_String &fileName);
    static FS_String ExtractFileExtension(const FS_String &fileName);
    static FS_String ExtractFileWithoutExtension(const FS_String &fileName);
};

FS_NAMESPACE_END

#endif
