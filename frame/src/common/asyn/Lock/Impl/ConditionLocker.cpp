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
#include "FrightenStone/common/asyn/Lock/Defs/MetaLocker.h"
#include "FrightenStone/common/asyn/Lock/Impl/ConditionLocker.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/Time.h"

#ifdef _WIN32
#include <process.h>

#undef FS_IS_EVENT_SINAL_WAKE_UP
#define FS_IS_EVENT_SINAL_WAKE_UP(waitRet)   \
(static_cast<long long>(WAIT_OBJECT_0) <= (waitRet)) &&\
((waitRet) <= static_cast<long long>(MAXIMUM_WAIT_OBJECTS + WAIT_OBJECT_0))
#else
#include<semaphore.h>
#include<sys/time.h>
#include<stdlib.h>
#include<string.h>
#endif

FS_NAMESPACE_BEGIN

ConditionLocker::ConditionLocker()
    :_waitCnt{0}
#ifdef _WIN32
    ,_event{NULL}
#endif
    , _isSinal{false}
{
    _Init();
}

ConditionLocker::~ConditionLocker()
{
    _Destroy();
}

static const Int64 g_nanoSecPerSecond = Time::_microSecondPerSecond * 1000;
Int32 ConditionLocker::Wait(UInt64 second, UInt64 microSec)
{
#ifdef _WIN32
    long long waitRet = WAIT_OBJECT_0;
    auto *event = _event.load();
    bool oldSinal = _isSinal;
    while(!_isSinal)
    {
        Unlock();
        ++_waitCnt;
        waitRet = WaitForMultipleObjects(1, &event, true, static_cast<DWORD>(second*Time::_millisecondPerSecond + microSec / Time::_microSecondPerMilliSecond));
        Lock();
        --_waitCnt;

        // 不论是否被唤醒都重置事件避免消耗
        ResetEvent(_event.load());

        if(waitRet == WAIT_TIMEOUT)
        {// 无论是否被唤醒（因为唤醒的时机恰好是超时）超时被唤醒
            _isSinal = false;
            return StatusDefs::WaitTimeOut;
        }

        // 出现错误则直接return
        if(!FS_IS_EVENT_SINAL_WAKE_UP(waitRet))
        {
            _isSinal = false;
            return StatusDefs::WaitFailure;
        }
    }

    if(oldSinal)
        ResetEvent(_event.load());

    _isSinal = false;
    return StatusDefs::Success;
#else
    struct timespec abstime; // nsec是时刻中的纳秒部分，注意溢出
    Int64 secInc = second + microSec / Time::_microSecondPerSecond;
    Int64 nanoSecInc = microSec % Time::_microSecondPerSecond * 1000;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += secInc;
    abstime.tv_nsec += static_cast<Long>(nanoSecInc);

    // 纳秒部分溢出1秒
    Int64 curNanoSecPart = static_cast<Int64>(abstime.tv_nsec);
    abstime.tv_sec += curNanoSecPart / g_nanoSecPerSecond;
    curNanoSecPart %= g_nanoSecPerSecond;
    abstime.tv_nsec = static_cast<Long>(curNanoSecPart);

    ++_waitCnt;
    int ret = pthread_cond_timedwait(&_condVal, &_metaLocker.load()->_handle, &abstime);
    if(ret == ETIMEDOUT)
    {
        --_waitCnt;
        return StatusDefs::WaitTimeOut;
    }

    if(ret != 0)
    {
        --_waitCnt;
        printf("\nret=%d\n", ret);
        perror("pthread cond timewait error");
        return StatusDefs::WaitFailure;
    }

    --_waitCnt;
    return StatusDefs::Success;
#endif
}

