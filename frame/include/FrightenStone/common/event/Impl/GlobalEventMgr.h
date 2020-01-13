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
 * @file  : GlobalEventMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/13
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Event_Impl_GlobalEventMgr_H__
#define __Frame_Include_FrightenStone_Common_Event_Impl_GlobalEventMgr_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/event/Impl/EventManager.h"
#include "FrightenStone/common/event/Impl/FS_Event.h"
#include "FrightenStone/common/event/Impl/IFS_EventFilter.h"

FS_NAMESPACE_BEGIN

class IFS_EventFilter;
class FS_Event;

class BASE_EXPORT GlobalEventMgr :public EventManager
{
public:
    GlobalEventMgr();
    ~GlobalEventMgr();

public:
    void FireEvent(Int32 id);
    virtual void FireEvent(FS_Event *ev);

public:
    template <typename Filter>
    Int64 AddEventFilter();

    void RemoveEventFilter(Int64 filterId);

private:
    void _FireEvent(FS_Event *ev);

private:
    Int64 _curEventFilterId;
    std::map<Int64, IFS_EventFilter *> _eventFilters;          // ÊÂ¼þ¹ýÂËÆ÷
};

FS_NAMESPACE_END

#include "FrightenStone/common/event/Impl/GlobalEventMgrImpl.h"

#endif
