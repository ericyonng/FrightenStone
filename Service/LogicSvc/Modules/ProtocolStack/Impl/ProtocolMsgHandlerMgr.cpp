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
 * @file  : ProtocolMsgHandlerMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/5/19
 * @brief :
 *          不同端口指定不同的协议类型
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Impl/ProtocolMsgHandlerMgr.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/ProtocolMsgHandler/ProtocolMsgHandlerFactory.h"

#include "Service/LogicSvc/Common.h"
#include "Service/Application.h"

ProtocolMsgHandlerMgr::ProtocolMsgHandlerMgr()
    :_reloadEvStub(0)
    , _willReloadEndEvStub(0)
{

}

ProtocolMsgHandlerMgr::~ProtocolMsgHandlerMgr()
{
    fs::STLUtil::DelMapContainer(_handlerTypeRefMsgHandlerDeleg);
    _UnRegisterEvents();
}

Int32 ProtocolMsgHandlerMgr::OnInitialize()
{
    // 创建协议处理委托
    auto allCfgs = g_ProtocolTypeCfgMgr->GetAllCfgs();
    for (auto &cfg : *allCfgs)
    {
        auto deleg = ProtocolMsgHandlerFactory::Create(cfg, this);
        _handlerTypeRefMsgHandlerDeleg.insert(std::make_pair(cfg->_handlerType, deleg));
    }

    _RegisterEvents();
    return StatusDefs::Success;
}

void ProtocolMsgHandlerMgr::_RegisterEvents()
{
    if (_reloadEvStub)
        return;

    _reloadEvStub = g_GlobalEventMgr->AddListener(EventIds::Cfgs_Reload, this, &ProtocolMsgHandlerMgr::_OnReload);
    _willReloadEndEvStub = g_GlobalEventMgr->AddListener(EventIds::Cfgs_WillReloadEnd, this, &ProtocolMsgHandlerMgr::_OnWillReloadEnd);
    _sessionConnected = g_GlobalEventMgr->AddListener(EventIds::Logic_OnSessionConnected, this, &ProtocolMsgHandlerMgr::_OnSessionConnected);
}

void ProtocolMsgHandlerMgr::_UnRegisterEvents()
{
    if (!_reloadEvStub)
        return;

    g_GlobalEventMgr->RemoveListenerX(_reloadEvStub);
    g_GlobalEventMgr->RemoveListenerX(_willReloadEndEvStub);
    g_GlobalEventMgr->RemoveListenerX(_sessionConnected);
}

void ProtocolMsgHandlerMgr::_OnReload(fs::FS_Event *ev)
{
    const Int32 cfgFileType = ev->GetParam(FS_EventParam::CfgFileType).AsInt32();
    if (cfgFileType != CfgFileUniqueType::ProtocolTypeCfgMgr)
        return;

    // TODO:修改配置并留存延迟处理的增删改配置信息
}

void ProtocolMsgHandlerMgr::_OnWillReloadEnd(fs::FS_Event *ev)
{
    // TODO:处理增删改配置信息,及相关数据
}

void ProtocolMsgHandlerMgr::_OnSessionConnected(fs::FS_Event *ev)
{
    // 绑定协议处理函数
    const UInt64 sessionId = ev->GetParam(FS_EventParam::SessionId).AsUInt64();
    auto session = _dispatcher->GetSession(sessionId);
    auto user = g_UserMgr->GetUserBySessionId(sessionId);

    // 端口指定协议类型
    UInt16 protocolPort = session->GetProtocolPort();
    if (!protocolPort)
    {
        g_Log->e<ProtocolMsgHandlerMgr>(_LOGFMT_("protocolPort is zero, session link in not from acceptor or connector sessionId[%llu],"
            " will kill this session now.")
            , sessionId);

        user->Close(UserCloseReasonType::NoProtocolPort);
        return;
    }

    auto cfg = g_ProtocolTypeCfgMgr->GetByPort(protocolPort);
    auto msgHandler = _GetMsgHandler(cfg->_handlerType);
    session->BindMsgHandler(msgHandler);
}
