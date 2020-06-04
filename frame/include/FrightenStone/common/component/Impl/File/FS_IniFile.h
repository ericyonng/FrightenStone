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
 * @file  : FS_IniFile.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 *          segment : 指段名,以[]括起来的字符
 *          key     : 指段下的键名,
 *          value   : 键对应的值
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_File_FS_IniFile_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_File_FS_IniFile_H__
#pragma once

#include <FrightenStone/exportbase.h>
#include "FrightenStone/common/basedefs/DataType/DataType.h"
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include <FrightenStone/common/component/Impl/FS_String.h>
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class FS_String;
class Locker;
class BASE_EXPORT FS_IniFile
{
    OBJ_POOL_CREATE_ANCESTOR(FS_IniFile);
public:
    FS_IniFile();
    virtual ~FS_IniFile();

public:
    bool Init(const char *path);
    void Clear();
    void Flush();
    const FS_String &GetPath() const;
    const FS_String &GetFileContent() const;

    void Lock();
    void Unlock();

    bool ReadStr(const char *segmentName, const char *keyName, const char *defaultStr, FS_String &strOut);
    Int64 ReadInt(const char *segmentName, const char *keyName, Int64 defaultInt);
    UInt64 ReadUInt(const char *segmentName, const char *keyName, UInt64 defaultInt);
    bool WriteStr(const char *segmentName, const char *keyName, const char *wrStr);

    bool HasCfgs(const char *segmentName) const;
    bool HasCfgs() const;
    bool ChangeLineBetweenSegs();
    bool WriteFileHeaderAnnotation(const FS_String &content);   // 内容只会在第一个segment的位置插入

private:
    bool _Init();
    bool _LoadAllCfgs();

    bool _ReadStr(const char *segmentName, const char *keyName, const char *defaultStr, FS_String &strOut);
    bool _WriteStr(const char *segmentName, const char *keyName, const char *wrStr);

    // 插入新行数据
    bool _InsertNewLineData(Int32 line, const FS_String &segment, const FS_String &key, const FS_String &value); // 会重新加载配置，容器迭代器全部失效
    bool _InsertNewLineData(Int32 line, const FS_String &content); // 向某一行插入任意数据
    // 更新配置
    void _UpdateIni(bool isUpdateMemCache = false);
    // 更新内存缓存
    UInt64 _UpdateFileMemCache(FILE &fp);

    // 读取到有效的数据
    void _OnReadValidData(const FS_String &validContent
                          , Int32 contentType
                          , Int32 line
                          , FS_String &curSegment
                          , std::map<FS_String, FS_String> *&curKeyValues);

    // 段的所包含的键值对的最大行
    Int32 _GetSegmentKeyValueMaxValidLine(const FS_String &segment) const;  // 返回-1该段不存在
    Int32 _GetSegmentLineByLoop(Int32 index) const; // 获取第index个segment的行号

private:
    Locker _lock;
    FS_String  _filePath;
    FS_String _fileContent; // 配置文件内容

    bool _isDirtied;
    Int32 _maxLine;                                 // 行号最小值1
    std::map<Int32, FS_String> _lineRefContent;     // 每一行的元数据,行号最小值从1开始
    std::map<FS_String, Int32> _segOrKeyRefLine;    // seg对应的行号或者seg-key所在的行号 例如seg, seg-key
    std::map<FS_String, std::map<FS_String, FS_String>> _segmentRefKeyValues;
    std::map<FS_String, Int32> _segmentRefMaxValidLine;        // key：段， value:该段有效的最大行号
};

FS_NAMESPACE_END

#endif
