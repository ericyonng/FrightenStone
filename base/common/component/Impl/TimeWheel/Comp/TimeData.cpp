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
 * @file  : TimeWheelData.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/6
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/component/Impl/TimeWheel/TimeWheel.h"
#include "base/common/component/Impl/TimeWheel/Comp/TimeData.h"
#include "base/common/component/Impl/TimeWheel/FS_Timer.h"

FS_NAMESPACE_BEGIN

bool TimeDataLess::operator ()(const TimeData *l, const TimeData *r) const
{
    if(l == r)
        return l < r;

    if(l->_expiredTime == r->_expiredTime)
        return l->_timeWheelUniqueId < r->_timeWheelUniqueId;

    return l->_expiredTime < r->_expiredTime;
}

TimeData::TimeData(FS_Timer *timer)
    :_isCancel(false)
    ,_isRotatingWheel(false)
    ,_timeWheelUniqueId(0)
    ,_timer(timer)
{

}

TimeData::~TimeData()
{
}

FS_String TimeData::ToString() const
{
    FS_String info;
    info.Format("timeData: _expiredTime:%lld|_isCancel:%d|_timeWheelUniqueId:%lld|"
                "_period:%lld"
                , _expiredTime.GetMicroTimestamp(), _isCancel, _timeWheelUniqueId
                , _period.GetTotalMicroSeconds());

    return info;
}

FS_NAMESPACE_END
