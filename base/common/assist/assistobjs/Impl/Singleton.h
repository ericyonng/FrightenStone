#ifndef __Base_Common_Assist_AssistObjs_Impl_Singleton_H__
#define __Base_Common_Assist_AssistObjs_Impl_Singleton_H__
/**
* @file Singleton.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once

#include<base\exportbase.h>
#include "base/common/asyn/asyn.h"
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

template<typename T>
class Singleton
{
public:
    static T *GetInstance();

private:
    Singleton(void) = delete;
    ~Singleton(void) = delete;
    Singleton(const Singleton<T>&) = delete;
    Singleton(Singleton<T>&&) = delete;
    Singleton& operator = (const Singleton<T>&) = delete;

private:
    static std::auto_ptr<T> _pObj;
    static Locker          _locker;     // 担心释放的问题
};

FS_NAMESPACE_END

#include "base/common/assist/assistobjs/Impl/SingletonImpl.h"

#endif //!__Base_Common_Assist_AssistObjs_Impl_Singleton_H__