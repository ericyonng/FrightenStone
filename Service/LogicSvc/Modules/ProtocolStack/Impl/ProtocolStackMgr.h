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
 * @file  : ProtocolStackMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/05
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_ProtocolStack_Impl_ProtocolStackMgr_H__
#define __Service_LogicSvc_Modules_ProtocolStack_Impl_ProtocolStackMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/ProtocolStack/Interface/IProtocolStackMgr.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/InvokeProtocolType.h"

class ProtocolStackMgr : public IProtocolStackMgr
{
public:
    ProtocolStackMgr();
    ~ProtocolStackMgr();

public:
    virtual Int32 OnInitialize();

public:
    virtual void SubscribeProtocol(UInt32 protocolCmd, fs::IDelegate<void, UInt64, fs::NetMsgDecoder *> *handler);
    virtual bool InvokeProtocolHandler(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);

private:
    void _RegisterEvents();
    void _UnRegisterEvents();
    void _OnLogicTurnStartup(fs::FS_Event *ev);

    bool _InvokeProtocolDoNothing(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);
    bool _InvokeRealProtocol(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);

private:
    // 协议处理
    typedef bool(ProtocolStackMgr::*InvokeProtocolCallback)(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);
    static InvokeProtocolCallback _invokeProtocol[InvokeProtocolType::End];
    Int32 _invokeProtocolType;
    std::map<UInt32, fs::IDelegate<void, UInt64, fs::NetMsgDecoder *> *> _protocolCmdRefHandlers;

    fs::FS_ListenerStub _logicTurnStartupStub;
};

#include "Service/LogicSvc/Modules/ProtocolStack/Impl/ProtocolStackMgrImpl.h"

#endif
