/**
* @file ConditionLocker.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief 条件锁（带唤醒与等待）
*/

#include "stdafx.h"
#include "base/common/asyn/Lock/Impl/ConditionLocker.h"
#include "base/common/status/status.h"
#include <process.h>

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
    while(!_isSinal)
    {
        auto *event = _event.load();
        if(UNLIKELY( !event))
        {
            _isSinal = false;
            return StatusDefs::WaitEventFailure;
        }

        Unlock();

        ++_waitCnt;
        waitRet = WaitForMultipleObjects(1, &event, true, milisec);

        Lock();
        ResetEvent(_event.load());
        --_waitCnt;

        if(UNLIKELY(waitRet == WAIT_FAILED))
        {
            _isSinal = false;
            return StatusDefs::WaitEventFailure;
        }

        // 没有被人唤醒 一般做超时等待的比较少所以这边，超时等待的情形的性能相较于其他性能低一点点
        if(UNLIKELY(!_isSinal &&
                    milisec != INFINITE))
            break;
    }

    _isSinal = false; 
    if((static_cast<long long>(WAIT_OBJECT_0) <= waitRet) &&
        (waitRet <= static_cast<long long>(MAXIMUM_WAIT_OBJECTS + WAIT_OBJECT_0)))
    {
        return StatusDefs::Success;
    }
    else if(waitRet == WAIT_TIMEOUT)
    {
        return StatusDefs::WaitEventTimeOut;
    }
    
    return StatusDefs::WaitEventFailure;
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
        ResetEvent(_event.load());
}

void ConditionLocker::ResetSinal()
{
    ResetEvent(_event.load());
}

bool ConditionLocker::_InitAnonymousEvent()
{
    // 这里创建一个嗯匿名的event对象主要是为了避免具名带来的冲突
    _event = CreateEvent(NULL, true, false, NULL);    // 创建一个匿名的手动重置的event对象，初始无状态
    if(UNLIKELY(!_event))
        return false;

    return true;
}

bool ConditionLocker::_DestroyEvent()
{
    if(UNLIKELY(!_event.load()))
        return true;

    if(UNLIKELY(!CloseHandle(_event.load())))
        return false;

    _event = NULL;

    return true;
}

FS_NAMESPACE_END
