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
 * @file  : TestProtocolsGlobal.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */

#include "stdafx.h"
#include "Service/LogicSvc/Modules/TestProtocols/Impl/TestProtocolsGlobal.h"
#include "Service/LogicSvc/Modules/TestProtocols/Defs/FS_ClientCfgMgr.h"
#include "Service/LogicSvc/Modules/TestProtocols/Interface/ITestProtocolsMgr.h"
#include "FrightenStone/common/component/Impl/Time.h"

#include "Service/LogicSvc/Modules/CheckStartup/CheckStartup.h"
#include "Service/LogicSvc/Modules/NodeConnection/NodeConnection.h"
#include "Service/LogicSvc/Modules/StubBehavior/StubBehavior.h"

TestProtocolsGlobal::TestProtocolsGlobal()
    :_onAsynConnectResult(0)
    , _userLoginResEvStub(0)
    , _onAfterSessionDisconnectedEvStub(0)
    ,_lastTime(0)
    , _testBlackList(NULL)
{
}

TestProtocolsGlobal::~TestProtocolsGlobal()
{
    _sendTimer->Cancel();
    _testBlackList->Cancel();
    Fs_SafeFree(_sendTimer);
    Fs_SafeFree(_testBlackList);
    _UnRegisterEvents();
}

void TestProtocolsGlobal::OnInitialize()
{
    _RegisterEvents();
    _sendTimer = new fs::FS_Timer(_dispatcher->GetTimeWheel());
    _sendTimer->SetTimeOutHandler(this, &TestProtocolsGlobal::_OnSendTimeOut);

    _testBlackList = new fs::FS_Timer(_dispatcher->GetTimeWheel());
    _testBlackList->SetTimeOutHandler(this, &TestProtocolsGlobal::_OnTestBlackListTimer);
}

void TestProtocolsGlobal::OnLocalServerReady()
{
    // 向对端发起连接
    _TestConnect();
}

void TestProtocolsGlobal::OnLocalServerStartup()
{
    auto logic = _dispatcher->GetLogic();
    if (logic->GetServiceId() != ServiceType::ClientSimulation)
        return;

    // 测试发送登陆包
    _TestSendLogin();
    // 测试黑民单系统
    // _StartTestBlackList();
    // _TestBlackList();
}

void TestProtocolsGlobal::OnLoginRes(UInt64 sessionId, fs::NetMsgDecoder *decoder)
{

}

void TestProtocolsGlobal::_RegisterEvents()
{
    if(_onAsynConnectResult)
        return;

    _onAsynConnectResult = g_GlobalEventMgr->AddListener(EventIds::Logic_AsynConnectResult, this, &TestProtocolsGlobal::_OnAsynConnectResult);
    _userLoginResEvStub = g_GlobalEventMgr->AddListener(EventIds::User_LoginRes, this, &TestProtocolsGlobal::_OnUserLoginRes);
    _onAfterSessionDisconnectedEvStub = g_GlobalEventMgr->AddListener(EventIds::Logic_OnAfterSessionDisconnected, this, &TestProtocolsGlobal::_OnAfterSessionDisconnected);
}

void TestProtocolsGlobal::_UnRegisterEvents()
{
    if(!_onAsynConnectResult)
        return;

    g_GlobalEventMgr->RemoveListenerX(_onAsynConnectResult);
    g_GlobalEventMgr->RemoveListenerX(_userLoginResEvStub);
    g_GlobalEventMgr->RemoveListenerX(_onAfterSessionDisconnectedEvStub);
}

void TestProtocolsGlobal::_OnAsynConnectResult(fs::FS_Event *ev)
{
    const UInt64 stub = ev->GetParam(FS_EventParam::Stub).AsUInt64();

    // 测试连接
    auto iterStub = _testConnectPendingStubs.find(stub);
    if(iterStub != _testConnectPendingStubs.end())
    {
        _testConnectPendingStubs.erase(iterStub);
        if(_testConnectPendingStubs.empty())
            g_CheckStartupMgr->RemovePendingBeforeStartup(CheckStartupPendingType::TestConnect);
    }
}

void TestProtocolsGlobal::_OnUserLoginRes(fs::FS_Event *ev)
{
    if (!_isSendAfterSvrResArrive)
        return;

    const UInt64 sessionId = ev->GetParam(FS_EventParam::SessionId).AsUInt64();
    const Int32 msgId = ev->GetParam(FS_EventParam::RecvMsgId).AsInt32();

    auto user = g_UserMgr->GetUserBySessionId(sessionId);
    auto testProtocolMgr = user->GetSys<ITestProtocolsMgr>();
    if (_checkMsgId)
        testProtocolMgr->CheckRecvMsg(msgId);

    testProtocolMgr->BuildLoginData(_sendReq._loginData);
    testProtocolMgr->AddSendMsgId();
    user->SendData(&_sendReq);
    testProtocolMgr->OnSendLoginReqSuc();
}

