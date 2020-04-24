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
 * @file  : GlobalEventMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/13
 * @brief :
 */
#include "stdafx.h"
#include "Service/Common/Event/Impl/EventMgr.h"

#define FS_EVENT_GIVE_WARNING_QUANTITY      100         // 当某个事件被监听达到100个以上时需要打印日志警告

EventMgr::EventMgr()
    :_curEventFilterId(0)
{

}
EventMgr::~EventMgr()
{
    fs::STLUtil::DelMapContainer(_eventFilters);
}

void EventMgr::FireEvent(fs::FS_Event *ev)
{
    BeforeFireEvent();
    _FireEvent(ev);
    AfterFireEvent();

    if(!ev->IsDontDelAfterFire())
        Fs_SafeFree(ev);
}

void EventMgr::_FireEvent(fs::FS_Event *ev)
{
    _ListenersMap::iterator mIt = _listeners.find(ev->GetId());
    if(mIt != _listeners.end())
    {
        _Listeners &listeners = mIt->second;

        size_t listenerSz = listeners.size();
        // 当一个事件的监听数量大于100时给个warning打印
        if(listenerSz > FS_EVENT_GIVE_WARNING_QUANTITY)
            g_Log->w<EventMgr>(_LOGFMT_("_FireEvent listener sz %u, eventId %d"), listenerSz, ev->GetId());

        for(_Listeners::iterator lIt = listeners.begin();
            lIt != listeners.end();
            lIt++)
        {
            _Listener &listener = *lIt;
            listener._listenCallBack->Invoke(ev);
        }
    }
}

