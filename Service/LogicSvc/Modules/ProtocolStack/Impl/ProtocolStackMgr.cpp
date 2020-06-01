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
 * @file  : ProtocolStackMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/05
 * @brief :
 */

#include "stdafx.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Impl/ProtocolStackMgr.h"

IProtocolStackMgr *g_ProtocolStackMgr = NULL;

ProtocolStackMgr::InvokeProtocolCallback ProtocolStackMgr::_invokeProtocol[InvokeProtocolType::End] =
{
  &ProtocolStackMgr::_InvokeProtocolDoNothing
, &ProtocolStackMgr::_InvokeRealProtocol
};

ProtocolStackMgr::ProtocolStackMgr()
    :_logicTurnStartupStub(0)
    ,_invokeProtocolType(InvokeProtocolType::DoNoting)
{
    g_ProtocolStackMgr = this;
}

ProtocolStackMgr::~ProtocolStackMgr()
{
    fs::STLUtil::DelMapContainer(_protocolCmdRefHandlers);
}

Int32 ProtocolStackMgr::OnInitialize()
{
    _RegisterEvents();
    return StatusDefs::Success;
}

void ProtocolStackMgr::SubscribeProtocol(UInt32 protocolCmd, fs::IDelegate<void, UInt64, fs::NetMsgDecoder *> *handler)
{
    auto iterHandler = _protocolCmdRefHandlers.find(protocolCmd);
    if(iterHandler != _protocolCmdRefHandlers.end())
    {
        g_Log->w<ProtocolStackMgr>(_LOGFMT_("protocol cmd[%d] handler[%p] already exists, please check if cover it or is wrong operation!")
                                    , protocolCmd, iterHandler->second);
        Fs_SafeFree(iterHandler->second);
        _protocolCmdRefHandlers.erase(iterHandler);
    }

    _protocolCmdRefHandlers.insert(std::make_pair(protocolCmd, handler));
}

void ProtocolStackMgr::_RegisterEvents()
{
    if(_logicTurnStartupStub)
        return;

    _logicTurnStartupStub = g_GlobalEventMgr->AddListener(EventIds::Logic_TurnStartup, this, &ProtocolStackMgr::_OnLogicTurnStartup);
}

void ProtocolStackMgr::_UnRegisterEvents()
{
    if(!_logicTurnStartupStub)
        return;

    g_GlobalEventMgr->RemoveListenerX(_logicTurnStartupStub);
}

void ProtocolStackMgr::_OnLogicTurnStartup(fs::FS_Event *ev)
{
    _invokeProtocolType = InvokeProtocolType::RealInvoke;
}

bool ProtocolStackMgr::_InvokeRealProtocol(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
{
    auto iterHandler = _protocolCmdRefHandlers.find(msgDecoder->GetCmd());
    if(iterHandler == _protocolCmdRefHandlers.end())
    {
        g_Log->e<ProtocolStackMgr>(_LOGFMT_("sessionId[%llu], protocolCmd[%u] have no handler!"), sessionId, msgDecoder->GetCmd());
        return false;
    }

    auto handler = iterHandler->second;
    handler->Invoke(sessionId, msgDecoder);
    return true;
}

