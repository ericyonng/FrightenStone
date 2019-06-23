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
 * @file  : ILog.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Log_Interface_ILog_H__
#define __Base_Common_Log_Interface_ILog_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/log/Defs/LogData.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT ILog
{
public:
    ILog();
    virtual ~ILog();

    static ILog *InitModule(const Byte8 *processName);
    virtual void FinishModule() = 0;

    // json日志

    // 普通日志
    template<typename ObjType, typename... Args>
    void i(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename ObjType, typename... Args>
    void d(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename ObjType, typename... Args>
    void w(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename ObjType, typename... Args>
    void e(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename ObjType, typename... Args>
    void crash(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);

    /* 功能函数 */
    template<typename ObjType>
    void InstallLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*func)(const LogData *logData));
    virtual void InstallLogHookFunc(Int32 level, IDelegatePlus<void, const LogData *> *delegate) = 0;

protected:
    virtual LogData *_BuildLogData(const Byte8 *className, const Byte8 *funcName, const FS_String &content, Int32 codeLine, Int32 logLevel) = 0;
    virtual void _WriteLog(Int32 fileUniqueIndex, LogData *logData) = 0;
};

FS_NAMESPACE_END

#include "base/common/log/Interface/ILogImpl.h"

#endif
