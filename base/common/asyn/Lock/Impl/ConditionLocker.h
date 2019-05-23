#ifndef __Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
#define __Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
/**
* @file ConditionLocker.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief 条件锁（带唤醒与等待）
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

#pragma region 信号
public:
    int Wait(unsigned long milisec = INFINITE);     // 外部需要加锁
    bool Sinal();                                   // 外部需要加锁
    void Broadcast();                               // 唤醒所有
    void ResetSinal();
#pragma endregion

    #pragma region 初始化以及销毁匿名event对象
private:
    bool _InitAnonymousEvent();
    bool _DestroyEvent();
    #pragma endregion

#pragma region 数据成员
private:
    std::atomic<void *> _event;
    std::atomic<bool> _isSinal;
    std::atomic<long long> _waitCnt;
#pragma endregion
};

FS_NAMESPACE_END

#endif // !__Base_Common_Asyn_Lock_Impl_ConditionLocker_H__
