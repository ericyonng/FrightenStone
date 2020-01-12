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
 * @file  : FS_Log.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "frame/src/common/log/Impl/FS_Log.h"
#include "FrightenStone/common/assist/utils/utils.h"
#include "FrightenStone/common/component/Impl/File/LogFile.h"
#include "FrightenStone/common/status/status.h"
#include "frame/src/common/log/Defs/LogDefs.h"
#include "frame/src/common/log/Defs/LogTask.h"
#include "FrightenStone/common/assist/utils/Impl/FS_FileUtil.h"
#include "frame/src/common/log/Defs/LogCaches.h"

#pragma region log config options
#undef LOG_THREAD_INTERVAL_MS_TIME
#define LOG_THREAD_INTERVAL_MS_TIME 100         // 日志线程写日志间隔时间ms
#undef LOG_SIZE_LIMIT
#define LOG_SIZE_LIMIT  268435456               // 日志尺寸限制256MB
// #define LOG_SIZE_LIMIT  -1                   // 无限制
#undef ENABLE_OUTPUT_CONSOLE
#define ENABLE_OUTPUT_CONSOLE 1                 // 开启控制台打印
#pragma endregion

FS_NAMESPACE_BEGIN

FS_Log::FS_Log()
    : _threadPool(NULL)
    , _threadWorkIntervalMsTime(LOG_THREAD_INTERVAL_MS_TIME)
    , _levelRefHook{NULL}
    , _levelRefBeforeLogHook{NULL}
    , _logFiles{NULL}
    , _logDatas{NULL}
    , _logCaches{NULL}
{
    // log根目录名以进程名+Log 不可放在构造初始化日志太早了
//     auto programName = FS_FileUtil::ExtractFileWithoutExtension(SystemUtil::GetCurProgramName().RemoveZeroTail());
//     programName.AppendFormat("%s", "Log");
    // InitModule(programName.c_str());
}

FS_Log::~FS_Log()
{
    FinishModule();
}

void FS_Log::UnInstallLogHookFunc(Int32 level, const IDelegate<void, const LogData *> *delegate)
{
    if(!_levelRefHook[level])
        return;

    for(auto iterHook = _levelRefHook[level]->begin(); iterHook != _levelRefHook[level]->end(); )
    {
        if(*iterHook == delegate)
        {
            Fs_SafeFree(*iterHook);
            iterHook = _levelRefHook[level]->erase(iterHook);
            continue;
        }

        ++iterHook;
    }
}

void FS_Log::UnInstallBeforeLogHookFunc(Int32 level, const IDelegate<void, LogData *> *delegate)
{
    if(!_levelRefBeforeLogHook[level])
        return;

    for(auto iterHook = _levelRefBeforeLogHook[level]->begin(); iterHook != _levelRefBeforeLogHook[level]->end(); )
    {
        if(*iterHook == delegate)
        {
            Fs_SafeFree(*iterHook);
            iterHook = _levelRefBeforeLogHook[level]->erase(iterHook);
            continue;
        }

        ++iterHook;
    }
}

Int32 FS_Log::InitModule(const Byte8 *rootDirName)
{
    _locker.Lock();
    if(_isInit)
    {
        _locker.Unlock();
        return StatusDefs::Success;
    }

    // 根目录
    if(!rootDirName)
    {
        auto programName = FS_FileUtil::ExtractFileWithoutExtension(SystemUtil::GetCurProgramName().RemoveZeroTail());
        programName.AppendFormat("%s", "Log");
        _rootDirName = programName;
    }
    else
    {
        _rootDirName = rootDirName;
    }

    // 初始化文件锁
    for(Int32 i = LogDefs::LOG_NUM_BEGIN; i < LogDefs::LOG_QUANTITY; ++i)
        _flielocker[i] = new ConditionLocker();

    // 初始化用于唤醒日志线程锁
    for(Int32 i = LogDefs::LOG_NUM_BEGIN; i < LogDefs::LOG_QUANTITY; ++i)
        _wakeupToWriteLog[i] = new ConditionLocker();

    // TODO:初始化log config options
    _ReadConfig();

    // 初始化日志文件
    ASSERT(LogDefs::LogInitHelper<LogDefs::LOG_NUM_MAX>::InitLog(this));

    // 初始化缓冲
    for(Int32 i = LogDefs::LOG_NUM_BEGIN; i < LogDefs::LOG_QUANTITY; ++i)
        _logCaches[i] = new std::list<LogData *>;

    // 初始化线程池并添加写日志任务,每个日志文件一个独立的线程
    _threadPool = new FS_ThreadPool(0, LogDefs::LOG_QUANTITY);
    for(Int32 i = LogDefs::LOG_NUM_BEGIN; i < LogDefs::LOG_QUANTITY; ++i)
    {
        auto writeLogDelegate = DelegatePlusFactory::Create(this, &FS_Log::_OnThreadWriteLog);
        ITask *logTask = new LogTask(_threadPool, writeLogDelegate, _threadWorkIntervalMsTime, i, *_wakeupToWriteLog[i]);
        ASSERT(_threadPool->AddTask(*logTask, true));
    }

    _isInit = true;
    _locker.Unlock();
    return StatusDefs::Success;
}

