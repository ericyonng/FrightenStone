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
 * @file  : TimeSliceImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Component_Impl_TimeSlice_H__

#pragma once

#include "FrightenStone/common/component/Impl/Time.h"

FS_NAMESPACE_BEGIN

inline TimeSlice::TimeSlice()
    :_slice(0)
{

}

inline TimeSlice::TimeSlice(int seconds, Int64 milliSeconds /*= 0*/, Int64 microSeconds /*= 0*/)
{
    _slice = seconds * Time::_microSecondPerSecond +
        milliSeconds * Time::_microSecondPerMilliSecond + microSeconds;
}

inline TimeSlice::TimeSlice(const TimeSlice &slice)
    :_slice(slice._slice)
{
}

inline TimeSlice::TimeSlice(int days, int hours, int minutes, Int64 seconds, Int64 milliSeconds /* = 0 */, Int64 microSeconds /* = 0 */)
{
    _slice = static_cast<Int64>((((((days * 24) + hours) * 60) + minutes) * 60) + seconds) *
        Time::_microSecondPerSecond + milliSeconds * Time::_microSecondPerMilliSecond + microSeconds;
}

inline TimeSlice::~TimeSlice()
{
}

inline int TimeSlice::GetDays() const
{
    return static_cast<int>(_slice / Time::_microSecondPerDay);
}

inline int TimeSlice::GetHours() const
{
    return static_cast<int>((_slice % Time::_microSecondPerDay) /
                            Time::_microSecondPerHour);
}

inline int TimeSlice::GetMinutes() const
{
    return static_cast<int>((_slice % Time::_microSecondPerHour) /
                            Time::_microSecondPerMinute);
}

inline int TimeSlice::GetSeconds() const
{
    return static_cast<int>((_slice % Time::_microSecondPerMinute) /
                            Time::_microSecondPerSecond);
}

inline int TimeSlice::GetMilliSeconds() const
{
    return static_cast<int>((_slice % Time::_microSecondPerSecond) /
                            Time::_microSecondPerMilliSecond);
}

inline int TimeSlice::GetMicroSeconds() const
{
    return static_cast<int>(_slice % Time::_microSecondPerMilliSecond);
}

inline int TimeSlice::GetTotalDays() const
{
    return static_cast<int>(_slice / Time::_microSecondPerDay);
}

inline int TimeSlice::GetTotalHours() const
{
    return static_cast<int>(_slice / Time::_microSecondPerHour);
}

inline int TimeSlice::GetTotalMinutes() const
{
    return static_cast<int>(_slice / Time::_microSecondPerMinute);
}

inline int TimeSlice::GetTotalSeconds() const
{
    return static_cast<int>(_slice / Time::_microSecondPerSecond);
}

inline Int64 TimeSlice::GetTotalMilliSeconds() const
{
    return static_cast<Int64>(_slice / Time::_microSecondPerMilliSecond);
}

inline const Int64 &TimeSlice::GetTotalMicroSeconds() const
{
    return _slice;
}

inline TimeSlice TimeSlice::operator +(const TimeSlice &slice) const
{
    return TimeSlice(_slice + slice._slice);
}

inline TimeSlice TimeSlice::operator -(const TimeSlice &slice) const
{
    return TimeSlice(_slice - slice._slice);
}

inline TimeSlice &TimeSlice::operator +=(const TimeSlice &slice)
{
    _slice += slice._slice;
    return *this;
}

inline TimeSlice &TimeSlice::operator -=(const TimeSlice &slice)
{
    _slice -= slice._slice;
    return *this;
}

inline bool TimeSlice::operator ==(const TimeSlice &slice) const
{
    return _slice == slice._slice;
}

inline bool TimeSlice::operator !=(const TimeSlice &slice) const
{
    return !(*this == slice);
}

inline bool TimeSlice::operator <(const TimeSlice &slice) const
{
    return _slice < slice._slice;
}

inline bool TimeSlice::operator >(const TimeSlice &slice) const
{
    return _slice > slice._slice;
}

inline bool TimeSlice::operator <=(const TimeSlice &slice) const
{
    return _slice <= slice._slice;
}

inline bool TimeSlice::operator >=(const TimeSlice &slice) const
{
    return _slice >= slice._slice;
}

inline TimeSlice &TimeSlice::operator =(const TimeSlice &slice)
{
    _slice = slice._slice;
    return *this;
}

inline TimeSlice &TimeSlice::operator =(Int64 microSecSlice)
{
    _slice = microSecSlice;
    return *this;
}

inline FS_String TimeSlice::ToString() const
{
    int days = GetDays();
    if(days != 0)
        return FS_String().AppendFormat("%d %02d:%02d:%02d.%06d",
                                    days, GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
    else
        return FS_String().AppendFormat("%02d:%02d:%02d.%06d",
                                    GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
}

inline TimeSlice::TimeSlice(const Int64 &slice)
:_slice(slice)
{

}

FS_NAMESPACE_END

#endif
