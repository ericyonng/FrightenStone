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
#include "FrightenStone/common/component/Impl/File/Defs/IniFileDefs.h"
#include "FrightenStone/common/assist/utils/utils.h"

#ifdef _WIN32
#pragma region ini file io
#include<io.h>
#pragma endregion
#endif

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_DEF_IMPL(FS_IniFile, __DEF_OBJ_POOL_OBJ_NUM__);

FS_IniFile::FS_IniFile()
// #ifndef _WIN32
    :_isDirtied(false)
    ,_maxLine(0)
    , _cache{0}
// #endif
{

}

FS_IniFile::~FS_IniFile()
{
#ifndef _WIN32
    _UpdateIni();
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

    char *ptr = _cache;
    _cache[0] = 0;
    if(_ReadStr(segmentName, keyName, "", ptr, sizeof(_cache)))
        return static_cast<UInt32>(StringUtil::StringToUInt64(_cache));

    return static_cast<UInt32>(defaultInt);
}

bool FS_IniFile::WriteStr(const char *segmentName, const char *keyName, const char *wrStr)
{
#ifdef _WIN32
    if(UNLIKELY(_filePath.empty()))
        return false;

    return WritePrivateProfileString(segmentName, keyName, wrStr, _filePath.c_str());
#else
    return _WriteStr(segmentName, keyName, wrStr);
#endif
}

bool FS_IniFile::ReadAllKeyValueOfSection(const char *segmentName, char *&outStr, UInt16 outSize)
{
    return  GetPrivateProfileSection(segmentName, outStr, outSize, _filePath.c_str()) != 0;
}

bool FS_IniFile::_Init()
{
// #ifndef _WIN32
    // 读取所有配置内容
    if(!_LoadAllCfgs())
        return false;
// #endif

    return true;
}

bool FS_IniFile::_LoadAllCfgs()
{
#ifndef _WIN32
    _UpdateIni();

    auto fp = FS_FileUtil::OpenFile(_filePath.c_str());
    if(!fp)
        return false;

    // 解析seg,key value
    // 读取每一行，遇到[表示新的segment
    // 一行一行的读取，读到读不出来为止
    _maxLine = 0;
    _lineRefContent.clear();
    std::map<FS_String, FS_String> *curKeyValues = NULL;
    FS_String curSegment;
    while(true)
    {
        FS_String lineData;
        auto cnt = FS_FileUtil::ReadOneLine(*fp, lineData);
        if(!cnt)
            break;

        _lineRefContent.insert(std::make_pair(++_maxLine, lineData));

        // 粗提取
        Int32 contentType = IniFileDefs::ContentType::Invalid;
        FS_String validContent;
        auto hasValidData = IniFileMethods::ExtractValidRawData(lineData, contentType, validContent);
        if(hasValidData)
            _OnReadValidData(validContent, contentType, _maxLine, curSegment, curKeyValues);
    }
#endif

    return true;
}

const char *FS_IniFile::_ReadStr(const char *segmentName, const char *keyName, const char *defaultStr, char *&outStr, UInt16 outSize)
{
//#ifndef _WIN32
    auto iterKeyValue = _segmentRefKeyValues.find(segmentName);
    if(iterKeyValue == _segmentRefKeyValues.end())
        return NULL;

    auto &keyValue = iterKeyValue->second;
    auto iterValue = keyValue.find(keyName);
    if(iterValue == keyValue.end())
        return NULL;

    if(iterValue->second.empty())
    {
        const size_t maxLen = std::min<size_t>(strlen(defaultStr), outSize - 1);
        ::memcpy(outStr, defaultStr, maxLen);
        outStr[maxLen + 1] = 0;
    }
    else
    {
        const size_t maxLen = std::min<size_t>(strlen(iterValue->second.c_str()), outSize - 1);
        iterValue->second.CopyTo(outStr, outSize, static_cast<Int32>(maxLen), 0);
    }

    return outStr;
//#endif

    return NULL;
}

