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
 * @file  : FS_EpollAcceptor.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/02/01
 * @brief :
 */

#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollAcceptor.h"
#include "FrightenStone/common/net/Impl/FS_Epoll.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Defs/EpollDefs.h"
#include "FrightenStone/common/net/Defs/IFS_NetBuffer.h"
#include "FrightenStone/common/net/Impl/IFS_ConfigMgr.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Defs/BriefListenAddrInfo.h"
#include <FrightenStone/common/net/Defs/NetCfgDefs.h>
#include "FrightenStone/common/net/Impl/FS_EpollAcceptPoller.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"
#include "FrightenStone/common/component/Impl/File/FS_IniFile.h"


FS_NAMESPACE_BEGIN

FS_EpollAcceptor::FS_EpollAcceptor(UInt32 compId
                                 , Locker &sessionLocker
                                 , Int32 &curSessionCnt
                                 , Int32 &maxSessionQuantityLimit
                                 , UInt64 &curMaxSessionId
                                 , const AcceptorCfgs &cfg
                                 , IFS_NetEngine *netEngine)
    :IFS_Acceptor(compId, netEngine, sessionLocker, curSessionCnt, maxSessionQuantityLimit, curMaxSessionId, cfg)
    , _isInit(false)
    , _poller(NULL)
{
    /*     _CrtMemCheckpoint(&s1);*/
}

FS_EpollAcceptor::~FS_EpollAcceptor()
{
    Fs_SafeFree(_poller);

    //     _CrtMemCheckpoint(&s2);
    //     if(_CrtMemDifference(&s3, &s1, &s2))
    //         _CrtMemDumpStatistics(&s3);
}

Int32 FS_EpollAcceptor::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    if(_isInit)
        return StatusDefs::Success;

    Int32 st = IFS_Acceptor::BeforeStart(totalCfgs);
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollAcceptor>(_LOGFMT_("before start fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_EpollAcceptor::Start()
{
    if(_isInit)
        return StatusDefs::Success;

    Int32 st = _poller->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollAcceptor>(_LOGFMT_("poller start error st[%d]"), st);
        return st;
    }

    _isInit = true;
    return StatusDefs::Success;
}

void FS_EpollAcceptor::AfterStart()
{
    if(_poller)
        _poller->AfterStart();

    g_Log->sys<FS_EpollAcceptor>(_LOGFMT_("acceptor start suc..."));
}

void FS_EpollAcceptor::WillClose()
{
    if(!_isInit)
        return;

    _poller->WillClose();
}

void FS_EpollAcceptor::BeforeClose()
{
    if(!_isInit)
        return;

    _poller->BeforeClose();
}

void FS_EpollAcceptor::Close()
{
    if(!_isInit)
        return;

    _poller->Close();
    _isInit = false;
}

void FS_EpollAcceptor::AfterClose()
{
    if(_poller)
        _poller->AfterClose();
}

IFS_AcceptorPoller *FS_EpollAcceptor::_CreatePoller()
{
    if (_poller)
    {
        g_Log->e<FS_EpollAcceptor>(_LOGFMT_("has create poller before"));
        return _poller;
    }

    _poller = new FS_EpollAcceptPoller(this);
    return _poller;
}

void FS_EpollAcceptor::OnSessionDisconnected(UInt64 sessionId)
{
    _poller->OnSessionDisconnected(sessionId);
}
FS_NAMESPACE_END

#endif
