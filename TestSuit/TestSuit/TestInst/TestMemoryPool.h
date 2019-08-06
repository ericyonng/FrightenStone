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
 * @file  : TestMemoryPool.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/07/15
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestMemoryPool_H__
#define __Test_TestMemoryPool_H__

#pragma once

#include "stdafx.h"

class TestMemPoolObj
{
    MEM_POOL_CREATE(_memHelper);
public:
    TestMemPoolObj()
    {
        std::cout << "¹¹Ôì" << std::endl;
        _int = 11;
    }
    ~TestMemPoolObj()
    {
        std::cout << "Îö¹¹" << std::endl;
    }

    void Print()
    {
        std::cout << "int"<<_int << std::endl;
    }

public:
    Int64 _int;

};

MEMPOOL_CREATE_IMPL(TestMemPoolObj, _memHelper)

class TestMemoryPool
{
public:
    static void Run()
    {
        g_Log->InitModule("main");
        //g_MemoryPool->InitPool();
        printf("mem:[%p]log[%p]", g_MemoryPool, g_Log);

        TestMemPoolObj *newObj = new TestMemPoolObj;
        newObj->Print();
        delete newObj;
        newObj = new TestMemPoolObj;
        //g_MemoryPool->FinishPool();
        //g_Log->FinishModule();
    }
};

#endif
