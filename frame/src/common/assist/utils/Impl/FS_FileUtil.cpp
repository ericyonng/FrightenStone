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
 * @file  : FS_FileUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "FrightenStone/common/assist/utils/Impl/FS_FileUtil.h"
#include "FrightenStone/common/assist/utils/Impl/FS_DirectoryUtil.h"
#include <string>
#include "FrightenStone/common/component/component.h"
#include "FrightenStone/common/basedefs/DataType/DataType.h"

#ifdef _WIN32
#include<direct.h>      //mkdir
#include<io.h>          //access
#else
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#endif

FS_NAMESPACE_BEGIN

void FS_FileUtil::DelFile(const char *filePath)
{
    std::string strDelCmd = "del ";
    strDelCmd += filePath;
    size_t findPos = 0;
    int nCount = 0;
    const auto strCount = strDelCmd.length();
    while((findPos = strDelCmd.find_first_of('/', findPos)) != std::string::npos)
    {
        strDelCmd[findPos] = '\\';
    }
    strDelCmd += " /f/s/q";

    system(strDelCmd.c_str());
}

bool FS_FileUtil::DelFileCStyle(const char *filePath)
{
    return remove(filePath) == 0;
}

FILE  *FS_FileUtil::CreateTmpFile()
{
    return tmpfile();
}

const char *FS_FileUtil::GenRandFileName(char randName[L_tmpnam])
{
    return tmpnam(randName);
}

const char * FS_FileUtil::GenRandFileNameNoDir(char randName[L_tmpnam])
{
    if(tmpnam(randName))
    {
        auto fileName = FS_DirectoryUtil::GetFileNameInPath(randName);
        if(fileName.GetRaw().length() > 0)
        {
            randName[0] = 0;
            auto len = sprintf(randName, "%s", fileName.c_str());
            len = ((len < L_tmpnam) ? std::max<Int32>(len, 0) : (L_tmpnam - 1));
            randName[len] = 0;
            return randName;
        }
    }

    return NULL;
}

FILE *FS_FileUtil::OpenFile(const char *fileName, bool isCreate /*= false*/, const char *openType /*= "rb+"*/)
{
    if(fileName == 0 || openType == NULL)
        return NULL;

    FILE *fp = NULL;
    fp = fopen(fileName, openType);
    if(!fp)
    {
        if(isCreate)
        {
            fp = fopen(fileName, "wb+");
            if(!fp)
                return NULL;
        }
        else
        {
            return NULL;
        }
    }

    clearerr(fp);
    rewind(fp);
    return fp;
}

bool FS_FileUtil::CopyFile(const char *srcFile, const char *destFile)
{
    if(UNLIKELY(!srcFile || !destFile))
        return false;

    auto srcFp = OpenFile(srcFile);
    if(!srcFp)
        return false;

    auto destFp = OpenFile(destFile, true, "wb+");
    if(!destFp)
        return false;

    clearerr(srcFp);
    clearerr(destFp);
    unsigned char get_c = 0;
    char count = 0, wrCount = 0;
    const auto sizeByte = GetFileSize(*srcFp);  //
    while(!feof(srcFp))
    {
        get_c = 0;
        count = char(fread(&get_c, 1, 1, srcFp));
        if(count != 1)
            break;

        wrCount = char(fwrite(&get_c, 1, 1, destFp));
        if(wrCount != 1)
            break;

        FlushFile(*destFp);
    }

    CloseFile(*srcFp);
    CloseFile(*destFp);
    return true;
}

bool FS_FileUtil::CopyFile(FILE &src, FILE &dest)
{
    clearerr(&src);
    clearerr(&dest);
    unsigned char get_c = 0;
    char count = 0, wrCount = 0;
    const auto sizeByte = GetFileSize(src);  //
    while(!feof(&src))
    {
        get_c = 0;
        count = char(fread(&get_c, 1, 1, &src));
        if(count != 1)
            break;

        wrCount = char(fwrite(&get_c, 1, 1, &dest));
        if(wrCount != 1)
            break;
    }

    FlushFile(dest);

    return true;
}

