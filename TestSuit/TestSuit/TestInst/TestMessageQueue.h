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
 * @file  : TestMessageQueue.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/03
 * @brief :
 * 
 *  结论：带线程的消息队列在8生产者1消费者的情况下达到，消费每秒百万数据包（每个包256字节）2Gb/s消费能力 但在消费者线程小于生产者线程时候内存有被耗尽的风险
          支持每个生产者每秒17万数据包压力 8生产者8消费者下会达到250w数据包且这个时候内存消耗达到动态平衡

          无线程的消息队列在8生产者1消费者的情况下达到,消费每秒98w数据包 8生产者8消费者下会达到135w
          每个生产者最低11.2w数据包,最高

          带线程消息队列与不带线程消息队列主要区别在于锁冲突的时间长短,带线程的由于有一条闲置线程只管对消息包的转运,没有业务处理所以锁冲突时间较短性能高
 * 
 */
#ifndef __Test_TestMessageQueue_H__
#define __Test_TestMessageQueue_H__

#pragma once

#include "stdafx.h"

// 8个生产者线程 1个消费者线程
#undef TEST_GENERATOR_QUANTITY
#define TEST_GENERATOR_QUANTITY 8
#define TEST_CONSUMER_QUANTITY 1
// fs::ConcurrentMessageQueue g_testMsgQueue(TEST_GENERATOR_QUANTITY, 1);
fs::ConcurrentMessageQueueNoThread g_TestMsgNoThreadQueue(TEST_GENERATOR_QUANTITY, TEST_CONSUMER_QUANTITY);

struct TestMessage
{
    // 序列化反序列化（ObjType 必须有SerialzieTo(FS_Stream *), DeserializeFrom(FS_Stream *)接口）
public:
    TestMessage();
    bool SerializeTo(fs::FS_Stream *stream) const;
    bool DeserializeFrom(fs::FS_Stream *stream);

public:
    BUFFER256 _buffer;
};

std::atomic<UInt64> g_consumCount{0};

class ComsumerTask :public fs::ITask
{
public:
    UInt32 _id;
    fs::FS_ThreadPool *_pool;

public:
    ComsumerTask(UInt32 id, fs::FS_ThreadPool *pool):_id(id), _pool(pool) {}
    ~ComsumerTask() {}

public:
    Int32 Run() 
    {
        Handler(_pool);
        return StatusDefs::Success;
    }
public:
    void Handler(fs::FS_ThreadPool *pool)
    {
        std::vector<std::list<fs::FS_MessageBlock *> *> *generatorMsgsQueue = new std::vector<std::list<fs::FS_MessageBlock *> *>;
        generatorMsgsQueue->resize(TEST_GENERATOR_QUANTITY);
        for(UInt32 i = 0; i < TEST_GENERATOR_QUANTITY; ++i)
            (*generatorMsgsQueue)[i] = new std::list<fs::FS_MessageBlock *>;

        Int64 countMsg = 0;
        fs::Time start, end;
        start.FlushTime();
        bool isFirst = true;
        bool hasMsg = true;
        while(true)
        {
            g_TestMsgNoThreadQueue.WaitForPoping(_id, generatorMsgsQueue, hasMsg);
            if(isFirst)
            {
                isFirst = false;
                start.FlushTime();
            }

            if(!g_TestMsgNoThreadQueue.IsWorking())
            {
                if(hasMsg)
                {
                    for(UInt32 i = 0; i < TEST_GENERATOR_QUANTITY; ++i)
                    {
                        auto msgBlockList = generatorMsgsQueue->at(i);
                        if(!msgBlockList)
                            continue;

                        for(auto iterBlock = msgBlockList->begin(); iterBlock != msgBlockList->end();)
                        {
                            auto msgBlock = *iterBlock;
                            TestMessage msg;
                            msgBlock->_data->DeserializeTo(msg);
                            Fs_SafeFree(msgBlock);
                            ++countMsg;
                            iterBlock = msgBlockList->erase(iterBlock);
                        }
                    }
                }

                break;
            }
            
            if(!hasMsg)
                continue;

            for(UInt32 i = 0; i < TEST_GENERATOR_QUANTITY; ++i)
            {
                auto msgBlockList = generatorMsgsQueue->at(i);
                if(!msgBlockList)
                    continue;

                for(auto iterBlock = msgBlockList->begin(); iterBlock != msgBlockList->end();)
                {
                    auto msgBlock = *iterBlock;
                    TestMessage msg;
                    msgBlock->_data->DeserializeTo(msg);
                    Fs_SafeFree(msgBlock);
                    ++countMsg;
                    iterBlock = msgBlockList->erase(iterBlock);
                }
            }

            hasMsg = false;
        }

        end.FlushTime();
        g_consumCount += countMsg;
        g_Log->any<ComsumerTask>("comsumer[%u] end consum [%lld] totalmsg[%llu] msgs escape time[%llu]"
                                 , _id, countMsg, (UInt64)(g_consumCount), (end - start).GetTotalMicroSeconds());
    }

