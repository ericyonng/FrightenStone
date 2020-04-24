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
 * @file  : FS_Timer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/06/08
 * @brief : 1. 先设置时间轮盘
 *          2. 设置超时或者cancel执行函数
 *          3. 设置超时周期
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_TimeWheel_FS_Timer_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_TimeWheel_FS_Timer_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/component/Impl/TimeWheel/TimeWheel.h"
#include "FrightenStone/common/component/Impl/TimeWheel/Comp/TimeData.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_Timer
{
    OBJ_POOL_CREATE_DEF(FS_Timer);

public:
    // 默认使用的时间轮盘对象是g_BusinessTimeWheel 若g_BusinessTimeWheel为NULL 使用的是g_TimeWheel
    FS_Timer(TimeWheel *timeWheel = g_BusinessTimeWheel);
    virtual ~FS_Timer();

public:
    // 取消
    void Cancel();
    // 设置时间轮盘
    // void SetTimeWheel(TimeWheel *timeWheel);
    // 以当前时间为基准，设置超时周期
    Int32 Schedule(Int64 milliSecPeriod);
    Int32 Schedule(const Time &startTime, Int64 milliSecPeriod);
    bool IsScheduling() const;

    // 设置超时执行函数
    template<typename ObjType>
    void SetTimeOutHandler(ObjType *objType, void (ObjType::*handler)(FS_Timer *, const Time &, const Time &));
    // timer对象 上一次timeout的时间,当前timewheel时间
    void SetTimeOutHandler(void(*handler)(FS_Timer *, const Time &, const Time &));
    // 设置cancel执行函数
    template<typename ObjType>
    void SetCancelHandler(ObjType *objType, void (ObjType::*handler)(FS_Timer *));
    void SetCancelHandler(void(*handler)(FS_Timer *));

    // 由timewheel调用超时
    void OnTimeOut(const Time &curWheelTIme);

    FS_String ToString() const;

private:
    Time _lastTimeOutTime;
    TimeWheel *_timeWheel;
    TimeData *_timeData;
    TimeSlice _periodCache;     // schedule时使用的周期缓冲
    // 超时委托 param1:返回值, param2:FS_Timer, param3:lastWheelTime, param4:curTime
    IDelegate<void, FS_Timer *, const Time &, const Time &> *_timeOutDelegate;
    // 注销委托
    IDelegate<void, FS_Timer *> *_cancelTimerDelegate;
};

FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/TimeWheel/FS_TimerImpl.h"

#endif
