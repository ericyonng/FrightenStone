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
 * @file  : TestProtocolsGlobalFacade.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/TestProtocols/Impl/TestProtocolsGlobalFacade.h"
#include "Service/LogicSvc/Modules/TestProtocols/Impl/TestProtocolsGlobalFactory.h"
#include "Service/LogicSvc/Modules/TestProtocols/Impl/TestProtocolsGlobal.h"

Int32 TestProtocolsGlobalFacade::OnInitialize()
{
    g_GlobalSysMgr->RegisterGlobalSys<TestProtocolsGlobalFactory>();
    _RegisterProtocols();
    return StatusDefs::Success;
}

void TestProtocolsGlobalFacade::_RegisterProtocols()
{
    // g_ProtocolStackMgr->SubscribeProtocol(fs::ProtocolCmd::LoginRes, this, &TestProtocolsGlobalFacade::_OnLoginRes);
    g_ProtocolStackMgr->SubscribeProtocol(fs::ProtocolCmd::CheckNetRes, this, &TestProtocolsGlobalFacade::_OnCheckNetRes);
    g_ProtocolStackMgr->SubscribeProtocol(fs::ProtocolCmd::CheckNetReq, this, &TestProtocolsGlobalFacade::_OnCheckNetReq);
}

void TestProtocolsGlobalFacade::_OnLoginRes(UInt64 sessionId, fs::NetMsgDecoder *decoder)
{
    auto testProtocolsGlobal = g_GlobalSysMgr->GetSys<TestProtocolsGlobal>();
    testProtocolsGlobal->OnLoginRes(sessionId, decoder);
}

void TestProtocolsGlobalFacade::_OnCheckNetRes(UInt64 sessionId, fs::NetMsgDecoder *decoder)
{
    auto testProtocolsGlobal = g_GlobalSysMgr->GetSys<TestProtocolsGlobal>();
    testProtocolsGlobal->OnCheckNetRes(sessionId, decoder);
}

void TestProtocolsGlobalFacade::_OnCheckNetReq(UInt64 sessionId, fs::NetMsgDecoder *decoder)
{
    auto testProtocolsGlobal = g_GlobalSysMgr->GetSys<TestProtocolsGlobal>();
    testProtocolsGlobal->OnCheckNetReq(sessionId, decoder);
}
