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
inline void ILog::i(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建logdata
    FS_String content;
    content.Format(fmt, args...) << FS_String::endl;
    LogData *logData = _BuildLogData(typeid(ObjType).name(), funcName, content, codeLine, LogLevel::Info);

    _WriteLog(LogLevel::Info, fileUnqueIndex, logData);
}

template<typename ObjType, typename... Args>
inline void ILog::d(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建logdata
    FS_String content;
    content.Format(fmt, args...) << FS_String::endl;
    LogData *logData = _BuildLogData(typeid(ObjType).name(), funcName, content, codeLine, LogLevel::Debug);

    _WriteLog(LogLevel::Debug, fileUnqueIndex, logData);
}

template<typename ObjType, typename... Args>
inline void ILog::w(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建logdata
    FS_String content;
    content.Format(fmt, args...) << FS_String::endl;
    LogData *logData = _BuildLogData(typeid(ObjType).name(), funcName, content, codeLine, LogLevel::Warning);

    _WriteLog(LogLevel::Warning, fileUnqueIndex, logData);
}

template<typename ObjType, typename... Args>
inline void ILog::e(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建logdata
    FS_String content;
    content.Format(fmt, args...) << FS_String::endl;
    LogData *logData = _BuildLogData(typeid(ObjType).name(), funcName, content, codeLine, LogLevel::Error);

    _WriteLog(LogLevel::Error, fileUnqueIndex, logData);
}

template<typename ObjType, typename... Args>
inline void ILog::crash(Int32 fileUnqueIndex, const char *funcName, Int32 codeLine, const char *fmt, const Args&... args)
{
    // 构建logdata
    FS_String content;
    content.Format(fmt, args...) << FS_String::endl;
    LogData *logData = _BuildLogData(typeid(ObjType).name(), funcName, content, codeLine, LogLevel::Crash);

    _WriteLog(LogLevel::Crash, fileUnqueIndex, logData);
}

template<typename ObjType>
inline void ILog::InstallLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*func)(const LogData *logData))
{
    IDelegatePlus<void, const LogData *> newDelegate = DelegatePlusFactory::Create(obj, func);
    InstallLogHookFunc(level, newDelegate);
}

FS_NAMESPACE_END

#endif