Int32 ConditionLocker::Wait(UInt64 milliSecond)
{
#ifdef _WIN32
    long long waitRet = WAIT_OBJECT_0;
    auto *event = _event.load();
    bool oldSinal = _isSinal;
    while(!_isSinal)
    {
        Unlock();
        ++_waitCnt;
        waitRet = WaitForMultipleObjects(1, &event, true, static_cast<DWORD>(milliSecond));
        Lock();
        --_waitCnt;

        // 不论是否被唤醒都重置事件避免消耗
        ResetEvent(_event.load());

        if(waitRet == WAIT_TIMEOUT)
        {// 无论是否被唤醒（因为唤醒的时机恰好是超时）超时被唤醒
            _isSinal = false;
            return StatusDefs::WaitTimeOut;
        }

        // 出现错误则直接return
        if(!FS_IS_EVENT_SINAL_WAKE_UP(waitRet))
        {
            _isSinal = false;
            return StatusDefs::WaitFailure;
        }
    }

    if(oldSinal)
        ResetEvent(_event.load());

    _isSinal = false;
    return StatusDefs::Success;
#else
    if(milliSecond == INFINITE)
        return DeadWait();

    struct timespec abstime; // nsec是时刻中的纳秒部分，注意溢出
    Int64 secInc = milliSecond / Time::_millisecondPerSecond;
    Int64 nanoSecInc = (milliSecond % Time::_millisecondPerSecond)*Time::_microSecondPerMilliSecond * 1000;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += secInc;
    abstime.tv_nsec += static_cast<Long>(nanoSecInc);

    // 纳秒部分溢出1秒
    Int64 curNanoSecPart = static_cast<Int64>(abstime.tv_nsec);
    abstime.tv_sec += curNanoSecPart / g_nanoSecPerSecond;
    curNanoSecPart %= g_nanoSecPerSecond;
    abstime.tv_nsec = static_cast<Long>(curNanoSecPart);

    ++_waitCnt;
    int ret = pthread_cond_timedwait(&_condVal, &_metaLocker.load()->_handle, &abstime);
    if(ret == ETIMEDOUT)
    {
        --_waitCnt;
        return StatusDefs::WaitTimeOut;
    }

    if(ret != 0)
    {
        --_waitCnt;
        printf("\nret=%d\n", ret);
        perror("pthread cond timewait error");
        return StatusDefs::WaitFailure;
    }

    --_waitCnt;
    return StatusDefs::Success;
#endif
}

Int32 ConditionLocker::DeadWait()
{
#ifdef _WIN32
    long long waitRet = WAIT_OBJECT_0;
    auto *event = _event.load();
    bool oldSinal = _isSinal;
    while(!_isSinal)
    {
        Unlock();
        ++_waitCnt;
        waitRet = WaitForMultipleObjects(1, &event, true, INFINITE);
        Lock();
        --_waitCnt;

        // 不论是否被唤醒都重置事件避免消耗
        ResetEvent(_event.load());

        if(waitRet == WAIT_TIMEOUT)
        {// 无论是否被唤醒（因为唤醒的时机恰好是超时）超时被唤醒
            _isSinal = false;
            return StatusDefs::WaitTimeOut;
        }

        // 出现错误则直接return
        if(!FS_IS_EVENT_SINAL_WAKE_UP(waitRet))
        {
            _isSinal = false;
            return StatusDefs::WaitFailure;
        }
    }

    if(oldSinal)
        ResetEvent(_event.load());

    _isSinal = false;
    return StatusDefs::Success;
#else
    ++_waitCnt;
    if(!_isSinal)
    {
        auto ret = pthread_cond_wait(&_condVal, &_metaLocker.load()->_handle);
        if(ret != 0)
        {
            --_waitCnt;
            printf("\nret=%d\n", ret);
            perror("cConSync::WaitCon -error waitcon");
            _isSinal = false;
            return StatusDefs::WaitFailure;
        }
    }

    _isSinal = false;
    --_waitCnt;
    return StatusDefs::Success;
#endif
}

bool ConditionLocker::Sinal()
{
#ifdef _WIN32
    _isSinal = SetEvent(_event.load());
    return _isSinal.load();
#else
    int ret = pthread_cond_signal(&_condVal);
    if(ret != 0)
    {
        printf("\nret=%d\n", ret);
        perror("signal fail\n");
        return false;
    }

    _isSinal = true;

    return true;
#endif
}

void ConditionLocker::Broadcast()
{
#ifdef _WIN32
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
#else
    int ret = pthread_cond_broadcast(&_condVal);
    if(ret != 0)
    {
        printf("\nret=%d\n", ret);
        perror("cond broadcast error");
    }
#endif
}

void ConditionLocker::ResetSinal()
{
#ifdef _WIN32
    ResetEvent(_event.load());
#endif
}

bool ConditionLocker::_Init()
{
#ifdef _WIN32
    _event = CreateEvent(NULL, true, false, NULL);
    if(UNLIKELY(!_event))
        return false;
#else
    int ret = pthread_cond_init(&_condVal, NULL);
    if(ret != 0)
    {
        printf("\nret=%d\n", ret);
        perror("cond init error!");
        return false;
    }
#endif
    return true;
}

bool ConditionLocker::_Destroy()
{
#ifdef _WIN32
    if(UNLIKELY(!_event.load()))
        return true;

    Broadcast();

    if(UNLIKELY(!CloseHandle(_event.load())))
        return false;

    _event = NULL;

    return true;
#else
    int ret = pthread_cond_destroy(&_condVal);
    if(ret != 0)
    {
        printf("\nret=%d\n", ret);
        perror("cond destroy error");
        return false;
    }
    return true;
#endif
}

FS_NAMESPACE_END