void FS_Log::FinishModule()
{
    if(_isFinish)
        return;

    _isFinish = true;

    // 清空日志
    FlushAllFile();

    // 关闭线程池
    _threadPool->Close();

    // 清理数据
    fs::STLUtil::DelArray(_logDatas);
    fs::STLUtil::DelArray(_logFiles);

    // log后hook
    for(Int32 i = 0; i < LogLevel::End; ++i)
    {
        if(!_levelRefHook[i])
            continue;

        for(auto iterHook = _levelRefHook[i]->begin(); iterHook != _levelRefHook[i]->end(); ++iterHook)
            Fs_SafeFree(*iterHook);

        Fs_SafeFree(_levelRefHook[i]);
    }

    // log前hook
    for(Int32 i = 0; i < LogLevel::End; ++i)
    {
        if(!_levelRefBeforeLogHook[i])
            continue;

        for(auto iterHook = _levelRefBeforeLogHook[i]->begin(); iterHook != _levelRefBeforeLogHook[i]->end(); ++iterHook)
            Fs_SafeFree(*iterHook);

        Fs_SafeFree(_levelRefBeforeLogHook[i]);
    }

    Fs_SafeFree(_threadPool);
    STLUtil::DelArray(_wakeupToWriteLog);
    STLUtil::DelArray(_flielocker);
    STLUtil::DelArray(_logCaches);
}

void FS_Log::FlushAllFile()
{
    for(Int32 i = LogDefs::LOG_NUM_BEGIN; i < LogDefs::LOG_QUANTITY; ++i)
    {
        _wakeupToWriteLog[i]->Lock();
        _wakeupToWriteLog[i]->Sinal();
        _wakeupToWriteLog[i]->Unlock();
    }
}

Int32 FS_Log::CreateLogFile(Int32 fileUnqueIndex, const char *logPath, const char *fileName)
{
    Int32 ret = StatusDefs::Success;
    // _locker.Lock();
    do 
    {
        // 1.是否创建过
        auto logFile = _logFiles[fileUnqueIndex];
        if(logFile)
            break;

        // 2.创建文件夹
        FS_String logName = "./";
        logName += (_rootDirName + "/" + logPath);
        if(!FS_DirectoryUtil::CreateDir(logName))
        {
            ret = StatusDefs::Log_CreateDirFail;
            break;
        }

        // 3.系统是否第一次创建
        logName += fileName;
        bool isFirstCreate = !FS_FileUtil::IsFileExist(logName.c_str());

        // 4.创建文件
        logFile = new LogFile;
        if(!logFile->Open(logName.c_str(), true, "ab+", true))
        {
            ret = StatusDefs::Log_CreateLogFileFail;
            ASSERT(!"log file open fail");
            break;
        }

        // 系统启动备份旧日志
        logFile->PartitionFile(isFirstCreate);
        logFile->UpdateLastPassDayTime();
        _logFiles[fileUnqueIndex] = logFile;

        // 4.创建日志内容
        _logDatas[fileUnqueIndex] = new std::list<LogData *>;
    } while(0);
    
    // _locker.Unlock();

    return ret;
}

void FS_Log::_WriteLog(Int32 level, Int32 fileUniqueIndex, LogData *logData)
{
    // 1.根据level不同调用不同的log前hook 不可做影响log执行的其他事情
    if(_levelRefBeforeLogHook[level])
    {
        for(auto iterHook = _levelRefBeforeLogHook[level]->begin();
            iterHook != _levelRefBeforeLogHook[level]->end();
            ++iterHook)
        {
            (*iterHook)->Invoke(logData);
        }
    }

    // 2.拷贝一次数据
    LogData cache = *logData;
    
    // 3.将日志数据放入队列
    _flielocker[fileUniqueIndex]->Lock();
    if(_isFinish)
    {
        _flielocker[fileUniqueIndex]->Unlock();
        Fs_SafeFree(logData);
        return;
    }
    _logDatas[fileUniqueIndex]->push_back(logData);
    _flielocker[fileUniqueIndex]->Unlock();

    // 4.打印到控制台
#if ENABLE_OUTPUT_CONSOLE
    _OutputToConsole(level, cache._logToWrite);
#endif
    
    // 5.根据level不同调用不同的hook
    if(_levelRefHook[level])
    {
        for(auto iterHook = _levelRefHook[level]->begin();
            iterHook != _levelRefHook[level]->end();
            ++iterHook)
        {
            (*iterHook)->Invoke(&cache);
        }
    }
}

