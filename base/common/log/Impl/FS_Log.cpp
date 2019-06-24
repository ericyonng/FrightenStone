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

FS_NAMESPACE_BEGIN

FS_Log::FS_Log(const Byte8 *processName)
    :_processName(processName)
{
    // 初始化日志文件
    LogDefs::LogInitHelper<LogDefs::LOG_NUM_MAX>::InitLog(this);
}

FS_Log::~FS_Log()
{
    FinishModule();
}

void FS_Log::InstallLogHookFunc(Int32 level, IDelegatePlus<void, const LogData *> *delegate)
{
    UnInstallLogHook(level);
    _levelRefDelegate.insert(std::make_pair(level, delegate));
}

void FS_Log::UnInstallLogHook(Int32 level)
{
    auto iterDelegate = _levelRefDelegate.find(level);
    if(iterDelegate == _levelRefDelegate.end())
        return;

    Fs_SafeFree(iterDelegate->second);
    _levelRefDelegate.erase(iterDelegate);
}

void FS_Log::FinishModule()
{
    // 唤醒线程池
    // 清理数据
    fs::STLUtil::DelMapContainer(_levelRefDelegate);
}

void FS_Log::SetFlushInterval(Int32 interval)
{

}

Int32 FS_Log::AddLogFile(Int32 fileUnqueIndex, const char *logPath, const char *fileName)
{
    return StatusDefs::Success;
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
    return newLogData;
}

void FS_Log::_WriteLog(Int32 fileUniqueIndex, LogData *logData)
{
}

FS_NAMESPACE_END

