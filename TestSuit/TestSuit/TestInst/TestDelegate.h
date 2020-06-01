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
 * @file  : TestDelegate.h
 * @author: Eric Yonng<120453674@qq.com>
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

class Test1
{
public:
    void print()
    {
        std::cout << "hello world"<< std::endl;
    }

    static void print2()
    {
        std::cout << "print2" << std::endl;
    }
    static void print32()
    {
        std::cout << "print32" << std::endl;
    }
};

static void print3()
{
    std::cout << "print3" << std::endl;
}

static void print4(int a, int b)
{
    std::cout << "print3"<< a << b << std::endl;
}

static void print5(const int &a, const int &b)
{
    std::cout << "print3" << a << b << std::endl;
}


class TestDelegate
{
public:
    static void Run()
    {
        g_Log->InitModule("testdelegate");
#ifdef _WIN32
        fs::CrashHandleUtil::InitCrashHandleParams();
#endif
        Test1 test1;
        fs::IDelegate<void> *delegate = fs::DelegatePlusFactory::Create(&test1, &Test1::print);
        delegate->Invoke();
        delete delegate;
        auto delegate3 = fs::DelegatePlusFactory::Create(print3);
        delegate3->Invoke();
        auto delegate4 = fs::DelegatePlusFactory::Create(&print3);
        delegate4->Invoke();
        auto delegate5 = fs::DelegatePlusFactory::Create(&Test1::print2);
        delegate5->Invoke();

        auto delegate6 = fs::DelegatePlusFactory::Create(&print5);
        delegate6->Invoke(1, 10);
        const int a = 1, b = 10;
        delegate6->Invoke(a, b);

        std::function<void()> func = Test1::print32;

        auto __lambda = [&test1](int i)->void
        {
            std::cout << "this is lambda" << "i:" << i << std::endl;
            return test1.print();
        };
        
        auto delegate7 = fs::DelegatePlusFactory::Create<decltype(func), void>(func);
        delegate7->Invoke();

        auto delegate8 = fs::DelegatePlusFactory::Create<decltype(__lambda), void, int>(__lambda);
        delegate8->Invoke(1);
        delegate8->Invoke(1);
        delegate8->Invoke(1);

        // 创建委托副本
        auto delegate9 = delegate8->CreateNewCopy();
        delegate9->Invoke(9);
    }
};

#endif

