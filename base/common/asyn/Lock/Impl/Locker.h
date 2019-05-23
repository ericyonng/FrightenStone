#ifndef __Base_Common_Asyn_Lock_Impl_Locker_H__
#define __Base_Common_Asyn_Lock_Impl_Locker_H__
/**
* @file Lock.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/25
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include <base/common/common.h>
#include "base/common/assist/assistobjs/Defs/AssistObjsDefs.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/Resource/Resource.h"

class BASE_EXPORT MetaLocker;

FS_NAMESPACE_BEGIN

class BASE_EXPORT Locker
{
public:
    Locker();
    virtual ~Locker();

public:
    void Lock();
    void Unlock();
    bool TryLock();
    bool Islock() const;

private:
    void _Init();

private:
    std::atomic<MetaLocker *> _metaLocker;
};
FS_NAMESPACE_END

#endif // !__Base_Common_Asyn_Lock_Impl_Locker_H__
