/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @author: Eric Yonng<120453674@qq.com>
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
OBJ_POOL_CREATE_ANCESTOR_IMPL(FS_IniFile, __DEF_OBJ_POOL_OBJ_NUM__);

FS_IniFile::FS_IniFile()
    :_isDirtied(false)
    ,_maxLine(0)
{

}

FS_IniFile::~FS_IniFile()
{
//#ifndef _WIN32
    _UpdateIni();
//#endif
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

void FS_IniFile::Clear()
{
    _filePath.Clear();
    _isDirtied = false;
    _maxLine = 0;
    _lineRefContent.clear();
    _segOrKeyRefLine.clear();
    _segmentRefKeyValues.clear();
    _segmentRefMaxValidLine.clear();
}

void FS_IniFile::Flush()
{
    if (_filePath.empty())
        return;

    if (!FS_FileUtil::IsFileExist(_filePath.c_str()))
        return;

    _UpdateIni(true);
}

const FS_String &FS_IniFile::GetPath() const
{
    return _filePath;
}

const FS_String &FS_IniFile::GetFileContent() const
{
    return _fileContent;
}

void FS_IniFile::Lock()
{
    _lock.Lock();
}

void FS_IniFile::Unlock()
{
    _lock.Unlock();
}

bool FS_IniFile::ReadStr(const char *segmentName, const char *keyName, const char *defaultStr, FS_String &strOut)
{
    if(UNLIKELY(_filePath.empty()))
        return false;

    return _ReadStr(segmentName, keyName, defaultStr, strOut);
}

Int64 FS_IniFile::ReadInt(const char *segmentName, const char *keyName, Int64 defaultInt)
{
    FS_String cache;
    if(_ReadStr(segmentName, keyName, "", cache) && !cache.empty())
        return StringUtil::StringToInt64(cache.c_str());

    return defaultInt;
}

UInt64 FS_IniFile::ReadUInt(const char *segmentName, const char *keyName, UInt64 defaultInt)
{
    FS_String cache;
    if(_ReadStr(segmentName, keyName, "", cache) && !cache.empty())
        return StringUtil::StringToUInt64(cache.c_str());

    return defaultInt;
}

bool FS_IniFile::WriteStr(const char *segmentName, const char *keyName, const char *wrStr)
{
    if(UNLIKELY(_filePath.empty()))
        return false;

    return _WriteStr(segmentName, keyName, wrStr);
}

bool FS_IniFile::HasCfgs(const char *segmentName) const
{
    return  _segmentRefKeyValues.find(segmentName) != _segmentRefKeyValues.end();
}

bool FS_IniFile::HasCfgs() const
{
    return !_segmentRefKeyValues.empty();
}

bool FS_IniFile::ChangeLineBetweenSegs()
{
    // 打开并清空文件
    auto fp = FS_FileUtil::OpenFile(_filePath.c_str(), false, "w");
    if(!fp)
        return false;

    FS_String lineData;
    FS_String seg;
    for(auto iterLineData = _lineRefContent.begin(); iterLineData != _lineRefContent.end(); ++iterLineData)
    {
        lineData.Clear();
        lineData = iterLineData->second + FS_String(IniFileDefs::_changeLineFlag);
        if(IniFileMethods::IsSegment(lineData, seg))
            FS_FileUtil::WriteFile(*fp, FS_String(IniFileDefs::_changeLineFlag));
        FS_FileUtil::WriteFile(*fp, lineData);
    }

    FS_FileUtil::FlushFile(*fp);
    FS_FileUtil::CloseFile(*fp);

    // 重新load配置
    _isDirtied = false;
    return _LoadAllCfgs();
}

bool FS_IniFile::WriteFileHeaderAnnotation(const FS_String &content)
{
    // 打开并清空文件
    auto fp = FS_FileUtil::OpenFile(_filePath.c_str(), false, "w");
    if(!fp)
        return false;
    
    // 1.获取第一个segment的行号在segment前插入
    const Int32 firstSegLine = _GetSegmentLineByLoop(1);
    // 2.分离出多行数据
    auto multiLineContent = content.Split(IniFileDefs::_changeLineFlag);
    // 3.每一行添加注释符
    const Int32 contentLine = static_cast<Int32>(multiLineContent.size());
    for(Int32 i = 0; i < contentLine; ++i)
        multiLineContent[i] = FS_String(IniFileDefs::_annotationFlag) + multiLineContent[i];

    // 4.多行内容拼接成一个string对象
    FS_String willWriteContent;
    for(Int32 i = 0; i < contentLine; ++i)
        willWriteContent << multiLineContent[i] << FS_String(IniFileDefs::_changeLineFlag);

    return _InsertNewLineData(firstSegLine, willWriteContent);
}

bool FS_IniFile::_Init()
{
    // 读取所有配置内容
    if(!_LoadAllCfgs())
        return false;

    return true;
}

bool FS_IniFile::_LoadAllCfgs()
{
    // 刷新文件
    _UpdateIni();

    auto fp = FS_FileUtil::OpenFile(_filePath.c_str());
    if(!fp)
        return false;

    // 解析seg,key value
    // 读取每一行，遇到[表示新的segment
    // 一行一行的读取，读到读不出来为止
    _maxLine = 0;
    _lineRefContent.clear();
    _segOrKeyRefLine.clear();
    _segmentRefKeyValues.clear();
    _segmentRefMaxValidLine.clear();

    std::map<FS_String, FS_String> *curKeyValues = NULL;
    FS_String curSegment;
    while(true)
    {
        FS_String lineData;
        auto cnt = FS_FileUtil::ReadOneLine(*fp, lineData);
        if(cnt || !FS_FileUtil::IsEnd(*fp))
            _lineRefContent.insert(std::make_pair(++_maxLine, lineData));
        if(!cnt)
        {
            if(FS_FileUtil::IsEnd(*fp))
                break;
            continue;
        }

        // 粗提取
        Int32 contentType = IniFileDefs::ContentType::Invalid;
        FS_String validContent;
        auto hasValidData = IniFileMethods::ExtractValidRawData(lineData, contentType, validContent);
        if(hasValidData)
            _OnReadValidData(validContent, contentType, _maxLine, curSegment, curKeyValues);
    }

    // 更新配置文件内存缓存
    _UpdateFileMemCache(*fp);
    FS_FileUtil::CloseFile(*fp);

    return true;
}

bool FS_IniFile::_ReadStr(const char *segmentName, const char *keyName, const char *defaultStr, FS_String &strOut)
{
    auto iterKeyValue = _segmentRefKeyValues.find(segmentName);
    if(iterKeyValue == _segmentRefKeyValues.end())
        return false;

    auto &keyValue = iterKeyValue->second;
    auto iterValue = keyValue.find(keyName);
    if(iterValue == keyValue.end())
        return false;

    if(iterValue->second.empty())
    {
        strOut = defaultStr;
    }
    else
    {
        strOut = iterValue->second;
    }

    return true;
}

bool FS_IniFile::_WriteStr(const char *segmentName, const char *keyName, const char *wrStr)
{
    // 寻找段 没有便创建段，并追加行号
    auto iterKeyValue = _segmentRefKeyValues.find(segmentName);
    if(iterKeyValue == _segmentRefKeyValues.end())
    {
        iterKeyValue = _segmentRefKeyValues.insert(std::make_pair(segmentName, std::map<FS_String, FS_String>())).first;
        _segOrKeyRefLine.insert(std::make_pair(segmentName, ++_maxLine));

        FS_String segContent = FS_String(IniFileDefs::_leftSegmentFlag);
        segContent << segmentName << FS_String(IniFileDefs::_rightSegmentFlag);
        _lineRefContent.insert(std::make_pair(_maxLine, segContent));
        _isDirtied = true;
    }

    // 寻找键 没有便创建键值对 并追加行号 并更新数据
    auto &keyValue = iterKeyValue->second;
    auto iterValue = keyValue.find(keyName);
    if(iterValue == keyValue.end())
    {
        const auto &segStr = iterKeyValue->first;
        Int32 segMaxValidLine = _GetSegmentKeyValueMaxValidLine(segStr);
        if(segMaxValidLine < 0)
        {// 段不存在
            return false;
        }

        // 更新keyvalue
        keyValue.insert(std::make_pair(keyName, wrStr));
        FS_String keyValueContent = keyName;
        keyValueContent << FS_String(IniFileDefs::_keyValueJoinerFlag) << wrStr;

        // 为key创建段中唯一索引
        FS_String segKey = segStr + FS_String(IniFileDefs::_segKeyJoinerFlag) + keyName;

        // 插入新的行数据
        return _InsertNewLineData(++segMaxValidLine, segStr, keyName, wrStr);
    }
    else
    {
        if(iterValue->second != wrStr)
        {
            // 新值
            iterValue->second = wrStr;

            // 新的键值对所在行
            FS_String segKey = iterKeyValue->first + FS_String(IniFileDefs::_segKeyJoinerFlag) + keyName;
            auto iterLine = _segOrKeyRefLine.find(segKey);

            auto iterContent = _lineRefContent.find(iterLine->second);
            auto &content = iterContent->second;

            // 分离
            const auto &splitStr = content.Split(IniFileDefs::_annotationFlag, 1);

            FS_String comments;
            if(splitStr.size() >= 2)
            {// 含有注释
                comments << FS_String(IniFileDefs::_annotationFlag);
                comments << splitStr[1];
            }

            content = keyName;
            content << FS_String(IniFileDefs::_keyValueJoinerFlag) << iterValue->second;
            if(!comments.empty())
                content << IniFileDefs::_annotationSpaceStr << comments;

            _isDirtied = true;
        }
    }

    // 更新文件
    _UpdateIni(true);

    return true;
}

bool FS_IniFile::_InsertNewLineData(Int32 line, const FS_String &segment, const FS_String &key,  const FS_String &value)
{
    // 段不存在不可插入
    auto iterKeyValue = _segmentRefKeyValues.find(segment);
    if(iterKeyValue == _segmentRefKeyValues.end())
        return false;

    FS_String keyValue;
    IniFileMethods::MakeKeyValuePairStr(key, value, keyValue);
    _InsertNewLineData(line, keyValue);
    return true;
}

bool FS_IniFile::_InsertNewLineData(Int32 line, const FS_String &content)
{
    auto iterContent = _lineRefContent.find(line);
    if(iterContent != _lineRefContent.end())
    {// 已存在则需要挪动行数据
        FS_String swapStr, frontStr;
        frontStr = iterContent->second;
        for(++iterContent; iterContent != _lineRefContent.end(); ++iterContent)
        {
            swapStr = iterContent->second;
            iterContent->second = frontStr;
            frontStr = swapStr;
        }

        // 最后一个节点挪到新的行
        ++_maxLine;
        _lineRefContent.insert(std::make_pair(_maxLine, frontStr));
        auto iterToModify = _lineRefContent.find(line);
        iterToModify->second = content;
    }
    else
    {// 不存在则不用挪直接插入
        _lineRefContent.insert(std::make_pair(line, content));
    }

    // 更新最大行号
    if(_maxLine < line)
        _maxLine = line;

    _isDirtied = true;
    _UpdateIni();

    // 重新加载配置
    return _LoadAllCfgs();
}

void FS_IniFile::_UpdateIni(bool isUpdateMemCache)
{
    if(!_isDirtied)
        return;

    _isDirtied = false;

    // 打开并清空文件
    auto fp = FS_FileUtil::OpenFile(_filePath.c_str(), false, "w");
    FS_String lineData;
    for(auto iterLineData = _lineRefContent.begin(); iterLineData != _lineRefContent.end(); ++iterLineData)
    {
        lineData.Clear();
        lineData = iterLineData->second + FS_String(IniFileDefs::_changeLineFlag);
        FS_FileUtil::WriteFile(*fp, lineData);
    }

    FS_FileUtil::FlushFile(*fp);

    // 更新内存缓存
    if (isUpdateMemCache)
        _UpdateFileMemCache(*fp);

    FS_FileUtil::CloseFile(*fp);
}

UInt64 FS_IniFile::_UpdateFileMemCache(FILE &fp)
{
    // 缓存文件内容
    _fileContent.Clear();
    FS_FileUtil::ResetFileCursor(fp);
    return FS_FileUtil::ReadFile(fp, _fileContent);
}

void FS_IniFile::_OnReadValidData(const FS_String &validContent
                                  , Int32 contentType
                                  , Int32 line
                                  , FS_String &curSegment
                                  , std::map<FS_String, FS_String> *&curKeyValues)
{
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
        {
            _segOrKeyRefLine.insert(std::make_pair(curSegment, line));
        }
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
                    FS_String segKey = curSegment + FS_String(IniFileDefs::_segKeyJoinerFlag) + key;
                    auto iterLine = _segOrKeyRefLine.find(segKey);
                    if(iterLine != _segOrKeyRefLine.end())
                        perror("segOrKey repeated");

                    _segOrKeyRefLine.insert(std::make_pair(segKey, line));
                }

                // 更新键值对最大有效行号
                auto iterMaxValidLine = _segmentRefMaxValidLine.find(curSegment);
                if(iterMaxValidLine == _segmentRefMaxValidLine.end())
                    iterMaxValidLine = _segmentRefMaxValidLine.insert(std::make_pair(curSegment, 0)).first;

                if(iterMaxValidLine->second < line)
                    iterMaxValidLine->second = line;
            }
        }
    }
}

Int32 FS_IniFile::_GetSegmentKeyValueMaxValidLine(const FS_String &segment) const
{
    // 若该段是空的则以segment所在行作为最大行号
    auto iterLine = _segmentRefMaxValidLine.find(segment);
    if(iterLine == _segmentRefMaxValidLine.end())
    {
        auto iterSegLine = _segOrKeyRefLine.find(segment);
        if(iterSegLine == _segOrKeyRefLine.end())
        {// 该段不存在
            return -1;
        }

        return iterSegLine->second;
    }

    return iterLine->second;
}

Int32 FS_IniFile::_GetSegmentLineByLoop(Int32 index) const
{
    FS_String lineData;
    FS_String seg;
    Int32 realLine = 1; // 找不到任何segment则从第1行插入
    Int32 loopCnt = 0;
    for(auto iterLineData = _lineRefContent.begin(); iterLineData != _lineRefContent.end(); ++iterLineData)
    {
        lineData.Clear();
        lineData = iterLineData->second;
        if(IniFileMethods::IsSegment(lineData, seg))
        {
            ++loopCnt;
            realLine = iterLineData->first;

            if(loopCnt >= index)
                break;
        }
    }

    return realLine;
}

FS_NAMESPACE_END