bool FS_IniFile::_WriteStr(const char *segmentName, const char *keyName, const char *wrStr)
{
// #ifndef _WIN32
    // 寻找段 没有便创建段，并追加行号
    auto iterKeyValue = _segmentRefKeyValues.find(segmentName);
    if(iterKeyValue == _segmentRefKeyValues.end())
    {
        iterKeyValue = _segmentRefKeyValues.insert(std::make_pair(segmentName, std::map<FS_String, FS_String>())).first;
        _segOrKeyRefLine.insert(std::make_pair(segmentName, ++_maxLine));

        FS_String segContent = "[";
        segContent << segmentName << "]";
        _lineRefContent.insert(std::make_pair(_maxLine, segContent));
        _isDirtied = true;
    }

    // 寻找键 没有便创建键值对 并追加行号 并更新数据
    auto &keyValue = iterKeyValue->second;
    auto iterValue = keyValue.find(keyName);
    if(iterValue == keyValue.end())
    {
        keyValue.insert(std::make_pair(keyName, wrStr));

        FS_String segKey = iterKeyValue->first + "-" + keyName;
        _segOrKeyRefLine.insert(std::make_pair(segKey, ++_maxLine));

        FS_String keyValueContent = keyName;
        keyValueContent << "=" << wrStr;
        _lineRefContent.insert(std::make_pair(_maxLine, keyValueContent));
        _isDirtied = true;
    }
    else
    {
        if(iterValue->second != wrStr)
        {
            iterValue->second = wrStr;

            FS_String segKey = iterKeyValue->first + "-" + keyName;
            auto iterLine = _segOrKeyRefLine.find(segKey);

            auto iterContent = _lineRefContent.find(iterLine->second);
            auto &content = iterContent->second;
            content = keyName;
            content << "=" << iterValue->second;
            _isDirtied = true;
        }
    }

    // 更新文件
    _UpdateIni();

    // 寻找键 没有便创建键值对 并追加行号
    // 写入 _segmentRefKeyValues _segOrKeyRefLine _lineRefContent（写入时段需要添加[]符号）
    // 标脏
    // 更新文件
// #endif

    return true;
}

void FS_IniFile::_UpdateIni()
{
#ifndef _WIN32
    if(!_isDirtied)
        return;

    _isDirtied = false;

    // 打开并清空文件
    auto fp = FS_FileUtil::OpenFile(_filePath.c_str(), false, "w");
    FS_String lineData;
    for(auto iterLineData = _lineRefContent.begin(); iterLineData != _lineRefContent.end(); ++iterLineData)
    {
        lineData.Clear();
        lineData = iterLineData->second + "\n";
        FS_FileUtil::WriteFile(*fp, lineData);
    }

    FS_FileUtil::FlushFile(*fp);
    FS_FileUtil::CloseFile(*fp);
#endif

}

void FS_IniFile::_OnReadValidData(const FS_String &validContent
                                  , Int32 contentType
                                  , Int32 line
                                  , FS_String &curSegment
                                  , std::map<FS_String, FS_String> *&curKeyValues)
{
#ifndef _WIN32
    if(contentType == IniFileDefs::ContentType::Segment)
    {// 是段
        curSegment = validContent;
        auto iterKeyValues = _segmentRefKeyValues.find(curSegment);
        if(iterKeyValues == _segmentRefKeyValues.end())
            iterKeyValues = _segmentRefKeyValues.insert(std::make_pair(curSegment, std::map<FS_String, FS_String>())).first;
        curKeyValues = &(iterKeyValues->second);

        // 记录段所在的行号
        auto iterLine = _segOrKeyRefLine.find(curSegment);
        if(iterLine == _segOrKeyRefLine.end())
            _segOrKeyRefLine.insert(std::make_pair(curSegment, line));
    }
    else if(contentType == IniFileDefs::ContentType::KeyValue)
    {// 是键值对
        if(!curSegment.empty())
        {// 需要先有段再有键值对
            FS_String key;
            FS_String value;
            if(IniFileMethods::SpiltKeyValue(validContent, key, value))
            {
                auto iterValue = curKeyValues->find(key);
                if(iterValue == curKeyValues->end())
                {
                    curKeyValues->insert(std::make_pair(key, value));

                    // 记录键值对所在的行号
                    FS_String segKey = curSegment + "-" + key;
                    auto iterLine = _segOrKeyRefLine.find(segKey);
                    if(iterLine != _segOrKeyRefLine.end())
                        perror("segOrKey repeated");

                    _segOrKeyRefLine.insert(std::make_pair(segKey, line));
                }
            }
        }
    }
#endif

}


FS_NAMESPACE_END


