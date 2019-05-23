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

class TestThreadPool
{
public:
    static void Run()
    {
        FS_MessageQueuePtr(pool);
        for(auto i = 0; i < 50; ++i)
        {
            Int32 *arg1 = new Int32;
            *arg1 = i;
            TestTask *task1 = new TestTask;
            task1->SetArg(arg1);
            pool->AddTask(*task1);
        }
        pool->Clear();
    }
};

#endif
