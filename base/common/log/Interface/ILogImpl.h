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
 * @file  : ILogImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/06/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Log_Interface_ILog_H__
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
                                   , typeid(ObjType).name()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Info, LogDefs::_SYSLOG_details_, newLogData);
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
                                   , typeid(ObjType).name()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Debug, LogDefs::_SYSLOG_details_, newLogData);
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
                                   , typeid(ObjType).name()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Warning, LogDefs::_SYSLOG_details_, newLogData);
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
                                   , typeid(ObjType).name()
                                   , funcName
                                   , codeLine)
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Error, LogDefs::_SYSLOG_details_, newLogData);
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

    _WriteLog(LogLevel::Crash, LogDefs::_SYSLOG_crash_, newLogData);
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
                                   , typeid(ObjType).name())
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Net, LogDefs::_SYSLOG_net_, newLogData);
}

template<typename... Args>
inline void ILog::memleak(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , LogLevel::GetDescription(LogLevel::Memleak))
                            .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Memleak, LogDefs::_SYSLOG_memleak_, newLogData);
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

    _WriteLog(LogLevel::MemPool, LogDefs::_SYSLOG_mempool_, newLogData);
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

    _WriteLog(LogLevel::ObjPool, LogDefs::_SYSLOG_objpool_, newLogData);
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
                                   , typeid(ObjType).name()
                                   , funcName
                                   , codeLine)
        .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Sys, LogDefs::_SYSLOG_sys_, newLogData);
}

template<typename ObjType, typename... Args>
inline void ILog::any(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat("%s<%s>[%s]: "
                                         , newLogData->_logTime.ToString().c_str()
                                         , LogLevel::GetDescription(LogLevel::Any)
                                         , typeid(ObjType).name())
        .AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Any, LogDefs::_SYSLOG_Any_, newLogData);
}

template<typename... Args>
inline void ILog::custom(const char *fmt, const Args&... args)
{
    // 构建日志数据
    LogData *newLogData = new LogData;
    newLogData->_logTime.FlushTime();
    newLogData->_logToWrite.AppendFormat(fmt, args...) << FS_String::endl;

    _WriteLog(LogLevel::Custom, LogDefs::_SYSLOG_Custom_, newLogData);
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

