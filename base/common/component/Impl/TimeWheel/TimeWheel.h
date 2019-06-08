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
 * @file  : TimeWeel.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/6
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_TimeWheel_TimeWheel_H__
#define __Base_Common_Component_Impl_TimeWheel_TimeWheel_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/component/Impl/TimeWheel/Comp/TimeData.h"
#include <set>

FS_NAMESPACE_BEGIN

class BASE_EXPORT TimeWheel
{
public:
    explicit TimeWheel(const TimeSlice &resolutionSlice);
    virtual ~TimeWheel();

    // 转动时间轮盘
    void RotateWheel();

    // 注册超时数据
    Int32 Register(TimeData *timeData);
    // 取消注册
    void UnRegister(TimeData *timeData);
    // 当前时间轮盘精度 @return:返回微妙
    // Int64 GetTimeWheelResolution() const;

private:
    // 产生timewheel生命周期中唯一id
    Int64 _NewIncreasId();

private:
    const TimeSlice _resolutionSlice;       // 每一轮时间切片长度
    Time _curTime;                  // 当前时间戳
    Time _lastTime;                 // 最后一次时间
    std::set<TimeData *, TimeDataLess> _timeDatas;

    Int64 _increaseId;              // 递增id
};

FS_NAMESPACE_END


#include "base/common/component/Impl/TimeWheel/TimeWheelImpl.h"

#endif
