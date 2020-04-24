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
 * @file  : EventFilter.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/16
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/EventFilter/Impl/EventFilter.h"
#include "Service/LogicSvc/Modules/EventFilter/Defs/EventFilterDefs.h"

EventFilter::EventFilter()
    :_onSessionConnected(0)
    ,_afterSessionConnected(0)
    ,_onSessionWillDisconnect(0)
    ,_onAfterSessionDisconnect(0)
    ,_onLogicBeforeMsgDispatch(0)
{

}

EventFilter::~EventFilter()
{

}

void EventFilter::Register(fs::EventManager &evMgr)
{
    if(_afterSessionConnected)
        return;

    _onSessionConnected = evMgr.AddListener(EventIds::Logic_OnSessionConnected, this, &EventFilter::_OnSessionConnected);
    _afterSessionConnected = evMgr.AddListener(EventIds::Logic_OnAfterSessionConnected, this, &EventFilter::_OnAfterSessionConnected);
    _onSessionWillDisconnect = evMgr.AddListener(EventIds::Logic_OnSessionWillDisconnect, this, &EventFilter::_OnSessionWillDisconnect);
    _onAfterSessionDisconnect = evMgr.AddListener(EventIds::Logic_OnAfterSessionDisconnected, this, &EventFilter::_OnAfterSessionDisconnect);
    _onLogicBeforeMsgDispatch = evMgr.AddListener(EventIds::Logic_BeforeMsgDispatch, this, &EventFilter::_OnLogicBeforeMsgDispatch);
}

void EventFilter::UnRegister(fs::EventManager &evMgr)
{
    if(!_afterSessionConnected)
        return;

    evMgr.RemoveListenerX(_onSessionConnected);
    evMgr.RemoveListenerX(_afterSessionConnected);
    evMgr.RemoveListenerX(_onSessionWillDisconnect);
    evMgr.RemoveListenerX(_onAfterSessionDisconnect);
}

void EventFilter::_OnSessionConnected(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnAfterSessionConnected(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnSessionWillDisconnect(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnAfterSessionDisconnect(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}

void EventFilter::_OnLogicBeforeMsgDispatch(fs::FS_Event *ev)
{
    DISPATCH_EVENT_BY_SESSIONID(ev);
}
