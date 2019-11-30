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
#include "FrightenStone/common/asyn/Lock/Impl/Locker.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/component/Impl/File/FS_IniFile.h"
#include "FrightenStone/common/assist/utils/Impl/FS_FileUtil.h"
#pragma region ini file io
#include<io.h>
#pragma endregion

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_DEF_IMPL(FS_IniFile, __DEF_OBJ_POOL_OBJ_NUM__);

FS_IniFile::FS_IniFile()
#ifndef _WIN32
    :_fp(NULL)
    ,_isDirtied(false)
#endif
{

}

FS_IniFile::~FS_IniFile()
{
#ifndef _WIN32
    FS_FileUtil::CloseFile(*_fp);
    _fp = NULL;
#endif
}

bool FS_IniFile::Init(const char *path)
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

    return _Init();
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
#ifdef _WIN32
    if(UNLIKELY(_filePath.empty()))
        return NULL;

    // read string
    GetPrivateProfileString(segmentName, keyName, defaultStr, outStr, outSize, _filePath.c_str());
    return outStr;
#else
    return _ReadStr(segmentName, keyName, defaultStr, outStr, outSize);
#endif

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

bool FS_IniFile::_Init()
{
#ifndef _WIN32
    // 读取所有配置内容
    if(!_LoadAllCfgs())
        return false;
#endif

    return true;
}

bool FS_IniFile::_LoadAllCfgs()
{
#ifndef _WIN32
    if(_fp)
    {
        _UpdateIni();
        FS_FileUtil::CloseFile(*_fp);
        _fp = NULL;
    }

    _fp = FS_FileUtil::OpenFile(_filePath.c_str());
    if(!_fp)
        return false;

    FS_String content;
    FS_FileUtil::ReadFile(*_fp, content);

    // 解析seg,key value
    // 读取每一行，遇到[表示新的segment
    FS_FileUtil::ReadOneLine()
#endif

    return true;
}

const char *FS_IniFile::_ReadStr(const char *segmentName, const char *keyName, const char *defaultStr, char *&outStr, UInt16 outSize)
{
#ifndef _WIN32
    auto result = _ReadFromDict(segmentName, keyName, defaultStr, outStr, outSize);
    if(!result)
        _Init();

    return _ReadFromDict(segmentName, keyName, defaultStr, outStr, outSize);
#endif

    return NULL;
}

const char *FS_IniFile::_ReadFromDict(const char *segmentName, const char *keyName, const char *defaultStr, char *&outStr, UInt16 outSize)
{
#ifndef _WIN32
    auto iterKeyValues = _segmentRefKeyValues.find(segmentName);
    if(iterKeyValues == _segmentRefKeyValues.end())
    {// 从配置数据中读取
        auto &content = _content.GetRaw();
        auto pos = content.find(segmentName);
        if(iterSegment == std::string::npos)
            return NULL;

        iterKeyValues = _segmentRefKeyValues.insert(std::make_pair(segmentName, std::map<FS_String, FS_String>())).first;
    }

    auto &keyValues = iterKeyValues->second;
    auto iterValue = keyValues.find(keyName);
    if(iterValue == keyValues.end())
    {// 从配置数据中读取
        auto &content = _content.GetRaw();
        auto pos = content.find(segmentName);
        if(pos == std::string::npos)
            return NULL;

        // 读取到下个segment之前的内容
    }

#endif
        return NULL;
}

bool FS_IniFile::_WriteStr()
{
#ifndef _WIN32

#endif

    return true;
}

void FS_IniFile::_UpdateIni()
{
#ifndef _WIN32
    if(!_isDirtied)
        return;

    _isDirtied = false;
    FS_FileUtil::FlushFile(*_fp);
#endif

}
FS_NAMESPACE_END


