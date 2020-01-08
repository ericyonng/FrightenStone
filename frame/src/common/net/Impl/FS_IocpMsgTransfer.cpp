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
 * @file  : FS_IocpMsgTransfer.cpp
 * @author: ericyonng<120453674@qq.com>
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
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/net/Impl/FS_IocpSession.h"
#include "FrightenStone/common/net/Defs/FS_IocpBuffer.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Impl/FS_IocpPoller.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

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
FS_IocpMsgTransfer::FS_IocpMsgTransfer(UInt32 compId, IFS_NetEngine *netEngine)
    :IFS_MsgTransfer(compId, netEngine)
    ,_sessionCnt{0}
    ,_cfgs(NULL)
    ,_poller(NULL)
{
/*        _CrtMemCheckpoint(&s1);*/
}

FS_IocpMsgTransfer::~FS_IocpMsgTransfer()
{
    Fs_SafeFree(_poller);
    Fs_SafeFree(_cfgs);

//     _CrtMemCheckpoint(&s2);
//     if(_CrtMemDifference(&s3, &s1, &s2))
//         _CrtMemDumpStatistics(&s3);
}

Int32 FS_IocpMsgTransfer::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    _cfgs = new TransferCfgs;
    *_cfgs = totalCfgs._transferCfgs;

    _poller = new FS_IocpPoller(this, PollerDefs::MonitorType_Transfer);
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
    _poller->AfterStart();
}

void FS_IocpMsgTransfer::BeforeClose()
{
    _poller->BeforeClose();
}

void FS_IocpMsgTransfer::Close()
{
    _poller->Close();
}

void FS_IocpMsgTransfer::AfterClose()
{
    _poller->AfterClose();
}

void FS_IocpMsgTransfer::OnWillConnect(BriefSessionInfo *newSessionInfo)
{
    // TODO:连接消息消息
    auto newMessageBlock = MessageBlockUtil::BuildTransferWillConnectMessageBlock(_compId
                                                                                  , _generatorId
                                                                                  , _poller->GetNetModuleObj()
                                                                                  , newSessionInfo);
    ++_sessionCnt;
#ifdef _DEBUG
    if(!_concurrentMq->Push(_generatorId, newMessageBlock))
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("concurrentmq push fail generatorId[%u] compid[%u] newSessionId[%llu] sock[%llu]")
                                     , _generatorId, _compId, newSessionInfo->_sessionId, newSessionInfo->_sock);
    }
#else
    _concurrentMq->Push(_generatorId, newMessageBlock);
#endif
}

FS_NAMESPACE_END

#endif
