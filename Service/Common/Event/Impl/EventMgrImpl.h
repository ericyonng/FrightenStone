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
 * @file  : GlobalEventMgrImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/13
 * @brief :
 */
#ifdef __Service_Common_Event_Impl_EventMgr_H__
#pragma once

inline void EventMgr::FireEvent(Int32 id)
{
    FireEvent(new fs::FS_Event(id));
}

template <typename Filter>
inline Int64 EventMgr::AddEventFilter()
{
    ++_curEventFilterId;
    auto *filter = new Filter();
    filter->Register(*this);
    _eventFilters.insert(std::make_pair(_curEventFilterId, filter));
    return _curEventFilterId;
}

inline void EventMgr::RemoveEventFilter(Int64 filterId)
{
    auto it = _eventFilters.find(filterId);
    if(it == _eventFilters.end())
        return;

    it->second->UnRegister(*this);
    Fs_SafeFree(it->second);
    _eventFilters.erase(it);
}

#endif
