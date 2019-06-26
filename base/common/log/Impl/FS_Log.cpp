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
#include "base/common/log/Impl/FS_Log.h"
#include "base/common/assist/utils/utils.h"
#include "base/common/component/Impl/File/LogFile.h"
#include "base/common/status/status.h"
#include "base/common/log/Defs/LogDefs.h"
#include "base/common/log/Defs/LogTask.h"
#include "base/common/assist/utils/Impl/FS_FileUtil.h"

#define LOG_THREAD_INTERVAL_MS_TIME 0    // 日志线程写日志间隔时间
#define LOG_SIZE_LIMIT  -1            // 日志尺寸限制10MB

FS_NAMESPACE_BEGIN

FS_Log::FS_Log(const Byte8 *processName)
    :_processName(processName)
    ,_threadPool(NULL)
    ,_threadWorkIntervalMsTime(LOG_THREAD_INTERVAL_MS_TIME)
    ,_threadWriteLogDelegate(NULL)
{

}

FS_Log::~FS_Log()
{
    FinishModule();
}

void FS_Log::InstallLogHookFunc(Int32 level, IDelegatePlus<void, const LogData *> *delegate)
{
    UnInstallLogHook(level);
    _levelRefHook.insert(std::make_pair(level, delegate));
}

void FS_Log::UnInstallLogHook(Int32 level)
{
    auto iterDelegate = _levelRefHook.find(level);
    if(iterDelegate == _levelRefHook.end())
        return;

    Fs_SafeFree(iterDelegate->second);
    _levelRefHook.erase(iterDelegate);
}

Int32 FS_Log::InitModule()
{
    // 初始化日志文件
    LogDefs::LogInitHelper<LogDefs::LOG_NUM_MAX>::InitLog(this);

    // 初始化线程池并添加写日志任务
    _threadPool = new FS_ThreadPool(1, 1);
    _threadWriteLogDelegate = DelegatePlusFactory::Create(this, &FS_Log::_OnThreadWriteLog);
    ITask *logTask = new LogTask(_threadPool, _threadWriteLogDelegate, _threadWorkIntervalMsTime);
    ASSERT(_threadPool->AddTask(*logTask));
    return StatusDefs::Success;
}

void FS_Log::FinishModule()
{
    // 关闭线程池
    _threadPool->Clear();

    // 清理数据
    Fs_SafeFree(_threadWriteLogDelegate);
    fs::STLUtil::DelMapContainer(_fileUniqueIndexRefLogDatas);
    fs::STLUtil::DelMapContainer(_fileUniqueIndexRefLogFiles);
    fs::STLUtil::DelMapContainer(_levelRefHook);
    fs::STLUtil::DelMapContainer(_logDatasCache);
    Fs_SafeFree(_threadPool);
}

Int32 FS_Log::CreateLogFile(Int32 fileUnqueIndex, const char *logPath, const char *fileName)
{
    Int32 ret = StatusDefs::Success;
    _locker.Lock();
    do 
    {
        // 1.是否创建过
        auto iterLogFile = _fileUniqueIndexRefLogFiles.find(fileUnqueIndex);
        if(iterLogFile != _fileUniqueIndexRefLogFiles.end())
            break;

        // 2.创建文件夹
        FS_String logName = ".\\";
        logName += (_processName + "\\" + logPath);
        if(!FS_DirectoryUtil::CreateDir(logName))
        {
            ret = StatusDefs::Log_CreateDirFail;
            break;
        }

        // 3.创建文件
        logName += fileName;
        auto logFile = new LogFile;
        if(!logFile->Open(logName.c_str(), true, "ab+", true))
        {
            ret = StatusDefs::Log_CreateLogFileFail;
            ASSERT(!"log file open fail");
            break;
        }

        logFile->UpdateLastTimestamp();
        _fileUniqueIndexRefLogFiles.insert(std::make_pair(fileUnqueIndex, logFile));

        // 4.创建日志缓冲
        _NewLogDataList(fileUnqueIndex);
    } while(0);
    
    _locker.Unlock();

    return ret;
}

