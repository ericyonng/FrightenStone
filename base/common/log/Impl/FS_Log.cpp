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

FS_Log::FS_Log(const Byte8 *rootDirName)
    :_rootDirName(rootDirName)
    , _threadPool(NULL)
    , _threadWorkIntervalMsTime(LOG_THREAD_INTERVAL_MS_TIME)
    , _threadWriteLogDelegate(NULL)
    , _logCaches(NULL)
    , _levelRefHook{NULL}
    , _logFiles{NULL}
    ,_logDatas{NULL}
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
    fs::STLUtil::DelArray(_logDatas);
    fs::STLUtil::DelArray(_logFiles);
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
        auto logFile = _logFiles[fileUnqueIndex];
        if(logFile)
            break;

        // 2.创建文件夹
        FS_String logName = ".\\";
        logName += (_rootDirName + "\\" + logPath);
        if(!FS_DirectoryUtil::CreateDir(logName))
        {
            ret = StatusDefs::Log_CreateDirFail;
            break;
        }

        // 3.创建文件
        logName += fileName;
        logFile = new LogFile;
        if(!logFile->Open(logName.c_str(), true, "ab+", false))
        {
            ret = StatusDefs::Log_CreateLogFileFail;
            ASSERT(!"log file open fail");
            break;
        }

        logFile->UpdateLastTimestamp();
        _logFiles[fileUnqueIndex] = logFile;

        // 4.创建日志内容
        _logDatas[fileUnqueIndex] = new std::list<LogData *>;
    } while(0);
    
    _locker.Unlock();

    return ret;
}

void FS_Log::_WriteLog(Int32 level, Int32 fileUniqueIndex, LogData *logData)
{
    _locker.Lock();

    // 1.将日志数据放入队列
    _logDatas[fileUniqueIndex]->push_back(logData);

    // 2.根据level不同调用不同的hook
    if(_levelRefHook[level])
        (*_levelRefHook[level])(std::move(logData));
    _locker.Unlock();
}

void FS_Log::_OnThreadWriteLog()
{
    // 1.转移到缓冲区（只交换list指针）
    _locker.Lock();
    for(_logCaches->_increasePos = 0, _logCaches->_pos = 0;
        _logCaches->_pos < fs::LogDefs::LOG_QUANTITY; 
        ++_logCaches->_pos)
    {
        if(_logDatas[_logCaches->_pos]->empty())
            continue;

        // 只交换数据队列指针拷贝最少，最快，而且交换后_logDatas队列是空的相当于清空了数据 保证缓冲队列前几个都不为NULL, 碰到NULL表示结束
        _logCaches->_swapCache = _logCaches->_logDataCache._cache[_logCaches->_increasePos];

        _logCaches->_logDataCache._cache[_logCaches->_increasePos] = _logDatas[_logCaches->_pos];
        _logCaches->_logDataCache._pos = _logCaches->_pos;

        _logDatas[_logCaches->_pos] = _logCaches->_swapCache;

        ++_logCaches->_increasePos;
    }
    _locker.Unlock();

    if(_logCaches->_logDataCache._cache[0]->empty())
        return;

    // 2.写日志
    for(_logCaches->_pos = 0; _logCaches->_pos < fs::LogDefs::LOG_QUANTITY; ++_logCaches->_pos)
    {
        if(_logCaches->_logDataCache._cache[_logCaches->_pos]->empty())
            break;

        _logCaches->_logFile = _logFiles[_logCaches->_logDataCache._pos];
        for(auto &iterLog : *_logCaches->_logDataCache._cache[_logCaches->_pos])
        {
            // 文件过大转储到分立文件
            if(_logCaches->_logFile->IsTooLarge(LOG_SIZE_LIMIT))
                _logCaches->_logFile->PartitionFile();

            // 写入文件
            _logCaches->_logFile->Write(iterLog->_logToWrite.c_str(), iterLog->_logToWrite.GetLength());
            Fs_SafeFree(iterLog);
        }

        _logCaches->_logFile->Flush();
        _logCaches->_logDataCache._cache[_logCaches->_pos]->clear();
    }
}
FS_NAMESPACE_END

