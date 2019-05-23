#ifndef __Base_Common_Asyn_Lock_Defs_MetaLocker_H__
#define __Base_Common_Asyn_Lock_Defs_MetaLocker_H__
/**
* @file MetaLocker.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/25
* @brief 使用windows临界区实现，轻量级锁
*/

#pragma once
#include "base/common/common.h"
#include "base/common/basedefs/BaseDefs.h"

class MetaLocker
{
public:
    MetaLocker();
    virtual ~MetaLocker();

public:
    LockerHandle _handle;    // 锁对象
};

#endif // !__Base_Common_Asyn_Lock_Defs_MetaLocker_H__
