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
 * @file  : TimeWeelImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/6
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_TimeWheel_TimeWheel_H__
#pragma once

FS_NAMESPACE_BEGIN

inline void TimeWheel::UnRegister(TimeData *timeData)
{
    if(UNLIKELY(timeData->_isRotatingWheel||_IsRotating()))
    {
        _NewAsynTimeData(AsynOpType::Op_UnRegister, timeData);
        return;
    }

    if(LIKELY(timeData->_timeWheelUniqueId))
        _timeDatas.erase(timeData);
}

inline const TimeSlice &TimeWheel::GetTimeWheelResolution() const
{
    return _resolutionSlice;
}

// 下一帧轮盘扫描时间修正值
inline void TimeWheel::GetModifiedResolution(TimeSlice &modifiedResolution) const
{
    const TimeData *firstTimeData = NULL;
    if(!_timeDatas.empty())
        firstTimeData = *_timeDatas.begin();

    // 若轮盘容器中有待过期的定时数据则使用定时数据作为修正依据，其他情况使用_curTime作为修正依据
    const auto &nowTimeToModify = firstTimeData ? (firstTimeData->_expiredTime - firstTimeData->_period) : _curTime;
    const auto &modifyResolution = fs::Time::Now() - nowTimeToModify;
    
    const auto &leftWaitTime = _resolutionSlice - modifyResolution;
    static const TimeSlice zeroSlice;

    modifiedResolution = leftWaitTime <= zeroSlice ? zeroSlice : leftWaitTime;
}

inline const Time &TimeWheel::GetCurTime() const
{
    return _curTime;
}

inline Int64 TimeWheel::_NewIncreasId()
{
    return ++_increaseId;
}

inline bool TimeWheel::_IsRotating() const
{
    return _rotating > 0;
}

inline void TimeWheel::_BeforeRotateWheel()
{
    ++_rotating;
}

inline AsynTimeData *TimeWheel::_NewAsynTimeData(Int32 opType, TimeData *timeData)
{
    auto newData = new AsynTimeData;
    newData->_opType = opType;
    newData->_timeData = timeData;
    _asynData.push_back(newData);
    return newData;
}

FS_NAMESPACE_END

#endif // __Base_Common_Component_Impl_TimeWheel_TimeWheel_H__

