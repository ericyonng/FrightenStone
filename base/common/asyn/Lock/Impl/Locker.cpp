/**
* @file Lock.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/25
* @brief
*/

#include "stdafx.h"
#include "base/common/asyn/Lock/Impl/Locker.h"
#include "base/common/asyn/Lock/Defs/MetaLocker.h"

#include "base/common/assist/assist.h"

FS_NAMESPACE_BEGIN

Locker::Locker()
    :_metaLocker(NULL)
{
    _Init();
}

Locker::~Locker()
{
    auto obj = _metaLocker.load();
    if(LIKELY(_metaLocker.load()))
    {
        _metaLocker = NULL;
        delete obj;
        obj = NULL;
    }
}

void Locker::Lock()
{
    if(UNLIKELY(!_metaLocker.load()))
        _Init();

    EnterCriticalSection(&_metaLocker.load()->_handle);
}

void Locker::Unlock()
{
    if(UNLIKELY(!_metaLocker.load()))
        _Init();

    LeaveCriticalSection(&_metaLocker.load()->_handle);
}

bool Locker::TryLock()
{
    if(UNLIKELY(!_metaLocker.load()))
        _Init();

    return TryEnterCriticalSection(&_metaLocker.load()->_handle);
}

bool Locker::Islock() const
{
    auto curThread = ULongToHandle(GetCurrentThreadId());
    return _metaLocker.load()->_handle.OwningThread != NULL && _metaLocker.load()->_handle.OwningThread != curThread;
}

void Locker::_Init()
{
    if(UNLIKELY(_metaLocker.load()))
        return;

    _metaLocker = new MetaLocker;

    //设置循环锁减少加锁时付出的上下文切换代价（上千个时钟周期）
    auto spinCnt = static_cast<DWORD>(1 << (WidthUtil::GetBinaryWidth<DWORD>() - 1)) | static_cast<DWORD>(SPINNING_COUNT);     //最高位置1使内核对象进程加载时创建，避免内存不足时候创建失败
    if(!InitializeCriticalSectionAndSpinCount(&(_metaLocker.load()->_handle), spinCnt))
        printf("MetaLocker create spinlock fail spinCnt[%lu]", spinCnt);
}

FS_NAMESPACE_END
