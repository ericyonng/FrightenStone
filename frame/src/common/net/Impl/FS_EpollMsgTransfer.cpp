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
 * @file  : FS_EpollMsgTransfer.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/20
 * @brief :
 */
#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollMsgTransfer.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Impl/FS_EpollTransferPoller.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"

FS_NAMESPACE_BEGIN

FS_EpollMsgTransfer::FS_EpollMsgTransfer(UInt32 compId, IFS_NetEngine *engine)
    :IFS_MsgTransfer(compId, engine)
    , _poller(NULL)
    ,_isInit{false}
{
}

FS_EpollMsgTransfer::~FS_EpollMsgTransfer()
{
    WillClose();
    BeforeClose();
    Close();
    AfterClose();
}

Int32 FS_EpollMsgTransfer::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    if(_isInit)
        return StatusDefs::Success;

    _cfgs = new TransferCfgs;
    *_cfgs = totalCfgs._transferCfgs;

    _poller = new FS_EpollTransferPoller(this);
    _poller->AttachMessageQueue(_concurrentMq);

    Int32 st = _poller->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollMsgTransfer>(_LOGFMT_("_poller->BeforeStart fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_EpollMsgTransfer::Start()
{
    if(_isInit)
        return StatusDefs::Success;

    Int32 st = _poller->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollMsgTransfer>(_LOGFMT_("_poller->Start fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

void FS_EpollMsgTransfer::AfterStart()
{
    if(_isInit)
        return;

    _poller->AfterStart();
    _isInit = true;
    g_Log->sys<FS_EpollMsgTransfer>(_LOGFMT_("transfer start suc..."));
}

void FS_EpollMsgTransfer::WillClose()
{
    if(!_isInit)
        return;

    _poller->WillClose();
}

void FS_EpollMsgTransfer::BeforeClose()
{
    if(!_isInit)
        return;

    _poller->BeforeClose();
}

void FS_EpollMsgTransfer::Close()
{
    if(!_isInit)
        return;

    _poller->Close();
}

void FS_EpollMsgTransfer::AfterClose()
{
    if(!_isInit)
        return;

    _poller->AfterClose();
    _isInit = false;
}

bool FS_EpollMsgTransfer::IsIoFinish() const
{
    return _poller->IsIoFinish();
}
FS_NAMESPACE_END


#endif

