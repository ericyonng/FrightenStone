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
 * @file  : LogDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/06/26
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Src_Common_Log_Defs_LogDefs_H__
#define __Frame_Src_Common_Log_Defs_LogDefs_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "frame/src/common/log/Defs/CreateLogFileMacro.h"
#include "FrightenStone/common/log/Interface/ILog.h"

FS_NAMESPACE_BEGIN

// 请在LogFileType.h添加响应的枚举，
// 并在virtual Int32 FS_Log::_GetLogFileIndex(Int32 logTypeEnum); 填充响应的实现
namespace LogDefs
{
    template <int i>
    struct LogInitHelper
    {
        static bool InitLog(ILog *logMgr) { return true; }
    };

    template<int i>
    struct ExcludeCommentHelper
    {
        enum { realvalue = ExcludeCommentHelper<i - 1>::realvalue };
    };

    template<>
    struct ExcludeCommentHelper<0>
    {
        enum { realvalue = 0 };
    };

    // 定义日志文件
    DECLEAR_GENERAL_LOG_BEGIN()
    GENERAL_ADDLOG(SYSLOG, crash)           // 崩溃
    GENERAL_ADDLOG(SYSLOG, details)         // 具体日志
    GENERAL_ADDLOG(SYSLOG, mempool)         // 内存池情况日志
    GENERAL_ADDLOG(SYSLOG, objpool)         // 对象池情况日志
    GENERAL_ADDLOG(SYSLOG, net)             // 网络层
    GENERAL_ADDLOG(SYSLOG, sys)             // 系统信息（用于分析）
    GENERAL_ADDLOG(SYSLOG, Custom)          // 自定义的日志格式内容
    DECLEAR_GENERAL_LOG_END()
};
 

FS_NAMESPACE_END

#include "frame/src/common/log/Defs/LogDefsImpl.h"

#define ASSERTLOG(x) (g_Log->e<decltype(x)>(_LOGFMT_("%s"), #x), ASSERT(!#x))

#endif
