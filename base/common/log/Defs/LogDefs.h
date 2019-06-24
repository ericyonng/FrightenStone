#ifndef __Base_Common_Log_Defs_LogDefs_H__
#define __Base_Common_Log_Defs_LogDefs_H__
#pragma once

#include "base/common/basedefs/BaseDefs.h"
#include "base/common/log/Defs/CreateLogFileMacro.h"

FS_NAMESPACE_BEGIN

class LogDefs
{
public:
    template <int i>
    struct LogInitHelper
    {
        static bool InitLog(ILog *logMgr) { return true; }
    };

    template<int i>
    struct ExcludeCommentHelper
    {
        enum { realvalue = ExcludeCommentHelper<i - 1>::realvalue };
    };

    template<>
    struct ExcludeCommentHelper<0>
    {
        enum { realvalue = 0 };
    };

    // 定义日志文件
    DECLEAR_GENERAL_LOG_BEGIN()
    GENERAL_ADDLOG(SYSLOG, crash)           // 崩溃
    GENERAL_ADDLOG(SYSLOG, frameLayer)      // 框架层
    GENERAL_ADDLOG(SYSLOG, memleak)         // 内存泄露
    GENERAL_ADDLOG(SYSLOG, net)             // 网络层
    GENERAL_ADDLOG(TESTLOG, testcode)       // 代码测试
    DECLEAR_GENERAL_LOG_END()
};


FS_NAMESPACE_END

#include "base/common/log/Defs/LogDefsImpl.h"

#endif
