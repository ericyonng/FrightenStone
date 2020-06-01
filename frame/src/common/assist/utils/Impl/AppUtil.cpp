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
 * @file  : AppUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/3/16
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/assist/utils/Impl/AppUtil.h"
#include "FrightenStone/common/assist/utils/Impl/TimeUtil.h"
#include "FrightenStone/common/assist/utils/Impl/FS_TlsUtil.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/SmartVar/SmartVar.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/memoryhelper/MemoryHelper.h"
#include "FrightenStone/common/memleak/memleak.h"

FS_NAMESPACE_BEGIN

Int32 AppUtil::Init()
{
    // 1.时区
    TimeUtil::SetTimeZone();

    // 2.智能变量的类型识别
    SmartVarRtti::InitRttiTypeNames();

    // 3.初始化线程局部存储句柄
    Int32 ret = FS_TlsUtil::CreateUtilTlsHandle();
    if(ret != StatusDefs::Success)
    {
        FS_String str;
        str.AppendFormat("CreateUtilTlsHandle fail ret[%d]", ret);
        ASSERTBOX(str.c_str());
        return ret;
    }

    // 4.log初始化 NULL默认以程序名为基准创建目录
    ret = g_Log->InitModule(NULL);
    if(ret != StatusDefs::Success)
    {
        FS_String str;
        str.AppendFormat("log init fail ret[%d]", ret);
        ASSERTBOX(str.c_str());
        return ret;
    }

    // 5. crash dump switch start
    ret = CrashHandleUtil::InitCrashHandleParams();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<IFS_NetEngine>(_LOGFMT_("init crash handle params fail ret[%d]"), ret);
        return ret;
    }

    // 6.大小端判断，服务器只支持x86等小端字节序的cpu
    if(!SystemUtil::IsLittleEndian())
    {
        ret = StatusDefs::SystemUtil_NotLittleEndian;
        g_Log->e<AppUtil>(_LOGFMT_("the endianness of current machine is bigendian. This app support little endian only.!ret[%d]")
                                , ret);
        return ret;
    }

    // 6.Socket环境
    ret = SocketUtil::InitSocketEnv();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<IFS_NetEngine>(_LOGFMT_("InitSocketEnv fail ret[%d]"), ret);
        return ret;
    }

    return StatusDefs::Success;
}

Int32 AppUtil::Start(UInt64 maxAllowObjPoolBytesOccupied, UInt64 maxAllowMemPoolBytesOccupied, bool enableMemMonitor, UInt32 memMonitorPrintLogIntervalSeconds)
{
    // 1.内存助手
    MemoryHelper::GetInstance()->Start(maxAllowObjPoolBytesOccupied, maxAllowMemPoolBytesOccupied);
    MemleakMonitor::GetInstance()->BeforeStart(memMonitorPrintLogIntervalSeconds);

    // 2.内存监控
    if(enableMemMonitor)
    {
        MemleakMonitor::GetInstance()->Start();
    }

    return StatusDefs::Success;
}

void AppUtil::Wait()
{
    getchar();
}

void AppUtil::Finish()
{
    // 清理sock环境
    SocketUtil::ClearSocketEnv();

    // 关闭内存监控
    MemleakMonitor::GetInstance()->Finish();
    MemoryHelper::GetInstance()->Finish();

    // 当前日志全部着盘
    g_Log->FlushAllFile();
    g_Log->FinishModule();
    g_MemoryPool->FinishPool();
}

FS_NAMESPACE_END

