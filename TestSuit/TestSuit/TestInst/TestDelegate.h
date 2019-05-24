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
 * @file  : TestDelegate.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestDelegate_H__
#define __Test_TestDelegate_H__

#pragma once
#include "stdafx.h"
#include "base/common/component/Impl/FS_ClassFactory.h"
#include "base/common/component/Impl/FS_Delegate.h"

template <class T, class R, typename... Args>
class  MyDelegate
{
public:
    MyDelegate(T* t, R(T::*f)(Args...)):m_t(t), m_f(f) {}

    R operator()(Args&&... args)
    {
        return (m_t->*m_f)(std::forward<Args>(args) ...);
    }

private:
    T* m_t;
    R(T::*m_f)(Args...);
};

template <class T, class R, typename... Args>
MyDelegate<T, R, Args...> CreateDelegate(T* t, R(T::*f)(Args...))
{
    return MyDelegate<T, R, Args...>(t, f);
}

struct A
{
    void Fun(int i) { std::cout << i << std::endl; }
    int Fun1(int &i, double &j) { std::cout << i + j << std::endl; return 0; }
};

struct bbs
{
    union
    {
        void *_voidPtr;
        std::map<int, int> *_mapPtr;
        double *_doublePtr;
    }_ptr;

    double _raw;
};

class TestDelegate
{
public:
    static void Run()
    {
        bbs ddn;
        memset(&ddn, 0, sizeof(ddn));
        ddn._ptr._voidPtr = new bbs;
        ddn._ptr._mapPtr->insert(std::make_pair(1, 1));
        std::map<int, int> a;
        std::cout << sizeof(a) << std::endl;
    }
};

#endif

