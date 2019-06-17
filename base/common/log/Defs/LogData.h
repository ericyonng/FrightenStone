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
 * @file  : LogData.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/13
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Log_Defs_LogData_H__
#define __Base_Common_Log_Defs_LogData_H__
#pragma once
#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN
class FS_String;
class Time;

class BASE_EXPORT LogLevel
{
public:
    enum LevelEnums
    {
        Begin = 0,          // 开始
        Warning = Begin,
        Debug,
        Info,
        Error,
        End,
    };
};

struct BASE_EXPORT LogData
{
    FS_String _content;             // 日志内容
    Time _logTime;                  // 时间
    FS_String _className;           // 类名
    FS_String _funcName;            // 函数名
    Int32 _line;                    // 写日志行数
    Int32 _level;                   // 日志级别
};

FS_NAMESPACE_END

#endif
