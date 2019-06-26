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
#ifndef __Base_Common_Log_Defs_LogCaches_H__
#define __Base_Common_Log_Defs_LogCaches_H__
#pragma once

#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

struct LogData;
class LogFile;

class LogCaches
{
public:
    LogCaches();
    virtual ~LogCaches();

public:
    std::map<Int32, std::list<LogData *> *> _logDatasCache;                 // »’÷æƒ⁄»›ª∫≥Â key:fileindex
    std::map<Int32, std::list<LogData *> *>::iterator _iterToWrite;
    bool _hasLog{false};

    // logfile
    LogFile *_logFile{NULL};
};

FS_NAMESPACE_END

#endif