UInt64 FS_FileUtil::ReadOneLine(FILE &fp, UInt64 bufferSize, char *&buffer)
{
    if(!buffer || bufferSize == 0)
        return 0;

    unsigned char get_c = 0;
    unsigned char *bufferTmp = reinterpret_cast<unsigned char *>(buffer);
    memset(bufferTmp, 0, bufferSize);

    clearerr(&fp);
    UInt64 cnt = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
#if (defined(_WIN32)|defined(_WIN64))
            if(get_c == '\r')
                continue;

            if(get_c != '\n')
            {
                *bufferTmp = get_c;
                ++bufferTmp;
                ++cnt;

                if(bufferSize <= cnt) 
                    break;
            }
            else
            {
                //SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_CUR, 0);
                break;
            }

#else
            if(get_c != '\n')
            {
                *bufferTmp = get_c;
                ++bufferTmp;
                ++cnt;

                if(bufferSize <= cnt) 
                    break;
            }
            else
            {
                //fread(&get_c, sizeof(get_c), 1, fpOutCache);
                break;
            }
#endif

        }
        else
        {
            break;
        }
    }

    return cnt;
}

UInt64 FS_FileUtil::ReadOneLine(FILE &fp, FS_String &outBuffer)
{
    clearerr(&fp);
    unsigned char get_c = 0;
    UInt64 cnt = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
#if (defined(_WIN32)|defined(_WIN64))
            if(get_c == '\r')
                continue;

            if(get_c != '\n')
            {
                outBuffer.AppendBitData(reinterpret_cast<const char *>(&get_c), 1);
                ++cnt;
            }
            else
            {
                break;
            }
#else
            if(get_c != '\n')
            {
                outBuffer.AppendBitData(reinterpret_cast<const char *>(&get_c), 1);
                ++cnt;
            }
            else
            {
                //fread(&get_c, sizeof(get_c), 1, fpOutCache);
                break;
            }
#endif
        }
        else
        {
            break;
        }
    }

    return cnt;
}

UInt64 FS_FileUtil::ReadFile(FILE &fp, UInt64 bufferSize, char *&buffer)
{
    if(!buffer || !bufferSize)
        return 0;

    clearerr(&fp);
    UInt64 readCnt = 0;
    unsigned char *bufferTmp = reinterpret_cast<unsigned char *>(buffer);
    unsigned char get_c = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
            *bufferTmp = get_c;
            ++bufferTmp;
            ++readCnt;

            if(readCnt >= bufferSize)
                break;
        }
        else
        {
            break;
        }
    }

    return readCnt;
}

UInt64 FS_FileUtil::ReadFile(FILE &fp, FS_String &outString, Int64 sizeLimit)
{
    clearerr(&fp);
    UInt64 readCnt = 0;
    unsigned char get_c = 0;
    while(!feof(&fp))
    {
        get_c = 0;
        if(fread(&get_c, sizeof(get_c), 1, &fp) == 1)
        {
            outString.AppendBitData(reinterpret_cast<const char *>(&get_c), 1);
            ++readCnt;

            if(sizeLimit > 0 && 
               static_cast<Int64>(readCnt) >= sizeLimit)
                break;
        }
        else
        {
            break;
        }
    }

    return readCnt;
}

Int64 FS_FileUtil::WriteFile(FILE &fp, const char *buffer, Int64 dataLenToWrite)
{
//     if(!buffer || dataLenToWrite == 0)
//         return 0;

    clearerr(&fp);
    Int64 cnt = 0;
    while(dataLenToWrite != 0)
    {
        --dataLenToWrite;
        if(fwrite(buffer + cnt, 1, 1, &fp) == 1)
        {
            ++cnt;
            continue;
        }

        break;
    }

//     if(dataLenToWrite != cnt)
//         printf("write error!");

    return cnt;
}

