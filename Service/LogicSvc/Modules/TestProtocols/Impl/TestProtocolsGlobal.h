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
 * @file  : TestProtocolsGlobal.h
 * @author: Eric Yonng<120453674@qq.com>
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
    virtual void OnInitialize();
    virtual void OnLocalServerReady();
    virtual void OnLocalServerStartup();

public:
    void OnLoginRes(UInt64 sessionId, fs::NetMsgDecoder *decoder);
    void OnCheckNetRes(UInt64 sessionId, fs::NetMsgDecoder *decoder);
    void OnCheckNetReq(UInt64 sessionId, fs::NetMsgDecoder *decoder);

private:
    void _RegisterEvents();
    void _UnRegisterEvents();
    // ���ӽ��
    void _OnAsynConnectResult(fs::FS_Event *ev);
    // ��½����
    void _OnUserLoginRes(fs::FS_Event *ev);
    // �Ͽ�����
    void _OnAfterSessionDisconnected(fs::FS_Event *ev);

    /* ���Է��� */
    void _OnSendTimeOut(fs::FS_Timer *timer, const fs::Time &lastTime, const fs::Time &curTime);
    void _SendAllUserLoginReq();
    void _TestConnect();
    void _TestSendLogin();
    void _TestCheckNet();
    void _SendAllUserCheckNet();

    /* ���Ժ����� */
    void _OnTestBlackListTimer(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curWheelTime);
    void _TestBlackList();
    void _StartTestBlackList();

private:
    fs::FS_ListenerStub _onAsynConnectResult;
    fs::FS_ListenerStub _userLoginResEvStub;
    fs::FS_ListenerStub _onAfterSessionDisconnectedEvStub;

    fs::FS_Timer *_sendTimer;

    Int32 _sendNumPerPeriod;        // �����ڷ��͵�����
    Int32 _sendPeriodMs = 0;      // ����ʱ������
    bool _checkMsgId = 0;       // �Ƿ�����Ϣid,���ڼ���Ƿ񶪰���
    bool _isSendAfterSvrResArrive = 0;  // �Ƿ����յ����������غ��������,��Ϊfalse��ᰴ��ʱ�����ڲ�ͣ�ķ���
    fs::LoginReq _sendReq;
    Int64 _lastTime;
    fs::CheckNetReq _checkNetReq;
    
    std::set<UInt64> _testConnectPendingStubs;

    /* ���Ժ����� */
    fs::FS_Timer *_testBlackList;    // ÿ��1s����һ��
};

#pragma region inline

#pragma endregion

#endif
