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
 * filename: Singleton.h
 *
 * author  : ericyonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
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
    static Locker          _locker;
};

FS_NAMESPACE_END

#include "base/common/assist/assistobjs/Impl/SingletonImpl.h"

#endif //!__Base_Common_Assist_AssistObjs_Impl_Singleton_H__