Int64 FS_FileUtil::WriteFile(FILE &fp, const FS_String &bitData)
{
    return WriteFile(fp, bitData.GetRaw().data(), bitData.GetLength());
}

bool FS_FileUtil::IsEnd(FILE &fp)
{
    return feof(&fp);
}

bool FS_FileUtil::CloseFile(FILE &fp)
{
    clearerr(&fp);
    if(fclose(&fp) != 0)
        return false;

    return true;
}

bool FS_FileUtil::IsFileExist(const char *fileName)
{
    if(UNLIKELY(!fileName))
        return false;

#ifdef _WIN32
    if(::_access(fileName, 0) == -1)
        return false;
#else
    if(::access(fileName, 0) == -1)
        return false;
#endif

    return true;
}

Int32 FS_FileUtil::GetFileCusorPos(FILE &fp)
{
    return ftell(&fp);
}

bool FS_FileUtil::SetFileCursor(FILE &fp, Int32 enumPos, long offset)
{
    return fseek(&fp, offset, enumPos) == 0;
}

void FS_FileUtil::ResetFileCursor(FILE &fp)
{
    clearerr(&fp);
    rewind(&fp);
}

bool FS_FileUtil::FlushFile(FILE &fp)
{
    return fflush(&fp) == 0;
}

long FS_FileUtil::GetFileSize(FILE &fp)
{
    auto curPos = ftell(&fp);
    if(UNLIKELY(curPos < 0))
        return -1;

    if(UNLIKELY(!SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_END, 0L)))
        return -1;

    auto fileSize = ftell(&fp);
    if(UNLIKELY(fileSize < 0))
    {
        SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_SET, curPos);
        return -1;
    }

    SetFileCursor(fp, FileCursorOffsetType::FILE_CURSOR_POS_SET, curPos);
    return fileSize;
}

Int64 FS_FileUtil::GetFileSizeEx(const char *filepath)
{
#ifdef _WIN32
    struct _stat info;
    if(::_stat(filepath, &info) != 0)
        return -1;

    return info.st_size;
#else
    struct stat info;
    if(::stat(filepath, &info) != 0)
        return -1;

    return info.st_size;
#endif

}

void FS_FileUtil::InsertFileTime(const FS_String &extensionName, const Time &timestamp, FS_String &fileName)
{
    auto endPos = fileName.GetRaw().rfind('.', fileName.GetLength() - 1);
    const auto &timeFmtStr = timestamp.Format("%Y-%m-%d");
    if(endPos == std::string::npos)
    {
        fileName << timeFmtStr << extensionName;
        return;
    }
    fileName.GetRaw().insert(endPos, timeFmtStr.GetRaw());
}

void FS_FileUtil::InsertFileTail(const FS_String &extensionName, const char *tail, FS_String &fileName)
{
    auto endPos = fileName.GetRaw().rfind('.', fileName.GetLength() - 1);
    if(endPos == std::string::npos)
    {
        fileName << tail << extensionName;
        return;
    }

    fileName.GetRaw().insert(endPos, tail);
}

FS_String FS_FileUtil::ExtractFileExtension(const FS_String &fileName)
{
    auto endPos = fileName.GetRaw().rfind('.', fileName.GetLength() - 1);
    if(endPos == std::string::npos)
        return "";

    return fileName.GetRaw().substr(endPos, fileName.GetLength() - endPos);
}

FS_String FS_FileUtil::ExtractFileWithoutExtension(const FS_String &fileName)
{
    auto endPos = fileName.GetRaw().rfind('.', fileName.GetLength() - 1);
    if(endPos == std::string::npos)
        return fileName;

    if(endPos == 0)
        return "";

    return fileName.GetRaw().substr(0, endPos);
}
FS_NAMESPACE_END

