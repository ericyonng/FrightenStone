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
 * @file  : Locker.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
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

  // Fs_SafeFree(_metaLocker);
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

    // 创建自旋锁，避免线程频繁挂起
    auto spinCnt = static_cast<DWORD>(1 << (WidthUtil::GetBinaryWidth<DWORD>() - 1)) | static_cast<DWORD>(SPINNING_COUNT);
    if(!InitializeCriticalSectionAndSpinCount(&(_metaLocker.load()->_handle), spinCnt))
        printf("MetaLocker create spinlock fail spinCnt[%lu]", spinCnt);
//     auto spinCnt = static_cast<DWORD>(1 << (WidthUtil::GetBinaryWidth<DWORD>() - 1)) | static_cast<DWORD>(SPINNING_COUNT);
//     if(!InitializeCriticalSectionAndSpinCount(&(_metaLocker.load()->_handle), spinCnt))
//         printf("MetaLocker create spinlock fail spinCnt[%lu]", spinCnt);
    // InitializeCriticalSection(&(_metaLocker.load()->_handle)); // 非自旋锁

}

FS_NAMESPACE_END
