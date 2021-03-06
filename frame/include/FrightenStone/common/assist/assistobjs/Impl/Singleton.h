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
 * filename: Singleton.h
 *
 * author  : Eric Yonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_Singleton_H__
#define __Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_Singleton_H__
/**
* @file Singleton.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once

#include<FrightenStone/exportbase.h>
#include "FrightenStone/common/assist/assistobjs/Impl/SmartPtr.h"
#include <memory>
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

// 单例请确保在程序进行全局对象初始化时候不适用singleton或者，全局初始化期间不调用单例创建,避免单例中锁可能因为初始化顺序而未来得及创建
template<typename ObjType, AssistObjsDefs::DelMethods delMethod = AssistObjsDefs::Delete>
class Singleton
{
public:
    static ObjType *GetInstance();
    template<typename... Args>
    static ObjType *GetInstance(Args... args);

private:
    Singleton(void) = delete;
    ~Singleton(void) = delete;
    Singleton(const Singleton<ObjType>&) = delete;
    Singleton(Singleton<ObjType>&&) = delete;
    Singleton& operator = (const Singleton<ObjType>&) = delete;

private:
    static SmartPtr<ObjType, delMethod> _pObj;
    static Locker _lock;
};

FS_NAMESPACE_END

#include "FrightenStone/common/assist/assistobjs/Impl/SingletonImpl.h"

#endif //!__Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_Singleton_H__