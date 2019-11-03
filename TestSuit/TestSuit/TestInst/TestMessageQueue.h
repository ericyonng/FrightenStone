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
 *
 * 
 */
#ifndef __Test_TestMessageQueue_H__
#define __Test_TestMessageQueue_H__

#pragma once

#include "stdafx.h"

fs::MessageQueue g_testMsgQueue;

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

class ComsumerTask
{
public:
    static void Handler(fs::FS_ThreadPool *pool)
    {
        std::list<fs::FS_MessageBlock *> msgBlocks;
        while(pool->IsPoolWorking())
        {
            g_testMsgQueue.PopLock();
            g_testMsgQueue.WaitForPoping(msgBlocks);
            g_testMsgQueue.PopUnlock();

            Sleep(30000);
            for(auto iterMsgBlock = msgBlocks.begin(); iterMsgBlock != msgBlocks.end();)
            {
                auto msgBlock = *iterMsgBlock;
                TestMessage msg;
                msgBlock->_data->DeserializeTo(msg);
                std::cout << msg._buffer << std::endl;
                Fs_SafeFree(msgBlock);
                iterMsgBlock = msgBlocks.erase(iterMsgBlock);
            }
        }
        g_Log->any<ComsumerTask>("comsumer end");
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
        fs::CrashHandleUtil::InitCrashHandleParams();

        g_testMsgQueue.BeforeStart();
        g_testMsgQueue.Start();

        fs::FS_ThreadPool *pool = new fs::FS_ThreadPool(0, 1);
        auto comsumerTask = fs::DelegatePlusFactory::Create(&ComsumerTask::Handler);
        pool->AddTask(comsumerTask, true);
        
        Int32 count = 0;
        std::list<fs::FS_MessageBlock *> msgBlocks;
        while(true)
        {
            Sleep(1000);
            TestMessage newMsg;
            sprintf(newMsg._buffer, "ni hao messagequeue %d", count);
            ++count;
            fs::FS_Stream stream;

            fs::FS_MessageBlock *newMsgBlock = new fs::FS_MessageBlock;
            newMsgBlock->_data = new fs::FS_Stream();
            newMsgBlock->_data->SerializeFrom(newMsg);
            msgBlocks.push_back(newMsgBlock);

            std::cout << "genorator:" << count << std::endl;
            if(msgBlocks.size() > 10)
            {
                g_testMsgQueue.PushLock();
                g_testMsgQueue.Push(msgBlocks);
                g_testMsgQueue.PushUnlock();
            }
        }
        
        getchar();
        // 主线程发送间隔一秒钟发送一个消息，消费者线程需要打印出消息
    }
};

#endif
