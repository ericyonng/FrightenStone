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
 * @file  : CheckStartupMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/06
 * @brief :
 */

#include "stdafx.h"
#include "Service/LogicSvc/Modules/CheckStartup/Impl/CheckStartupMgr.h"
#include "Service/LogicSvc/Modules/CheckStartup/Defs/CheckStartupPendingType.h"
#include "Service/LogicSvc/Modules/TestProtocols/TestProtocols.h"
#include "Service/LogicSvc/Modules/StubBehavior/StubBehavior.h"
#include "Service/LogicSvc/Modules/NodeConnection/NodeConnection.h"

ICheckStartupMgr *g_CheckStartupMgr = NULL;

CheckStartupMgr::CheckStartupMgr()
    :_onWillStartup(0)
    ,_onLocalServerReady(0)
{
    g_CheckStartupMgr = this;
}

CheckStartupMgr::~CheckStartupMgr()
{
    _UnRegisterEvents();
}

Int32 CheckStartupMgr::OnInitialize()
{
    _RegisterEvents();
    return StatusDefs::Success;
}

bool CheckStartupMgr::RegisterPendingBeforeStartup(Int32 pendingType)
{
    if(pendingType < CheckStartupPendingType::Begin || pendingType >= CheckStartupPendingType::End)
    {
        g_Log->w<CheckStartupMgr>(_LOGFMT_("unknown pendingType[%d]"), pendingType);
        return false;
    }

    _pendingEvs.insert(pendingType);
    return true;
}

void CheckStartupMgr::RemovePendingBeforeStartup(Int32 pendingType)
{
    if(pendingType < CheckStartupPendingType::Begin || pendingType >= CheckStartupPendingType::End)
    {
        g_Log->w<CheckStartupMgr>(_LOGFMT_("unknown pendingType[%d]"), pendingType);
        return;
    }

    _pendingEvs.erase(pendingType);

    _CheckStartup();
}

bool CheckStartupMgr::CanStartup() const
{
    return _pendingEvs.empty();
}

void CheckStartupMgr::_RegisterEvents()
{
    if(_onWillStartup)
        return;

    _onWillStartup = g_GlobalEventMgr->AddListener(EventIds::Logic_WillStartup, this, &CheckStartupMgr::_OnWillStartup);
    _onLocalServerReady = g_GlobalEventMgr->AddListener(EventIds::Logic_LocalServerReady, this, &CheckStartupMgr::_OnLocalServerReady);
}

void CheckStartupMgr::_UnRegisterEvents()
{
    if(!_onWillStartup)
        return;

    g_GlobalEventMgr->RemoveListenerX(_onWillStartup);
    g_GlobalEventMgr->RemoveListenerX(_onLocalServerReady);
}

void CheckStartupMgr::_OnWillStartup(fs::FS_Event *ev)
{
    _CheckStartup();
}

void CheckStartupMgr::_OnLocalServerReady(fs::FS_Event *ev)
{
    // TODO: ¼ÓÈëonstartup ÅÐ¶ÏÏî
}

void CheckStartupMgr::_CheckStartup()
{
    if(!_pendingEvs.empty())
    {
        fs::FS_String str;
        str.AppendFormat("has pending [%llu] evs not handle: [", _pendingEvs.size());
        for(auto ev : _pendingEvs)
            str.AppendFormat("%d,", ev);
        str.AppendFormat("]. cant start up now");
        g_Log->i<CheckStartupMgr>(_LOGFMT_("%s"), str.c_str());
        return;
    }

    auto ev = new fs::FS_Event(EventIds::Logic_TurnStartup);
    g_GlobalEventMgr->FireEvent(ev);

    ev = new fs::FS_Event(EventIds::Logic_TurnStartupFinish);
    g_GlobalEventMgr->FireEvent(ev);

    g_Log->i<CheckStartupMgr>(_LOGFMT_("server is entirely start up."));
}