void TestProtocolsGlobal::_OnAfterSessionDisconnected(fs::FS_Event *ev)
{
//    _TestBlackList();
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
            testProtocolsMgr->AddSendMsgId();
            user->SendData(&_sendReq);
            testProtocolsMgr->OnSendLoginReqSuc();
        }
    }
}

void TestProtocolsGlobal::_TestConnect()
{
    auto logic = _dispatcher->GetLogic();
    if (logic->GetServiceId() != ServiceType::ClientSimulation)
        return;

    // TODO:抛连接节点消息到connector
    const Int32 connectQuatity = g_ClientCfgMgr->GetClientQuantity();
    const auto &ip = g_ClientCfgMgr->GetTargetSvrIp();
    const auto port = g_ClientCfgMgr->GetTargetSvrPort();

    for(Int32 i = 0; i < connectQuatity; ++i)
    {
        auto stub = g_StubBehaviorGlobal->GetNewStub();
        fs::FS_ConnectInfo newConnectInfo;
        newConnectInfo._stub = stub;
        newConnectInfo._ip = ip;
        newConnectInfo._port = port;
        auto st = g_NodeConnectionMgr->AsynConnect(newConnectInfo);
        if(st == StatusDefs::Success)
            _testConnectPendingStubs.insert(stub);
    }

    if(!_testConnectPendingStubs.empty())
        g_CheckStartupMgr->RegisterPendingBeforeStartup(CheckStartupPendingType::TestConnect);
}

void TestProtocolsGlobal::_TestSendLogin()
{
    // 测试发送登陆包
    _sendNumPerPeriod = g_ClientCfgMgr->GetMsgNumPerPeriod();
    _sendPeriodMs = g_ClientCfgMgr->GetSendPeriodMs();
    _checkMsgId = g_ClientCfgMgr->NeedCheckMsgId();
    _isSendAfterSvrResArrive = g_ClientCfgMgr->IsSendAfterSvrResArrive();

    auto &loginData = _sendReq._loginData;
    strcpy(loginData._userName, "shy");
    strcpy(loginData._pwd, "123456");

    if (!_isSendAfterSvrResArrive)
        _sendTimer->Schedule(_sendPeriodMs);
    else
    {
        _SendAllUserLoginReq();
    }
}

void TestProtocolsGlobal::_OnTestBlackListTimer(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curWheelTime)
{
    _TestBlackList();
}

void TestProtocolsGlobal::_TestBlackList()
{
    auto logic = _dispatcher->GetLogic();
    if (logic->GetServiceId() != ServiceType::ClientSimulation)
        return;

    // 1.连接回调定义
    auto __connectCallback = [](UInt64 sessionId, PendingStubInfo *stub, Int32 resultCode)->void
    {
        // 连接不成功
        if (resultCode != StatusDefs::Success)
        {
            g_Log->w<TestProtocolsGlobal>(_LOGFMT_("fail connect target sessionId[%llu], stub[%llu] addr[%s:%hu]")
                , sessionId, stub->_pendingStub, stub->_ip.c_str(), stub->_port);
            return;
        }

        auto user = g_UserMgr->GetUserBySessionId(sessionId);
//         BUFFER256 randData{
//             // 前4字节长度
//             Byte8(127), Byte8(127), Byte8(127), Byte8(127),
//             // 后4字节cmd
//             Byte8(127), Byte8(127), Byte8(127), Byte8(127),
//             Byte8(0) };

        BUFFER256 randData;
        user->SendData(randData, sizeof(randData));
#ifdef _DEBUG
        g_Log->i<TestProtocolsGlobal>(_LOGFMT_("sessionId[%llu], has send a rand data"), sessionId);
#endif
    };

    // 2. 新建一个连接
    const auto &ip = g_ClientCfgMgr->GetTargetSvrIp();
    const auto port = g_ClientCfgMgr->GetTargetSvrPort();
    auto stub = g_StubBehaviorGlobal->GetNewStub();
    fs::FS_ConnectInfo newConnectInfo;
    newConnectInfo._stub = stub;
    newConnectInfo._ip = ip;
    newConnectInfo._port = port;
    auto deleg = fs::DelegatePlusFactory::Create<decltype(__connectCallback), void, UInt64, PendingStubInfo *, Int32>(__connectCallback);
    auto st = g_NodeConnectionMgr->AsynConnect(newConnectInfo, deleg);
    if (st != StatusDefs::Success)
        g_Log->w<TestProtocolsGlobal>(_LOGFMT_("g_NodeConnectionMgr->AsynConnect fail st[%d] addr[%s:%hu]")
            , st, ip.c_str(), port);
}

void TestProtocolsGlobal::_StartTestBlackList()
{
    // 每隔10s测试一次黑名单系统
    _testBlackList->Schedule(fs::Time::_millisecondPerSecond * 4);
}
