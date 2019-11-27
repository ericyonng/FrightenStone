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
 * @file  : FS_File.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_File_FS_File_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_File_FS_File_H__


#pragma once

#include <FrightenStone/exportbase.h>
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
//#include "FrightenStone/common/component/Impl/Time.h"
//#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/asyn/Lock/Lock.h"
// #include <string>
#include "FrightenStone/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

class Time;
class FS_String;

class BASE_EXPORT FS_File
{
    OBJ_POOL_CREATE_DEF(FS_File);
public:
    FS_File();
    virtual ~FS_File();

public:
    virtual bool Open(const char *fileName, bool isCreate = false, const char *openMode = "ab+", bool useTimestampTailer = false);
    virtual bool Reopen();
    virtual bool Flush();
    virtual bool Close();

    virtual Int64 Write(const void *buffer, UInt64 writeDataLen);
    virtual Int64 Read(FS_String &outBuffer);
    virtual Int64 Read(Int64 sizeLimit, FS_String &outBuffer);
    virtual Int64 ReadOneLine(FS_String &outBuffer);

    void Lock();
    void UnLock();

    bool IsOpen() const;
    const FS_String &GetPath() const;
    const FS_String &GetFileName() const;
    Int64 GetSize() const;

public:
    operator bool() const;
    operator FILE *();
    operator const FILE *() const;

protected:
    FILE *_fp = NULL;
    Int64 _fileSize;
    Time _createFileTime;
    Time _modifyFileTime;
    bool _useTimestampTailer = false;
    FS_String _path;
    FS_String _fileName;
    FS_String _extensionName;       // 扩展名
    FS_String _openMode = "ab+";
    Locker  _locker;
};

#pragma region Inline

inline void FS_File::Lock()
{
    _locker.Lock();
}

inline void FS_File::UnLock()
{
    _locker.Unlock();
}

inline bool FS_File::IsOpen() const
{
    return _fp != NULL;
}

inline const FS_String &FS_File::GetPath() const
{
    return _path;
}

inline const FS_String &FS_File::GetFileName() const
{
    return _fileName;
}

inline Int64 FS_File::GetSize() const
{
    return _fileSize;
}

inline FS_File::operator bool() const
{
    return _fp != NULL;
}

inline FS_File::operator FILE *()
{
    return _fp;
}

inline FS_File::operator const FILE *() const
{
    return _fp;
}
#pragma endregion

FS_NAMESPACE_END

#endif
