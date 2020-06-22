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
 * @file  : TestProtocolsMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_TestProtocols_Impl_TestProtocolsMgr_H__
#define __Service_LogicSvc_Modules_TestProtocols_Impl_TestProtocolsMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/TestProtocols/Interface/ITestProtocolsMgr.h"

struct StatisticsData;
class TestProtocolsMgr : public ITestProtocolsMgr
{
public:
    TestProtocolsMgr();
    ~TestProtocolsMgr();

public:
    virtual void OnInitialize();

public:
    void BuildLoginData(fs::LoginData &data);
    void BuildCheckNetData(fs::CheckNetReq &data);
    void OnSendReqSuc();
    void CheckRecvMsg(Int32 recvMsgId);
    virtual Int32 GetSendMsgId() const;
    virtual void AddSendMsgId();
    virtual void OnCheckNetReq(const fs::CheckNetReq &req);

private:
    void _RegisterEvents();
    void _UnRegisterEvents();

    void _OnSessionConnected(fs::FS_Event *ev);
    void _OnSessionWillDisconnect(fs::FS_Event *ev);
    // �ӿͻ��˵������
    void _OnLogin(fs::FS_Event *ev);
    // �ӷ���˷��ظ��ͻ���
    void _OnLoginRes(fs::FS_Event *ev);

private:

    // ������Ϣid����
    int _recvMsgID = 1;
    // ������Ϣid����
    int _sendMsgID = 1;

    Int32 _sendNumPerPeriod;        // �����ڷ��͵�����
    Int32 _sendPeriodMs = 0;      // ����ʱ������
    bool _checkMsgId = 0;       // �Ƿ�����Ϣid,���ڼ���Ƿ񶪰���
    bool _isSendAfterSvrResArrive = 0;  // �Ƿ����յ����������غ��������,��Ϊfalse��ᰴ��ʱ�����ڲ�ͣ�ķ���
    fs::LoginReq _sendReq;
    Int64 _lastTime;

    fs::FS_ListenerStub _onSessionConnected;
    fs::FS_ListenerStub _onSessionWillDisConnect;
    fs::FS_ListenerStub _onLogin;
    fs::FS_ListenerStub _userLoginResStub;
};

#endif
