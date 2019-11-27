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
 * @file  : ConditionLocker.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "FrightenStone/common/asyn/Lock/Impl/ConditionLocker.h"
#include "FrightenStone/common/status/status.h"
#include <process.h>
#include "FrightenStone/common/basedefs/BaseDefs.h"

#undef FS_IS_EVENT_SINAL_WAKE_UP
#define FS_IS_EVENT_SINAL_WAKE_UP(waitRet)   \
(static_cast<long long>(WAIT_OBJECT_0) <= (waitRet)) &&\
((waitRet) <= static_cast<long long>(MAXIMUM_WAIT_OBJECTS + WAIT_OBJECT_0))

FS_NAMESPACE_BEGIN

ConditionLocker::ConditionLocker()
    :_event(NULL)
    ,_isSinal(false)
    ,_waitCnt(0)
{
    _InitAnonymousEvent();
}

ConditionLocker::~ConditionLocker()
{
    _DestroyEvent();
}

int ConditionLocker::Wait(unsigned long milisec /*= INFINITE*/)
{
    long long waitRet = WAIT_OBJECT_0;
    auto *event = _event.load();
    bool oldSinal = _isSinal;
    while(!_isSinal)
    {
        Unlock();
        ++_waitCnt;
        waitRet = WaitForMultipleObjects(1, &event, true, milisec);
        Lock();
        --_waitCnt;

        // 不论是否被唤醒都重置事件避免消耗
        ResetEvent(_event.load());

        if(waitRet == WAIT_TIMEOUT)
        {// 无论是否被唤醒（因为唤醒的时机恰好是超时）超时被唤醒
            _isSinal = false;
            return StatusDefs::WaitEventTimeOut;
        }

        // 出现错误则直接return
        if(!FS_IS_EVENT_SINAL_WAKE_UP(waitRet))
        {
            _isSinal = false;
            return StatusDefs::WaitEventFailure;
        }
    }

    if(oldSinal)
        ResetEvent(_event.load());

    _isSinal = false;
    return StatusDefs::Success;
// 
//     if((static_cast<long long>(WAIT_OBJECT_0) <= waitRet) &&
//         (waitRet <= static_cast<long long>(MAXIMUM_WAIT_OBJECTS + WAIT_OBJECT_0)))
//     {// 等待是在合法的返回值（64个内核对象）
//         return StatusDefs::Success;
//     }
//     else if(waitRet == WAIT_TIMEOUT)
//     {// 超时等待
//         return StatusDefs::WaitEventTimeOut;
//     }
// 
//     return StatusDefs::WaitEventFailure;
}

bool ConditionLocker::Sinal()
{
    _isSinal = SetEvent(_event.load());
    return _isSinal.load();
}

void ConditionLocker::Broadcast()
{
    bool isSinal = false;
    while(_waitCnt > 0)
    {
        Lock();

        if(LIKELY(Sinal()))
            isSinal = true;

        Unlock();
        Sleep(0);
    }

    if(LIKELY(isSinal))
    {
        _isSinal = false;
        ResetEvent(_event.load());
    }
}

void ConditionLocker::ResetSinal()
{
    ResetEvent(_event.load());
}

bool ConditionLocker::_InitAnonymousEvent()
{
    _event = CreateEvent(NULL, true, false, NULL);
    if(UNLIKELY(!_event))
        return false;

    return true;
}

bool ConditionLocker::_DestroyEvent()
{
    if(UNLIKELY(!_event.load()))
        return true;

    Broadcast();

    if(UNLIKELY(!CloseHandle(_event.load())))
        return false;

    _event = NULL;

    return true;
}

FS_NAMESPACE_END
