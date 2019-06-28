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

 // 函数与行号便利宏
#undef _FUNC_LINE_ARGS_
#define _FUNC_LINE_ARGS_ __FUNCTION__, __LINE__

FS_NAMESPACE_BEGIN

class BASE_EXPORT ILog
{
public:
    ILog();
    virtual ~ILog();

    static ILog *InitModule(const Byte8 *rootDir);
    virtual void FinishModule() = 0;

    // json日志

    // 
    // 详细日志details.log
    template<typename ObjType, typename... Args>
    void i(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename... Args>
    void i2(const char *className, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename ObjType, typename... Args>
    void d(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename... Args>
    void d2(const char *className, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename ObjType, typename... Args>
    void w(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename... Args>
    void w2(const char *className, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename ObjType, typename... Args>
    void e(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename... Args>
    void e2(const char *className, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);

    // crash日志
    template<typename ObjType, typename... Args>
    void crash(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename... Args>
    void crash2(const char *className, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    // 网络日志
    template<typename ObjType, typename... Args>
    void net(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename... Args>
    void net2(const char *className, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    // 内存泄漏日志
    template<typename ObjType, typename... Args>
    void memleak(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);
    template<typename... Args>
    void memleak2(const char *className, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args);

    /* 功能函数 */
    // hook与具体类型有关
    template<typename ObjType>
    void InstallLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*func)(const LogData *logData));
    // 与具体类型无关hook
    virtual void InstallLogHookFunc(Int32 level, IDelegatePlus<void, const LogData *> *delegate) = 0;
    // 创建日志文件调用 ！！！外部不可调用，由系统初始化时候调用
    virtual Int32 CreateLogFile(Int32 fileUnqueIndex, const char *logPath, const char *fileName) = 0;

protected:
    virtual void _WriteLog(Int32 level, Int32 fileUniqueIndex, LogData *logData) = 0;
};

FS_NAMESPACE_END

#include "base/common/log/Interface/ILogImpl.h"

#endif
