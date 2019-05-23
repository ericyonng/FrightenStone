#ifndef __Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
#define __Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
/**
* @file ConditionLocker.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once
#include<base\exportbase.h>
#include <base\common\common.h>
#include "base\common\asyn\Lock\Impl\Locker.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT ConditionLocker : public Locker
{
public:
    ConditionLocker();
    virtual ~ConditionLocker();

#pragma region sinal
public:
    int Wait(unsigned long milisec = INFINITE);
    bool Sinal();
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

FS_NAMESPACE_END

#endif // !__Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
