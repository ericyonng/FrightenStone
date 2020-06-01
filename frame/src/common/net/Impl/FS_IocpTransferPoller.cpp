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
 * @file  : FS_IocpTransferPoller.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/05
 * @brief :
 */
#include "stdafx.h"

#ifdef _WIN32

#include "FrightenStone/common/net/Impl/FS_IocpTransferPoller.h"
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Defs/IoEvDefs.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"

#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/log/Log.h"

FS_NAMESPACE_BEGIN

Int32 FS_IocpTransferPoller::BeforeStart()
{
    if (_isInit)
        return StatusDefs::Success;

    _pool = new FS_ThreadPool(0, 1);
    _iocp = new FS_Iocp;
    _ioEv = new IoEvent;

    const Int32 st = _iocp->Create();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpTransferPoller>(_LOGFMT_("create iocp fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpTransferPoller::Start()
{
    if (_isInit)
        return StatusDefs::Success;

    return StatusDefs::Success;
}

void FS_IocpTransferPoller::AfterStart()
{
    if (_isInit)
        return;

    auto task = DelegatePlusFactory::Create(this, &FS_IocpTransferPoller::_Monitor);
    if (!_pool->AddTask(task, true))
    {
        g_Log->e<FS_IocpTransferPoller>(_LOGFMT_("addtask fail"));
    }

    _isInit = true;
}

void FS_IocpTransferPoller::WillClose()
{
    if (!_isInit)
        return;

    _pool->Close();
}

void FS_IocpTransferPoller::BeforeClose()
{
    if (!_isInit)
        return;
}

void FS_IocpTransferPoller::Close()
{
    if (!_isInit)
        return;

    _isInit = false;
    FS_Release(_pool);
    Fs_SafeFree(_iocp);
    Fs_SafeFree(_ioEv);
}

IFS_NetModule * FS_IocpTransferPoller::GetNetModuleObj()
{
    return _iocp;
}

void FS_IocpTransferPoller::_Monitor(FS_ThreadPool *pool)
{
    Int32 st = StatusDefs::Success;
    const UInt32 generatorId = _engineComp->CastTo<IFS_MsgTransfer>()->GetGeneratorId();
    const UInt32 compId = _engineComp->GetCompId();
    Int32 errorCode = StatusDefs::Success;

    // 等待其他组件ready
    auto engine = _engineComp->GetEngine();
    _engineComp->MaskReady(true);
    g_Log->sys<FS_IocpTransferPoller>(_LOGFMT_("iocp transfer monitor working..."));

    EngineCompsMethods::WaitForAllCompsReady(engine);
    while (pool->IsPoolWorking())
    {
        st = _iocp->WaitForCompletion(*_ioEv, errorCode, __FS_POLLER_MONITOR_TIME_OUT_INTERVAL__);

        if (st == StatusDefs::Ignore)
            continue;

        _mq->Push(generatorId, MessageBlockUtil::BuildIocpMsgArrivedMsgBlock(compId, generatorId, _ioEv, errorCode));
    }

    _engineComp->MaskReady(false);
    _iocp->Destroy();
    g_Log->sys<FS_IocpTransferPoller>(_LOGFMT_("Transfer Poller compId[%u] generatorId[%u] threadId[%llu] end")
        , compId, generatorId, SystemUtil::GetCurrentThreadId());
}

FS_NAMESPACE_END

#endif
