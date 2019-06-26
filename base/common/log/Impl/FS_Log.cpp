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
#include "base/common/log/Defs/LogCaches.h"

#define LOG_THREAD_INTERVAL_MS_TIME 0    // 日志线程写日志间隔时间
#define LOG_SIZE_LIMIT  -1            // 日志尺寸限制10MB

FS_NAMESPACE_BEGIN

FS_Log::FS_Log(const Byte8 *processName)
    :_processName(processName)
    , _threadPool(NULL)
    , _threadWorkIntervalMsTime(LOG_THREAD_INTERVAL_MS_TIME)
    , _threadWriteLogDelegate(NULL)
    , _logCaches(NULL)
    , _levelRefHook{NULL}
{

}

FS_Log::~FS_Log()
{
    FinishModule();
}

Int32 FS_Log::InitModule()
{
    if(_isInit)
        return StatusDefs::Success;

    // 初始化日志文件
    _logCaches = new LogCaches;
    LogDefs::LogInitHelper<LogDefs::LOG_NUM_MAX>::InitLog(this);

    // 初始化线程池并添加写日志任务
    _threadPool = new FS_ThreadPool(1, 1);
    _threadWriteLogDelegate = DelegatePlusFactory::Create(this, &FS_Log::_OnThreadWriteLog);
    ITask *logTask = new LogTask(_threadPool, _threadWriteLogDelegate, _threadWorkIntervalMsTime);
    ASSERT(_threadPool->AddTask(*logTask));

    _isInit = true;
    return StatusDefs::Success;
}

void FS_Log::FinishModule()
{
    if(_isFinish)
        return;

    _isFinish = true;

    // 关闭线程池
    _threadPool->Clear();

    // 清理数据
    Fs_SafeFree(_threadWriteLogDelegate);
    fs::STLUtil::DelMapContainer(_fileUniqueIndexRefLogDatas);
    fs::STLUtil::DelMapContainer(_fileUniqueIndexRefLogFiles);
    fs::STLUtil::DelArray(_levelRefHook);
    Fs_SafeFree(_logCaches);
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

        // 4.创建日志内容
        _fileUniqueIndexRefLogDatas.insert(std::make_pair(fileUnqueIndex, new std::list<LogData *>));
        // 5.日志内容缓冲
        _logCaches->_logDatasCache.insert(std::make_pair(fileUnqueIndex, new std::list<LogData *>));
    } while(0);
    
    _locker.Unlock();

    return ret;
}

void FS_Log::_WriteLog(Int32 level, Int32 fileUniqueIndex, LogData *logData)
{
    _locker.Lock();

    // 1.将日志数据放入队列
    auto logList = _fileUniqueIndexRefLogDatas.find(fileUniqueIndex)->second;
    logList->push_back(logData);

    // 2.根据level不同调用不同的hook
    if(_levelRefHook[level])
        (*_levelRefHook[level])(std::move(logData));
    _locker.Unlock();
}

void FS_Log::_OnThreadWriteLog()
{
    // 1.转移到缓冲区
    _locker.Lock();
    _logCaches->_hasLog = false;
    for(auto &iterLogSrcList : _fileUniqueIndexRefLogDatas)
    {
        if(iterLogSrcList.second->empty())
            continue;

        _logCaches->_iterToWrite = _logCaches->_logDatasCache.find(iterLogSrcList.first);
        _logCaches->_hasLog = true;
        std::swap(*_logCaches->_iterToWrite->second, *iterLogSrcList.second);
    }
    _locker.Unlock();

    if(!_logCaches->_hasLog)
        return;

    // 2.写日志
    for(auto &iterToWrite : _logCaches->_logDatasCache)
    {
        if(iterToWrite.second->empty())
            continue;

        _logCaches->_logFile = _fileUniqueIndexRefLogFiles.find(iterToWrite.first)->second;
        for(auto &iterLog : *iterToWrite.second)
        {
            if(_logCaches->_logFile->IsDayPass(iterLog->_logTime))
            {
                ASSERT(_logCaches->_logFile->Reopen());
                _logCaches->_logFile->UpdateLastTimestamp();
            }
            else
            {
                // 文件过大转储到分立文件
                if(_logCaches->_logFile->IsTooLarge(LOG_SIZE_LIMIT))
                    _logCaches->_logFile->PartitionFile();
            }

            // 写入文件
            _logCaches->_logFile->Write(iterLog->_logToWrite.c_str(), iterLog->_logToWrite.GetLength());
            Fs_SafeFree(iterLog);
        }

        _logCaches->_logFile->Flush();
        iterToWrite.second->clear();
    }
}
FS_NAMESPACE_END

