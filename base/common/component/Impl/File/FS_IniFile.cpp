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
 * @file  : FS_IniFile.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/asyn/Lock/Impl/Locker.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/component/Impl/File/FS_IniFile.h"
#include "base/common/assist/utils/Impl/FS_FileUtil.h"
#pragma region ini file io
#include<io.h>
#pragma endregion

FS_NAMESPACE_BEGIN

MEM_POOL_CREATE_IMPL_DEF(FS_IniFile);

FS_IniFile::FS_IniFile()
{

}

FS_IniFile::~FS_IniFile()
{

}

bool FS_IniFile::SetPath(const char *path)
{
    // detect if file is existed
    if(UNLIKELY(!FS_FileUtil::IsFileExist(path)))
    {
        auto fp = FS_FileUtil::OpenFile(path, true);
        if(UNLIKELY(!fp))
            return false;

        FS_FileUtil::CloseFile(*fp);
    }

    _filePath = path;

    return true;
}

const FS_String &FS_IniFile::GetPath() const
{
    return _filePath;
}

void FS_IniFile::Lock()
{
    _lock.Lock();
}

void FS_IniFile::Unlock()
{
    _lock.Unlock();
}

const char * FS_IniFile::ReadStr(const char *segmentName, const char *keyName, const char *defaultStr, char *&outStr, UInt16 outSize)
{
    if(UNLIKELY(_filePath.empty()))
        return NULL;

    // read string
    GetPrivateProfileString(segmentName, keyName, defaultStr, outStr, outSize, _filePath.c_str());
    return outStr;
}

UInt32 FS_IniFile::ReadInt(const char *segmentName, const char *keyName, Int32 defaultInt)
{
    if(UNLIKELY(_filePath.empty()))
        return 0;

    return GetPrivateProfileInt(segmentName, keyName, defaultInt, _filePath.c_str());
}

bool FS_IniFile::WriteStr(const char *segmentName, const char *keyName, const char *wrStr)
{
    if(UNLIKELY(_filePath.empty()))
        return false;

    return WritePrivateProfileString(segmentName, keyName, wrStr, _filePath.c_str());
}

bool FS_IniFile::ReadAllKeyValueOfSection(const char *segmentName, char *&outStr, UInt16 outSize)
{
    return  GetPrivateProfileSection(segmentName, outStr, outSize, _filePath.c_str()) != 0;
}
FS_NAMESPACE_END

