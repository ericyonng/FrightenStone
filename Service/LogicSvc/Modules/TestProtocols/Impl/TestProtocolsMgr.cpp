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
 * @file  : TestProtocolsMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/TestProtocols/Impl/TestProtocolsMgr.h"
#include "Service/LogicSvc/Modules/TestProtocols/Interface/ITestProtocolsGlobal.h"
#include "Service/LogicSvc/Modules/TestProtocols/Defs/FS_ClientCfgMgr.h"
#include "Service/LogicSvc/Modules/TestProtocols/Defs/StatisticsData.h"

TestProtocolsMgr::TestProtocolsMgr()
    :_onSessionConnected(0)
    ,_onSessionWillDisConnect(0)
{
}

TestProtocolsMgr::~TestProtocolsMgr()
{
    _UnRegisterEvents();
}

void TestProtocolsMgr::OnInitialize()
{
    _RegisterEvents();
}

void TestProtocolsMgr::BuildLoginData(fs::LoginData &data)
{
    data._msgId = _sendMsgID;
    ++_sendMsgID;
}

void TestProtocolsMgr::OnSendLoginReqSuc()
{
    g_StaticsData->_lock.Lock();
    ++g_StaticsData->_curSendMsgCount;
    g_StaticsData->_lock.Unlock();
}

void TestProtocolsMgr::CheckRecvMsg(Int32 recvMsgId)
{
    if(_recvMsgID != recvMsgId)
    {
        g_Log->w<TestProtocolsMgr>(_LOGFMT_("recv msg id[%d] local is[%d]"), recvMsgId, _recvMsgID);
    }
    ++_recvMsgID;
}

void TestProtocolsMgr::_RegisterEvents()
{
    if(_onSessionConnected)
        return;

    auto user = GetUser();
    _onSessionConnected = user->AddListener(EventIds::Logic_OnSessionConnected, this, &TestProtocolsMgr::_OnSessionConnected);
    _onSessionWillDisConnect = user->AddListener(EventIds::Logic_OnSessionWillDisconnect, this, &TestProtocolsMgr::_OnSessionWillDisconnect);
}

void TestProtocolsMgr::_UnRegisterEvents()
{
    if(!_onSessionConnected)
        return;

    auto user = GetUser();
    user->RemoveListenerX(_onSessionConnected);
    user->RemoveListenerX(_onSessionWillDisConnect);
}

void TestProtocolsMgr::_OnSessionConnected(fs::FS_Event *ev)
{
    g_StaticsData->_lock.Lock();
    ++g_StaticsData->_curSucConnectedClient;
    g_StaticsData->_lock.Unlock();
}

void TestProtocolsMgr::_OnSessionWillDisconnect(fs::FS_Event *ev)
{
    g_StaticsData->_lock.Lock();
    --g_StaticsData->_curSucConnectedClient;
    g_StaticsData->_lock.Unlock();
}