LogData *FS_Log::_BuildLogData(const Byte8 *className, const Byte8 *funcName, const FS_String &content, Int32 codeLine, Int32 logLevel)
{
    LogData *newLogData = new LogData;
    newLogData->_className = className;
    newLogData->_funcName = funcName;
    newLogData->_content = content;
    newLogData->_line = codeLine;
    newLogData->_level = logLevel;
    newLogData->_processName = _processName;
    newLogData->_logTime = Time::Now();
    newLogData->_logToWrite.Format("%s<%s>[%s][%s]line:%d %s"
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(logLevel)
                                   , className
                                   , funcName
                                   , codeLine
                                   , content.c_str());
    return newLogData;
}

void FS_Log::_WriteLog(Int32 level, Int32 fileUniqueIndex, LogData *logData)
{
    _locker.Lock();

    // 1.将日志数据放入队列
    auto logList = _GetLogDataList(fileUniqueIndex);
    logList->push_back(logData);

    // 2.根据level不同调用不同的hook
    auto iterHook = _levelRefHook.find(level);
    if(iterHook != _levelRefHook.end())
    {
        auto hook = iterHook->second;
        (*hook)(std::move(logData));
    }
    _locker.Unlock();
}

std::list<LogData *> * FS_Log::_GetLogDataList(Int32 fileIndex)
{
    auto iterLogDatas = _fileUniqueIndexRefLogDatas.find(fileIndex);
    if(iterLogDatas == _fileUniqueIndexRefLogDatas.end())
        return NULL;

    return iterLogDatas->second;
}

std::list<LogData *> * FS_Log::_NewLogDataList(Int32 fileIndex)
{
    auto newList = new std::list<LogData *>;
    _fileUniqueIndexRefLogDatas.insert(std::make_pair(fileIndex, newList));
    return newList;
}

void FS_Log::_OnThreadWriteLog()
{
    // 线程操作需要枷锁
    // 将日志内容移出到缓冲

    // 1.转移到缓冲区
    static std::map<Int32, std::list<LogData *> *>::iterator iterToWrite;

    _locker.Lock();
    if(_fileUniqueIndexRefLogDatas.empty())
    {
        _locker.Unlock();
        return;
    }

    static bool hasLog = false;
    hasLog = false;
    for(auto iterLogDatas = _fileUniqueIndexRefLogDatas.begin(); iterLogDatas != _fileUniqueIndexRefLogDatas.end(); ++iterLogDatas)
    {
        iterToWrite = _logDatasCache.find(iterLogDatas->first);
        if(iterToWrite == _logDatasCache.end())
            iterToWrite = _logDatasCache.insert(std::make_pair(iterLogDatas->first, new std::list<LogData *>)).first;

        if(iterLogDatas->second->empty())
            continue;
        
        hasLog = true;

        std::swap(*iterToWrite->second, *iterLogDatas->second);
    }
    _locker.Unlock();
    if(!hasLog)
        return;

    // 2.写日志
    static std::map<Int32, LogFile *>::iterator iterLogFile;
    static LogFile * logFile = NULL;
    static std::list<LogData *> * logDataList = NULL;
    static FS_String cache;
    static Int32 fileIndex = 0;
    static Time cacheTIme;
    for(auto &iterLogDatas : _logDatasCache)
    {
        fileIndex = iterLogDatas.first;
        logDataList = iterLogDatas.second;

        iterLogFile = _fileUniqueIndexRefLogFiles.find(fileIndex);
        logFile = iterLogFile->second;

        logFile->Lock();
        for(auto &iterLog : *logDataList)
        {
            if(logFile->IsDayPass(iterLog->_logTime))
            {
                ASSERT(logFile->Reopen());
                logFile->UpdateLastTimestamp();
            }
            else
            {
                // 文件过大转储到分立文件
                if(logFile->IsTooLarge(LOG_SIZE_LIMIT))
                    logFile->PartitionFile();
            }

            // 构造日志正文
            cache.Clear();
            cache.Format("nowTime[%lld]%s"
                         , cacheTIme.FlushTime().GetMicroTimestamp()
                         , iterLog->_logToWrite.c_str());

            // 写入文件
            ASSERT(logFile->Write(cache.c_str(), cache.GetLength()) == cache.GetLength());
            Fs_SafeFree(iterLog);
        }

        logFile->Flush();
        logDataList->clear();
        logFile->UnLock();
    }
}
FS_NAMESPACE_END

