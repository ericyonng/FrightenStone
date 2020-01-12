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
 * @file  : LogCaches.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/06/27
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Src_Common_Log_Defs_LogCaches_H__
#define __Frame_Src_Common_Log_Defs_LogCaches_H__
#pragma once

#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "frame/src/common/log/Defs/LogDefs.h"

FS_NAMESPACE_BEGIN

struct LogDataCache
{
    OBJ_POOL_CREATE_DEF(LogDataCache);
    LogDataCache();
    ~LogDataCache();

    std::list<LogData *> *_cache;    // 日志数据队列
};

struct LogData;
class LogFile;

class LogCaches
{
    OBJ_POOL_CREATE_DEF(LogCaches);
public:
    LogCaches();
    virtual ~LogCaches();

public:
    std::list<LogData *> *_swapCache;
    std::list<LogData *> *_cache4RealLog;

    // logfile
    LogFile *_logFile{NULL};
};

FS_NAMESPACE_END

#endif
