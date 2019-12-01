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
 * @file  : TestTimeWheel.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/06/08
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestTimeWheel_H__
#define __Test_TestTimeWheel_H__
#pragma once

class TestWheel1
{
public:
    void TimeOut(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curTime)
    {
        std::cout << "TestWheel1 curTime:" << curTime.ToString() << std::endl;
        std::cout << "TestWheel1 timer:" << timer->ToString() << std::endl;

        if(lastWheelTime.GetLocalMinute() != curTime.GetLocalMinute())
        {
            std::cout << "cross minitue" << std::endl;
            timer->Cancel();
        }
    }
};

static void TimeOut(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curTime)
{
    std::cout << "static test TimeOut :" << std::endl;

    std::cout << "static test TimeOut : curTime:" << curTime.ToString() << std::endl;
    std::cout << "static test TimeOut : timer:" << timer->ToString() << std::endl;
    timer->Cancel();
}

static void TimeOut4(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curTime)
{
    std::cout << "static test TimeOut4:" << std::endl;

    std::cout << "static test TimeOut4 : curTime:" << curTime.ToString() << std::endl;
    std::cout << "static test TimeOut4 : timer:" << timer->ToString() << std::endl;
}

static void TimeOut2(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curTime)
{
    static fs::FS_Timer *timer4 = new fs::FS_Timer;
    if(!timer4->IsScheduling())
    {
        timer4->SetTimeOutHandler(&TimeOut4);
        timer4->Schedule(2000);
    }

    std::cout << "static test TimeOut2:" << std::endl;

    std::cout << "static test TimeOut2 : curTime:" << curTime.ToString() << std::endl;
    std::cout << "static test TimeOut2 : timer:" << timer->ToString() << std::endl;
    static Int32 cnt = 5;
    if(--cnt <= 0)
        timer->Cancel();
}


static void Cancel(fs::FS_Timer *timer)
{
    std::cout << "cancel a timer" << std::endl;
}

class TestTimeWheel
{
public:
    static void Run(const fs::FS_ThreadPool *pool)
    {
        g_Log->InitModule("TestTimeWheel");
        g_MemleakMonitor->Start();

        // 设置时间轮盘参数
//         fs::TimeSlice resolution(0, 100);
//         fs::TimeWheel timeWheel(resolution);
        fs::FS_Timer timer;
        fs::FS_Timer timer2;
        fs::FS_Timer timer3;


        // 设置超时执行函数
        TestWheel1 test1;
        timer.SetTimeOutHandler(&test1, &TestWheel1::TimeOut);
        // timer.Schedule(fs::Time::FromFmtString(fs::FS_String("2019-06-09 16:02:59.000000")), 4000);
        timer.Schedule(4000);

        timer2.SetTimeOutHandler(&TimeOut);
        timer2.Schedule(2000);

        timer3.SetTimeOutHandler(&TimeOut2);
        timer3.Schedule(3000);

        fs::TimeSlice waitMilliSec;
        fs::g_TimeWheel.GetModifiedResolution(waitMilliSec);
        while(true)
        {
            fs::SystemUtil::Sleep(static_cast<DWORD>(waitMilliSec.GetTotalMilliSeconds()));

            // 转动时间轮盘
            fs::g_TimeWheel.RotateWheel();

            // 修正下一帧时间
            fs::g_TimeWheel.GetModifiedResolution(waitMilliSec);
            if(!pool->IsPoolWorking())
                break;
        }
    }
};

#endif
