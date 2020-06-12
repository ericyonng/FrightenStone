/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : ILogImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/06/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Log_Interface_ILog_H__
#pragma once

FS_NAMESPACE_BEGIN
inline ILog::ILog()
{
}

inline ILog::~ILog()
{
}

template<typename ObjType, typename... Args>
inline void ILog::i(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s][%s][line:%d]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Info)
                                   , RTTIUtil::GetByType<ObjType>()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Info, _GetLogFileIndex(LogFileType::Details), newLogData);
}

template<typename ObjType, typename... Args>
inline void ILog::d(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s][%s][line:%d]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Debug)
                                   , RTTIUtil::GetByType<ObjType>()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Debug, _GetLogFileIndex(LogFileType::Details), newLogData);
}

template<typename ObjType, typename... Args>
inline void ILog::w(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s][%s][line:%d]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Warning)
                                   , RTTIUtil::GetByType<ObjType>()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Warning, _GetLogFileIndex(LogFileType::Details), newLogData);
}

template<typename ObjType, typename... Args>
inline void ILog::e(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s][%s][line:%d]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Error)
                                   , RTTIUtil::GetByType<ObjType>()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Error, _GetLogFileIndex(LogFileType::Details), newLogData);
    ASSERT(!"error");
}

template<typename... Args>
inline void ILog::crash(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Crash))
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Crash, _GetLogFileIndex(LogFileType::Crash), newLogData);
}

template<typename ObjType, typename... Args>
inline void ILog::net(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Net)
                                   , RTTIUtil::GetByType<ObjType>())
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Net, _GetLogFileIndex(LogFileType::Net), newLogData);
}

template<typename... Args>
inline void ILog::mempool(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>: "
                                         , newLogData->_logTime.ToString().c_str()
                                         , LogLevel::GetDescription(LogLevel::MemPool))
        .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::MemPool, _GetLogFileIndex(LogFileType::Mempool), newLogData);
}

template<typename... Args>
inline void ILog::objpool(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>: "
                                         , newLogData->_logTime.ToString().c_str()
                                         , LogLevel::GetDescription(LogLevel::ObjPool))
        .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::ObjPool, _GetLogFileIndex(LogFileType::ObjPool), newLogData);
}
template<typename ObjType, typename... Args>
inline void ILog::sys(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s][%s][line:%d]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Sys)
                                   , RTTIUtil::GetByType<ObjType>()
                                   , funcName
                                   , codeLine)
        .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Sys, _GetLogFileIndex(LogFileType::Sys), newLogData);
}

template<typename... Args>
inline void ILog::custom(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Custom, _GetLogFileIndex(LogFileType::Custom), newLogData);
}

// 性能分析日志
template<typename ObjType, typename... Args>
inline void ILog::performance(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s][%s][line:%d]: "
        , newLogData->_logTime.ToString().c_str()
        , LogLevel::GetDescription(LogLevel::Sys)
        , RTTIUtil::GetByType<ObjType>()
        , funcName
        , codeLine)
        .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::PerformanceAnalysis, _GetLogFileIndex(LogFileType::PerformanceAnalysis), newLogData);
}

// 网络包
// 请使用便利宏_LOGFMT_
template<typename ObjType, typename... Args>
inline void ILog::netpackage(const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s][%s][line:%d]: "
        , newLogData->_logTime.ToString().c_str()
        , LogLevel::GetDescription(LogLevel::Sys)
        , RTTIUtil::GetByType<ObjType>()
        , funcName
        , codeLine)
        .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::NetPackage, _GetLogFileIndex(LogFileType::NetPackage), newLogData);
}

template<typename ObjType>
inline const IDelegate<void, const LogData *> *ILog::InstallLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*func)(const LogData *logData))
{
    auto newDelegate = DelegatePlusFactory::Create(obj, func);
    return _InstallLogHookFunc(level, newDelegate);
}

inline const IDelegate<void, const LogData *> * ILog::InstallLogHookFunc(Int32 level, void(*func)(const LogData *logData))
{
    auto newDelegate = DelegatePlusFactory::Create(func);
    return _InstallLogHookFunc(level, newDelegate);
}

// hook与具体类型有关
template<typename ObjType>
inline const IDelegate<void, LogData *> * ILog::InstallBeforeLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*func)(LogData *logData))
{
    auto newDelegate = DelegatePlusFactory::Create(obj, func);
    return _InstallBeforeLogHookFunc(level, newDelegate);
}

inline const IDelegate<void, LogData *> *ILog::InstallBeforeLogHookFunc(Int32 level, void(*func)(LogData *logData))
{
    auto newDelegate = DelegatePlusFactory::Create(func);
    return _InstallBeforeLogHookFunc(level, newDelegate);
}

FS_NAMESPACE_END

#endif

