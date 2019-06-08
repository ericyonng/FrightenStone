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
    void TimeOut(fs::FS_Timer *&timer, const fs::Time &lastWheelTime, const fs::Time &curTime)
    {
        std::cout << "test wheel1 :" << "lastWheelTime:" << lastWheelTime.GetMicroTimestamp()<<" fmt:"<<lastWheelTime.ToString().c_str() << std::endl;
        std::cout << "test wheel1 : curTime:" << curTime.GetMicroTimestamp() << " fmt: " << curTime.ToString().c_str() << std::endl;

        if(lastWheelTime.GetLocalMinute() != curTime.GetLocalMinute())
        {
            std::cout << "cross minitue" << std::endl;
        }
    }
};

class TestTimeWheel
{
public:
    static void Run()
    {
        // 设置时间轮盘参数
        fs::TimeSlice resolution(0, 100);
        fs::TimeWheel timeWheel(resolution);
        fs::FS_Timer timer;

        // 设置时间轮盘
        timer.SetTimeWheel(&timeWheel);

        // 设置超时执行函数
        TestWheel1 test1;
        timer.SetTimeOutHandler(&test1, &TestWheel1::TimeOut);
        timer.Schedule(1000);

        while(true)
        {
            Sleep(static_cast<DWORD>(resolution.GetTotalMilliSeconds()));

            // 转动时间轮盘
            timeWheel.RotateWheel();
        }
    }
};

#endif