Int32 FS_Log::_GetLogFileIndex(Int32 logTypeEnum)
{
    switch(logTypeEnum)
    {
        case LogFileType::Crash:
            return LogDefs::_SYSLOG_crash_;
        case LogFileType::Custom:
            return LogDefs::_SYSLOG_Custom_;
        case LogFileType::Details:
            return LogDefs::_SYSLOG_details_;
        case LogFileType::Mempool:
            return LogDefs::_SYSLOG_mempool_;
        case LogFileType::Net:
            return LogDefs::_SYSLOG_net_;
        case LogFileType::ObjPool:
            return LogDefs::_SYSLOG_objpool_;
        case LogFileType::Sys:
            return LogDefs::_SYSLOG_sys_;
        default:
            break;
    }

    return -1;
}

void FS_Log::_OutputToConsole(Int32 level,const FS_String &logStr)
{
//#ifdef _WIN32
    // windows下需要打印控制台
    if(_IsAllowToConsole(level))
    {
        SystemUtil::LockConsole();
        const Int32 oldColor = SystemUtil::GetConsoleColor();
        _SetConsoleColor(level);
        SystemUtil::OutputToConsole(logStr);
        SystemUtil::SetConsoleColor(oldColor);
        SystemUtil::UnlockConsole();
    }
//#endif

}

void FS_Log::_SetConsoleColor(Int32 level)
{
#ifdef _WIN32
    switch(level)
    {
        case LogLevel::Debug:
        case LogLevel::Info:
        case LogLevel::Net:
        case LogLevel::Sys:
        {
            SystemUtil::SetConsoleColor(FS_ConsoleColor::Fg_White | FS_ConsoleColor::Bg_Black);
        }
        break;
        case LogLevel::Warning:
        {
            SystemUtil::SetConsoleColor(FS_ConsoleColor::Fg_LightYellow | FS_ConsoleColor::Bg_Black);
        }
        break;
        case LogLevel::Error:
        case LogLevel::Crash:
        {
            SystemUtil::SetConsoleColor(FS_ConsoleColor::Fg_Red | FS_ConsoleColor::Bg_Black);
        }
        break;
        default:
        {
            SystemUtil::SetConsoleColor(FS_ConsoleColor::Fg_White | FS_ConsoleColor::Bg_Black);
        }
    }
#endif
}

void FS_Log::_ReadConfig()
{
    // 日志配置项
}

void FS_Log::_OnThreadWriteLog(Int32 logFileIndex)
{
    // 1.转移到缓冲区（只交换list指针）
    _flielocker[logFileIndex]->Lock();
    if(_logDatas[logFileIndex]->empty())
    {
        _flielocker[logFileIndex]->Unlock();
        return;
    }

    // 只交换数据队列指针拷贝最少，最快，而且交换后_logDatas队列是空的相当于清空了数据 保证缓冲队列前几个都不为NULL, 碰到NULL表示结束
    std::list<LogData *> *&swapCache = _logCaches[logFileIndex]; // 由于主线程不会共享_logCaches所以是线程安全的（每个日志文件线程只有一个线程）,请保证外部其他线程不会调用本接口，需要立即写日志请调用flushall接口
    std::list<LogData *> *cache4RealLog = NULL;
    cache4RealLog = _logDatas[logFileIndex];
    _logDatas[logFileIndex] = swapCache;
    swapCache = cache4RealLog;
    _flielocker[logFileIndex]->Unlock();

    if(swapCache->empty())
        return;

    // 2.写日志
    LogFile *logFile = _logFiles[logFileIndex];
    for(auto &iterLog : *swapCache)
    {
        // 跨天重新打开
        if(logFile->IsDayPass(iterLog->_logTime))
        {
            // 跨天需要重新打开文件
            logFile->Reopen(&iterLog->_logTime);
            logFile->UpdateLastPassDayTime(&iterLog->_logTime);
        }

        // 文件过大转储到分立文件
        if(logFile->IsTooLarge(LOG_SIZE_LIMIT))
            logFile->PartitionFile(false, &iterLog->_logTime);

        // 写入文件
        logFile->Write(iterLog->_logToWrite.c_str(), iterLog->_logToWrite.GetLength());
        Fs_SafeFree(iterLog);
    }

    logFile->Flush();
    swapCache->clear();
}
FS_NAMESPACE_END

