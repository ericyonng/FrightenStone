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
 * @file  : ConditionLocker.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
#define __Base_Common_Asyn_Lock_Impl_ConditionLocker_H__


#pragma once
#include<base/exportbase.h>
#include <base/common\common.h>
#include <base/common\asyn\Lock\Impl\Locker.h>

FS_NAMESPACE_BEGIN

// locker是默认锁，linux下由mutex实现，windows下使用自旋锁实现
class BASE_EXPORT ConditionLocker : public Locker
{
public:
    ConditionLocker();
    virtual ~ConditionLocker();

#pragma region sinal
public:
    // 支持多线程等待sinal一次只唤醒一个线程
    int Wait(unsigned long milisec = INFINITE);
    bool Sinal();
    bool HasWaiter();
    void Broadcast();
    void ResetSinal();
#pragma endregion

    #pragma region inner
private:
    bool _InitAnonymousEvent();
    bool _DestroyEvent();
    #pragma endregion

#pragma region member
private:
    std::atomic<void *> _event;
    std::atomic<bool> _isSinal;
    std::atomic<long long> _waitCnt;
#pragma endregion
};

inline bool ConditionLocker::HasWaiter()
{
    return _waitCnt > 0;
}
FS_NAMESPACE_END

#endif // !__Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
