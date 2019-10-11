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
 * @file  : FS_Log.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 *          1.提供写日志接口（传入任意参数）
 *          2.分目录日志 crash, system, netlayerlog（协议信息）业务层日志等
 *          3.按照文件大小分文件
 *          4.按照问题的优先级分:warning,error,debug,info等
 *          5.支持json log 风格：
 *                              {
                                    time:(精确到微妙)fmt:1970-01-01 00:00:00.123456
                                    class name:
                                    function:
                                    line:
                                    level:
                                    content:
                                    {
                                        op:
                                        status:statusCode
                                        ecxceptioninfo:
                                        stackinfo:
                                    }
                                }
            6.日志是单线程日志
 */
#ifndef __Base_Common_Log_Impl_Log_H__
#define __Base_Common_Log_Impl_Log_H__
#pragma once

#include "base/common/log/Interface/ILog.h"
#include "base/common/log/Defs/LogData.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/asyn/asyn.h"
#include "base/common/log//Defs/LogDefs.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class LogFile;
class LogCaches;

class BASE_EXPORT FS_Log : public ILog
{
public:
    FS_Log();
    virtual ~FS_Log();

    /* 功能函数 */
    virtual void UnInstallLogHookFunc(Int32 level, const IDelegate<void, const LogData *> *delegate);
    virtual void UnInstallBeforeLogHookFunc(Int32 level, const IDelegate<void, LogData *> *delegate);
    virtual Int32 InitModule(const Byte8 *rootDirName = NULL);
    virtual void FinishModule();
    virtual void FlushAllFile();
    virtual Int32 CreateLogFile(Int32 fileUnqueIndex, const char *logPath, const char *fileName);
    
protected:
    /* 日志hook */
    virtual IDelegate<void, const LogData *> * _InstallLogHookFunc(Int32 level, IDelegate<void, const LogData *> *delegate);    // 抽象的delegate
    virtual const IDelegate<void, LogData *> *_InstallBeforeLogHookFunc(Int32 level, IDelegate<void, LogData *> *delegate);
    // 日志相关
    virtual void _WriteLog(Int32 level, Int32 fileUniqueIndex, LogData *logData);

    // 控制台相关
    void _OutputToConsole(Int32 level, const FS_String &logStr);
    bool _IsAllowToConsole(Int32 level) const;
    void _SetConsoleColor(Int32 level);

    // 杂项
    void _ReadConfig();

    // 线程操作
private:
    void _OnThreadWriteLog(Int32 logFileIndex);

private:
    std::atomic_bool _isInit{false};
    std::atomic_bool _isFinish{false};

    FS_ThreadPool *_threadPool;                                                 // 线程池
    FS_String _rootDirName;                                                     // 进程名
    std::list<IDelegate<void, const LogData *> *> *_levelRefHook[LogLevel::End];         // 日志级别对应的hook
    std::list<IDelegate<void, LogData *> *> *_levelRefBeforeLogHook[LogLevel::End];         // 写日志前日志级别对应的hook 用于修改日志内容等
    Int32 _threadWorkIntervalMsTime;                                            // 日志线程工作间隔时间

    /* 日志文件内容 */
    ConditionLocker *_flielocker[LogDefs::LOG_QUANTITY];                    // 日志文件锁
    ConditionLocker *_wakeupToWriteLog[LogDefs::LOG_QUANTITY];              // 唤醒日志线程去写日志
    ConditionLocker _locker;                                                // 系统锁

    LogFile *_logFiles[LogDefs::LOG_QUANTITY];                              // 日志id日志文件 创建后文件只允许读不允许增删改
    std::list<LogData *> *_logDatas[LogDefs::LOG_QUANTITY];                 // 日志id日志内容
    IDelegate<void, Int32> *_threadWriteLogDelegate;                    // 日志线程写日志委托
    std::list<LogData *> *_logCaches[LogDefs::LOG_QUANTITY];                // 缓冲变量，提高性能
};

FS_NAMESPACE_END

#include "base/common/log/Impl/FS_LogImpl.h"

#endif
