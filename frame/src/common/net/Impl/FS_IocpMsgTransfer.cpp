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
 * @file  : FS_IocpMsgTransfer.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_IocpMsgTransfer.h"

#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
///////////////////////////////////////以下需要清理
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Impl/FS_IocpTransferPoller.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"
#include "FrightenStone/common/net/Impl/FS_Session.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/memleak/memleak.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_IocpMsgTransfer, __DEF_OBJ_POOL_OBJ_NUM__);

FS_IocpMsgTransfer::FS_IocpMsgTransfer(UInt32 compId, IFS_NetEngine *netEngine)
    :IFS_MsgTransfer(compId, netEngine)
    ,_poller(NULL)
    ,_isInit{false}
{
/*        _CrtMemCheckpoint(&s1);*/
}

FS_IocpMsgTransfer::~FS_IocpMsgTransfer()
{
    Fs_SafeFree(_poller);

//     _CrtMemCheckpoint(&s2);
//     if(_CrtMemDifference(&s3, &s1, &s2))
//         _CrtMemDumpStatistics(&s3);
}

Int32 FS_IocpMsgTransfer::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    if(_isInit)
        return StatusDefs::Success;

    _cfgs = new TransferCfgs;
    *_cfgs = totalCfgs._transferCfgs;

    _poller = new FS_IocpTransferPoller(this);
    _poller->AttachMessageQueue(_concurrentMq);

    Int32 st = _poller->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("_poller->BeforeStart fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpMsgTransfer::Start()
{
    if(_isInit)
        return StatusDefs::Success;

    Int32 st = _poller->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("_poller->Start fail st[%d]"), st);
        return st;
    }
    
    return StatusDefs::Success;
}

void FS_IocpMsgTransfer::AfterStart()
{
    if(_isInit)
        return;

    _poller->AfterStart();
    _isInit = true;

    g_Log->sys<FS_IocpMsgTransfer>(_LOGFMT_("transfer start suc..."));
}

void FS_IocpMsgTransfer::WillClose()
{
    if(!_isInit)
        return;

    _poller->WillClose();
}

void FS_IocpMsgTransfer::BeforeClose()
{
    if(!_isInit)
        return;

    _poller->BeforeClose();
}

void FS_IocpMsgTransfer::Close()
{
    if(!_isInit)
        return;

    _poller->Close();
}

void FS_IocpMsgTransfer::AfterClose()
{
    if(!_isInit)
        return;

    _poller->AfterClose();
    _isInit = false;
}


IFS_BasePoller * FS_IocpMsgTransfer::GetPoller()
{
    return _poller;
}

FS_NAMESPACE_END



#endif
