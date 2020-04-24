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
 * @file  : TestProtocolsGlobal.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */

#include "stdafx.h"
#include "Service/LogicSvc/Modules/TestProtocols/Impl/TestProtocolsGlobal.h"
#include "Service/LogicSvc/Modules/TestProtocols/Defs/FS_ClientCfgMgr.h"
#include "Service/LogicSvc/Modules/TestProtocols/Interface/ITestProtocolsMgr.h"
#include "FrightenStone/common/component/Impl/Time.h"

TestProtocolsGlobal::TestProtocolsGlobal()
    :_connectOpFinish(0)
    ,_userLoginResStub(0)
    ,_isConnectOpFinish(false)
    ,_lastTime(0)
{
    
}

TestProtocolsGlobal::~TestProtocolsGlobal()
{
    _sendTimer->Cancel();
    Fs_SafeFree(_sendTimer);
    _UnRegisterEvents();
}

void TestProtocolsGlobal::OnInitialize()
{
    _RegisterEvents();
    _sendTimer = new fs::FS_Timer(_dispatcher->GetTimeWheel());
    _sendTimer->SetTimeOutHandler(this, &TestProtocolsGlobal::_OnSendTimeOut);
}

void TestProtocolsGlobal::OnLoginRes(UInt64 sessionId, fs::NetMsgDecoder *decoder)
{
    fs::LoginRes loginRes;
    if(!loginRes.DeserializeFrom(decoder))
    {
        g_Log->e<TestProtocolsGlobal>(_LOGFMT_("sessionId[%llu] login res DeserializeFrom msgDecoder error len[%u]")
                             , sessionId, decoder->GetMsgLen());
        return;
    }

    auto user = g_UserMgr->GetUserBySessionId(sessionId);
    auto testProtocolMgr = user->GetSys<ITestProtocolsMgr>();
    if(_checkMsgId)
        testProtocolMgr->CheckRecvMsg(loginRes._msgId);
    //g_Log->i<TestProtocolsGlobal>(_LOGFMT_("msgid[%d]"), loginRes._msgId);
    if(_isSendAfterSvrResArrive)
    {
        testProtocolMgr->BuildLoginData(_sendReq._loginData);
        user->SendData(&_sendReq);
        testProtocolMgr->OnSendLoginReqSuc();
    }
}

void TestProtocolsGlobal::_RegisterEvents()
{
    if(_connectOpFinish)
        return;

    _connectOpFinish = g_GlobalEventMgr->AddListener(EventIds::Logic_OnAllConnectToRemoteOpFinish, this, &TestProtocolsGlobal::_OnConnectOpFinish);
}

void TestProtocolsGlobal::_UnRegisterEvents()
{
    if(!_connectOpFinish)
        return;

    g_GlobalEventMgr->RemoveListenerX(_connectOpFinish);
}

void TestProtocolsGlobal::_OnConnectOpFinish(fs::FS_Event *ev)
{
    _sendNumPerPeriod = g_ClientCfgMgr->GetMsgNumPerPeriod();
    _sendPeriodMs = g_ClientCfgMgr->GetSendPeriodMs();
    _checkMsgId = g_ClientCfgMgr->NeedCheckMsgId();
    _isSendAfterSvrResArrive = g_ClientCfgMgr->IsSendAfterSvrResArrive();

    auto &loginData = _sendReq._loginData;
    strcpy(loginData._userName, "shy");
    strcpy(loginData._pwd, "123456");

    if(!_isSendAfterSvrResArrive)
        _sendTimer->Schedule(_sendPeriodMs);
    else
    {
        _SendAllUserLoginReq();
    }
}

void TestProtocolsGlobal::_OnSendTimeOut(fs::FS_Timer *timer, const fs::Time &lastTime, const fs::Time &curTime)
{
    _sendTimer->Cancel();

    if(_isSendAfterSvrResArrive)
        return;

    _SendAllUserLoginReq();
    _sendTimer->Schedule(_sendPeriodMs);

//     auto nowTime = static_cast<Int64>(fs::Time::NowMilliTimestamp());
//     Int64 diff = 0;
//     if(!_lastTime)
//         _lastTime = nowTime;
// 
//     diff = nowTime - _lastTime;
//     _lastTime = nowTime;
// 
//     g_Log->i<TestProtocolsGlobal>(_LOGFMT_("diff[%lld ms] sendperiodms[%d]"), nowTime - _lastTime, _sendPeriodMs);

}

void TestProtocolsGlobal::_SendAllUserLoginReq()
{
    auto &allUsers = g_UserMgr->GetAllUsers();
    for(Int32 i = 0; i < _sendNumPerPeriod; ++i)
    {// 单位时间发_sendNumPerPeriod条消息
        for(auto &iterUser : allUsers)
        {
            auto &user = iterUser.second;
            auto testProtocolsMgr = user->GetSys<ITestProtocolsMgr>();
            testProtocolsMgr->BuildLoginData(_sendReq._loginData);
            user->SendData(&_sendReq);
            testProtocolsMgr->OnSendLoginReqSuc();
        }
    }
}
