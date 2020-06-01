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
 * @file  : FS_IocpAcceptPoller.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/17
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_IocpAcceptPoller.h"
#include <FrightenStone/common/net/Impl/FS_Iocp.h>
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Defs/NetMethodUtil.h"
#include "FrightenStone/common/net/Defs/IoEvDefs.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Impl/IFS_Acceptor.h"

#include <FrightenStone/common/component/Impl/FS_ThreadPool.h>
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/net/Defs/EngineCompDefs.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_IocpAcceptPoller, 32);

Int32 FS_IocpAcceptPoller::BeforeStart()
{
    if(_isInit)
        return StatusDefs::Success;

    _pool = new FS_ThreadPool(0, 1);
    _iocp = new FS_Iocp;
    _ioEv = new IoEvent;

    const Int32 st = _iocp->Create();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpAcceptPoller>(_LOGFMT_("create iocp fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpAcceptPoller::Start()
{
    if(_isInit)
        return StatusDefs::Success;

    return StatusDefs::Success;
}

void FS_IocpAcceptPoller::AfterStart()
{
    if(_isInit)
        return;

    auto task = DelegatePlusFactory::Create(this, &FS_IocpAcceptPoller::_AcceptorMonitor);
    if(!_pool->AddTask(task, true))
    {
        g_Log->e<FS_IocpAcceptPoller>(_LOGFMT_("addtask fail"));
    }

    _isInit = true;
}

void FS_IocpAcceptPoller::WillClose()
{
    if(!_isInit)
        return;

    if(_quitIocpMonitor)
        _quitIocpMonitor->Invoke();

    _pool->Close();
}

void FS_IocpAcceptPoller::BeforeClose()
{
    if(!_isInit)
        return;
}

void FS_IocpAcceptPoller::Close()
{
    if(!_isInit)
        return;

    _isInit = false;
    FS_Release(_pool);
    Fs_SafeFree(_iocp);
    Fs_SafeFree(_ioEv);
}

IFS_NetModule *FS_IocpAcceptPoller::GetNetModuleObj()
{
    return _iocp;
}

bool FS_IocpAcceptPoller::OnSessionDisconnected(UInt64 sessionId)
{
    if (UNLIKELY(!IFS_AcceptorPoller::OnSessionDisconnected(sessionId)))
    {
        g_Log->w<FS_IocpAcceptPoller>(_LOGFMT_("sessionId[%llu] not belong to this acceptor compId[%llu], port[%hu]")
            , sessionId, _engineComp->GetCompId(), reinterpret_cast<IFS_Acceptor *>(_engineComp)->GetListenPort());
        return false;
    }

    {// 补充投递
        IoDataBase *backNode = NULL;
        _prepareQueueGuard.Lock();
        if (!_prepareQueue.empty())
        {
            backNode = _prepareQueue.back();
            _prepareQueue.pop_back();
        }        
        _prepareQueueGuard.Unlock();

        // 重新post加速连入 不必每次连入就重新投递一个这样降低了连入 TODO:可以在断开链接时候再行补充
        if (backNode)
        {
            const auto st = _iocp->PostAccept(_bindAcceptorSock, backNode);
            if (st != StatusDefs::Success)
            {
                g_Log->e<FS_IocpAcceptPoller>(_LOGFMT_("post accept fail whnen new session create listen sock[%llu], iodata addr[%p] st[%d]")
                    , _bindAcceptorSock, backNode, st);
            }
        }
    }

    return true;
}

void FS_IocpAcceptPoller::_AcceptorMonitor(FS_ThreadPool *pool)
{
    // 1.监听端口
    _iocp->Reg(_bindAcceptorSock);
    _iocp->LoadAcceptEx(_bindAcceptorSock);
    auto netEngine = _engineComp->GetEngine();

    // 2.定义关闭iocp
    auto __quitIocpFunc = [this]()->void {
        _iocp->PostQuit();
    };

    // 3.绑定退出iocp
    _quitIocpMonitor = DelegatePlusFactory::Create<decltype(__quitIocpFunc), void>(__quitIocpFunc);

    // 4.预先投递_maxSessionQuantityLimit个accept 快速连入
    char **bufArray = NULL;
    IoDataBase **ioDataArray = NULL;
    NetMethodUtil::PreparePostAccept(_iocp, _bindAcceptorSock, bufArray, ioDataArray, _maxSessionQuantityLimit);

    // 5.监听网络连入
    UInt64 quitFlag = static_cast<UInt64>(IocpDefs::IO_QUIT);
    IoDataBase *ioData = NULL;

    auto engine = _engineComp->GetEngine();

    // 等待其他组件ready
    _engineComp->MaskReady(true);
    g_Log->sys<FS_IocpAcceptPoller>(_LOGFMT_("iocp acceptor poller working..."));
    EngineCompsMethods::WaitForAllCompsReady(engine);
    Int32 errCode = StatusDefs::Success;
    while(pool->IsPoolWorking())
    {
        // 监听iocp
        auto ret = _iocp->WaitForCompletion(*_ioEv, errCode);

        // 出错
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_IocpAcceptPoller>(_LOGFMT_("_AcceptorMonitor.WaitForCompletion error ret[%d] errCode[%d]"), ret, errCode);
            break;
        }

        // 处理iocp退出
        if(_ioEv->_sessionId == quitFlag)
        {
            g_Log->sys<FS_IocpAcceptPoller>(_LOGFMT_("_AcceptorMonitor iocp退出 threadId<%llu> code=%llu")
                                      , SystemUtil::GetCurrentThreadId(), _ioEv->_sessionId);
            break;
        }

        // 接受连接 完成
        ioData = reinterpret_cast<IoDataBase *>(_ioEv->_ioData);
        if(IocpDefs::IO_ACCEPT == ioData->_ioType)
        {
            // TODO:在getacceptAddrInfo时候需要考虑线程是否安全
            sockaddr_in *clientAddrInfo = NULL;
            _iocp->GetClientAddrInfo(ioData->_wsaBuff.buf, clientAddrInfo);

            _HandleSessionWillConnect(netEngine, ioData->_sock, clientAddrInfo);

            // discnnected时候投递
            _prepareQueueGuard.Lock();
            _prepareQueue.push_back(ioData);
            _prepareQueueGuard.Unlock();

//             // 重新post加速连入 不必每次连入就重新投递一个这样降低了连入 TODO:可以在断开链接时候再行补充
//             const auto st = _iocp->PostAccept(_bindAcceptorSock, ioData);
//             if(st != StatusDefs::Success)
//             {
//                 g_Log->e<FS_IocpAcceptPoller>(_LOGFMT_("post accept fail whnen new session create listen sock[%llu], iodata addr[%p] st[%d]")
//                                         , _bindAcceptorSock, ioData, st);
//             }
        }
    }

    _engineComp->MaskReady(false);
    _iocp->Destroy();
    NetMethodUtil::FreePrepareAcceptBuffers(bufArray, ioDataArray, _maxSessionQuantityLimit);

    g_Log->sys<FS_IocpAcceptPoller>(_LOGFMT_("Acceptor FS_IocpPoller  compId[%u] threadId[%llu] end")
                              , _engineComp->GetCompId(), SystemUtil::GetCurrentThreadId());
}

FS_NAMESPACE_END

#endif
