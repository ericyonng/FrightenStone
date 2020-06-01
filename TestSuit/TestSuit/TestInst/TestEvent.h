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
 * @file  : TestEvent.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestEvent_H__
#define __Test_TestEvent_H__
#pragma once

#include "stdafx.h"

class EventTestTask
{
public:
    EventTestTask() {}
    ~EventTestTask() {}

    void TestEvent(fs::FS_Event *ev)
    {
        std::cout << "TestEvent 1" << std::endl;
        std::cout << ev->GetParam("token").AsStr() << std::endl;
    }
};

void TestEventTask2(fs::FS_Event *ev)
{
    std::cout << "TestEventTask2" << std::endl;
    std::cout << ev->GetParam("token").AsStr() << std::endl;
}

class TestEvent
{
public:
    static void Run()
    {
        EventTestTask task1;
        auto stub1 = g_FSEventMgr->AddListener(1, fs::DelegatePlusFactory::Create(&task1, &EventTestTask::TestEvent));
        auto stub2 = g_FSEventMgr->AddListener(1, fs::DelegatePlusFactory::Create(&TestEventTask2));

        fs::FS_Event *ev = new fs::FS_Event(1);
        ev->SetParam("token", "hello world");
        g_FSEventMgr->FireEvent(ev);

        g_FSEventMgr->RemoveListenerX(stub1);
        g_FSEventMgr->RemoveListenerX(stub2);
    }
};

#endif
