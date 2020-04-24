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
 * @file  : TestThreadPool.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestThreadPool_H__
#define __Test_TestThreadPool_H__

#pragma once

class TestTask :public fs::ITask
{
public:
    Int32 Run()
    {
        std::cout << "hello" << *reinterpret_cast<Int32 *>(_arg) << std::endl;
        return 0;
    }
};

class MyObj
{
public:
    void CallBack(fs::FS_ThreadPool *pool)
    {
        std::cout << "this is my obj call back" << std::endl;
    }
};

class TestThreadPool
{
public:
    static void Run()
    {
        FS_MessageQueuePtr(pool);
        MyObj myObj;
//         for(auto i = 0; i < 50; ++i)
//         {
//             Int32 *arg1 = new Int32;
//             *arg1 = i;
//             TestTask *task1 = new TestTask;
//             task1->SetArg(arg1);
//             pool->AddTask(*task1);
//         }

        pool->AddTask(fs::DelegatePlusFactory::Create(&myObj, &MyObj::CallBack));
        pool->Close();
    }
};

#endif
