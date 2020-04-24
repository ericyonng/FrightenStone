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
 * @file  : TestProtocolsMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_TestProtocols_Impl_TestProtocolsMgr_H__
#define __Service_LogicSvc_Modules_TestProtocols_Impl_TestProtocolsMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/TestProtocols/Interface/ITestProtocolsMgr.h"

class TestProtocolsMgr : public ITestProtocolsMgr
{
public:
    TestProtocolsMgr();
    ~TestProtocolsMgr();

public:
    virtual void OnInitialize();

public:
    void BuildLoginData(fs::LoginData &data);
    void OnSendLoginReqSuc();
    void CheckRecvMsg(Int32 recvMsgId);

private:
    void _RegisterEvents();
    void _UnRegisterEvents();

    void _OnSessionConnected(fs::FS_Event *ev);
    void _OnSessionWillDisconnect(fs::FS_Event *ev);

private:

    // 接收消息id计数
    int _recvMsgID = 1;
    // 发送消息id计数
    int _sendMsgID = 1;

    fs::FS_ListenerStub _onSessionConnected;
    fs::FS_ListenerStub _onSessionWillDisConnect;
};

#endif