    static void Handler2(fs::FS_ThreadPool *pool)
    {
        std::vector<std::list<fs::FS_MessageBlock *> *> *generatorMsgsQueue = new std::vector<std::list<fs::FS_MessageBlock *> *>;
        generatorMsgsQueue->resize(TEST_GENERATOR_QUANTITY);
        for(UInt32 i = 0; i < TEST_GENERATOR_QUANTITY; ++i)
            (*generatorMsgsQueue)[i] = new std::list<fs::FS_MessageBlock *>;

        Int64 countMsg = 0;
        fs::Time start, end;
        start.FlushTime();
        bool isFirst = true;
        bool hasMsg = true;
        while(g_TestMsgNoThreadQueue.IsWorking() || hasMsg)
        {
            g_TestMsgNoThreadQueue.WaitForPoping(1, generatorMsgsQueue, hasMsg, 1);

            if(isFirst)
            {
                isFirst = false;
                start.FlushTime();
            }

            // Sleep(30000);
            for(UInt32 i = 0; i < TEST_GENERATOR_QUANTITY; ++i)
            {
                auto msgBlockList = generatorMsgsQueue->at(i);
                if(!msgBlockList)
                    continue;

                for(auto iterBlock = msgBlockList->begin(); iterBlock != msgBlockList->end();)
                {
                    auto msgBlock = *iterBlock;
                    TestMessage msg;
                    msgBlock->_data->DeserializeTo(msg);
                    Fs_SafeFree(msgBlock);
                    ++countMsg;
                    iterBlock = msgBlockList->erase(iterBlock);
                }
            }

            hasMsg = false;
        }

        end.FlushTime();
        g_Log->any<ComsumerTask>("comsumer[1] end consum [%lld] msgs escape time[%llu]"
                                 , countMsg, (end - start).GetTotalMicroSeconds());
    }
    
};

#define TEST_SESSION_CNT    25000
class GeneratorTask :fs::ITask
{
private:
    UInt32 _queueId;
    fs::FS_ThreadPool *_pool;

public:
    GeneratorTask(UInt32 queueId, fs::FS_ThreadPool *pool)
        :_queueId(queueId)
        ,_pool(pool)
    {
    }
public:
    Int32 Run()
    {
        Int64 count = 0;
        std::list<fs::FS_MessageBlock *> *msgBlocks = new std::list<fs::FS_MessageBlock *>;
        fs::Time start, end;
        start.FlushTime();
        while(_pool->IsPoolWorking())
        {
            TestMessage newMsg;
            sprintf(newMsg._buffer, "queueId[%u] ni hao messagequeue %lld", _queueId, count);
            ++count;

            fs::FS_MessageBlock *newMsgBlock = new fs::FS_MessageBlock;
            newMsgBlock->_data = new fs::FS_Stream(256);
            newMsgBlock->_data->SerializeFrom(newMsg);
            msgBlocks->push_back(newMsgBlock);

            if(g_TestMsgNoThreadQueue.IsWorking())
                g_TestMsgNoThreadQueue.Push(_queueId, msgBlocks);
            else
            {
                count -= msgBlocks->size();
                fs::STLUtil::DelListContainer(*msgBlocks);
                //g_TestMsgNoThreadQueue.NotifyConsumerByGenerator(_queueId);
            }

            // 注意notifyconsumer需要在pushunlock之后避免死锁
            //g_TestMsgNoThreadQueue.NotifyConsumerByGenerator(_queueId);
            //fs::SocketUtil::Sleep(0, 0);
        }

        if(!msgBlocks->empty())
        {
            g_Log->w<GeneratorTask>(_LOGFMT_("queue id[%u]unhandled msgs[%llu]"), _queueId, msgBlocks->size());
            fs::STLUtil::DelListContainer(*msgBlocks);
        }

        end.FlushTime();
        g_Log->any<GeneratorTask>("queue id[%u] count[%lld] thread end eslcape time[%lld]"
                                  , _queueId, count, (end - start).GetTotalMicroSeconds());
        return StatusDefs::Success;
    }
};


inline TestMessage::TestMessage()
{
    memset(_buffer, 0, sizeof(_buffer));
}

inline bool TestMessage::SerializeTo(fs::FS_Stream *stream) const
{
    auto writeSteam = stream->CastTo<fs::FS_MsgWriteStream>();
    return writeSteam->Write(_buffer);
}

inline bool TestMessage::DeserializeFrom(fs::FS_Stream *stream)
{
    auto readStream = stream->CastTo<fs::FS_MsgReadStream>();
    return readStream->Read(_buffer);
}

class TestMessageQueue
{
public:
    static void Run()
    {
        // 创建一个接收并处理消息的线程
        fs::TimeUtil::SetTimeZone();
        fs::SmartVarRtti::InitRttiTypeNames();
        fs::FS_TlsUtil::CreateUtilTlsHandle();
        g_Log->InitModule(NULL);
#ifdef _WIN32
        fs::CrashHandleUtil::InitCrashHandleParams();
#endif
        g_MemleakMonitor->Start(100000000, 100000000);

        g_TestMsgNoThreadQueue.BeforeStart();
        g_TestMsgNoThreadQueue.Start();
//         g_testMsgQueue.BeforeStart();
//         g_testMsgQueue.Start();

        fs::FS_ThreadPool *pool = new fs::FS_ThreadPool(0, TEST_GENERATOR_QUANTITY + TEST_CONSUMER_QUANTITY);
        for(UInt32 i = 0; i < TEST_CONSUMER_QUANTITY; ++i)
        {
            auto consumTask = new ComsumerTask(i, pool);
            pool->AddTask(*consumTask, true);
        }
//         auto comsumerTask2 = fs::DelegatePlusFactory::Create(&ComsumerTask::Handler2);
//         pool->AddTask(comsumerTask2, true);
        
        for(UInt32 i = 0; i < TEST_GENERATOR_QUANTITY; ++i)
        {
            fs::ITask *newTask = reinterpret_cast<fs::ITask *>(new GeneratorTask(i, pool));
            pool->AddTask(*newTask, true);
        }
        
        getchar();
        g_Log->any<TestMessageQueue>(_LOGFMT_("will close all test"));
        //g_testMsgQueue.BeforeClose();
        g_TestMsgNoThreadQueue.BeforeClose();
        pool->Close();
        //g_testMsgQueue.Close();
        g_TestMsgNoThreadQueue.Close();

        // 主线程发送间隔一秒钟发送一个消息，消费者线程需要打印出消息
    }
};

#endif
