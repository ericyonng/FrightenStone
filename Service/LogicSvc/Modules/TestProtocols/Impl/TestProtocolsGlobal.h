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
 * @file  : TestProtocolsGlobal.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_TestProtocols_Impl_TestProtocolsGlobal_H__
#define __Service_LogicSvc_Modules_TestProtocols_Impl_TestProtocolsGlobal_H__
#pragma once

#include "Service/LogicSvc/Modules/TestProtocols/Interface/ITestProtocolsGlobal.h"

class TestProtocolsGlobal : public ITestProtocolsGlobal
{
public:
    TestProtocolsGlobal();
    ~TestProtocolsGlobal();

public:
    virtual bool IsConnectOpFinish();
    virtual void OnInitialize();

public:
    void OnLoginRes(UInt64 sessionId, fs::NetMsgDecoder *decoder);

private:
    void _RegisterEvents();
    void _UnRegisterEvents();

    void _OnConnectOpFinish(fs::FS_Event *ev);
    void _OnSendTimeOut(fs::FS_Timer *timer, const fs::Time &lastTime, const fs::Time &curTime);
    void _SendAllUserLoginReq();

private:
    fs::FS_ListenerStub _connectOpFinish;
    fs::FS_ListenerStub _userLoginResStub;

    bool _isConnectOpFinish;

    fs::FS_Timer *_sendTimer;

    Int32 _sendNumPerPeriod;        // 周期内发送的数量
    Int32 _sendPeriodMs = 0;      // 发送时间周期
    bool _checkMsgId = 0;       // 是否检测消息id,用于检测是否丢包等
    bool _isSendAfterSvrResArrive = 0;  // 是否在收到服务器返回后继续发送,若为false则会按照时间周期不停的发送
    fs::LoginReq _sendReq;
    Int64 _lastTime;
};

#pragma region inline

inline bool TestProtocolsGlobal::IsConnectOpFinish()
{
    return _isConnectOpFinish;
}
#pragma endregion

#endif
