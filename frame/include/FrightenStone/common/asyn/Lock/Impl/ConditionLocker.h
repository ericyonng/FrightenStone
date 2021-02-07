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
 * @file  : ConditionLocker.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Asyn_Lock_Impl_ConditionLocker_H__
#define __Frame_Include_FrightenStone_Common_Asyn_Lock_Impl_ConditionLocker_H__


#pragma once
#include<FrightenStone/exportbase.h>
#include "FrightenStone/common/basedefs/DataType/ForAll/ForAll.h"
#include <FrightenStone/common/asyn/Lock/Impl/Locker.h>

FS_NAMESPACE_BEGIN

// locker是默认锁，linux下由mutex实现，windows下使用自旋锁实现
class BASE_EXPORT ConditionLocker : public Locker
{
public:
    ConditionLocker();
    virtual ~ConditionLocker();

#pragma region sinal
public:
    // 支持多线程等待sinal一次只唤醒一个线程 INFINITE
    Int32 Wait(UInt64 second, UInt64 microSec);
    Int32 Wait(UInt64 milliSecond = INFINITE);
    Int32 DeadWait();
    bool Sinal();
    bool HasWaiter();
    void Broadcast();
    void ResetSinal();
    void ResetSinalFlag();
#pragma endregion

    #pragma region inner
private:
    bool _Init();
    bool _Destroy();
    #pragma endregion

    #pragma region member
private:
    std::atomic<long long> _waitCnt;
    std::atomic<bool> _isSinal;

    #ifdef _WIN32
        std::atomic<void *> _event;
    #else
        pthread_cond_t _condVal;
    #endif
    #pragma endregion
};

inline bool ConditionLocker::HasWaiter()
{
    return _waitCnt > 0;
}

inline void ConditionLocker::ResetSinalFlag()
{
    _isSinal = false;
}

FS_NAMESPACE_END

#endif // !__Frame_Include_FrightenStone_Common_Asyn_Lock_Impl_ConditionLocker_H__
