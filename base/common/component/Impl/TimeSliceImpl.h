#ifdef __Base_Common_Component_Impl_TimeSlice__H__

#pragma once

#include "base/common/component/Impl/Time.h"

FS_NAMESPACE_BEGIN

inline TimeSlice::TimeSlice()
    :_slice(0)
{

}

inline TimeSlice::TimeSlice(int seconds, int milliSeconds /*= 0*/, int microSeconds /*= 0*/)
{
    _slice = seconds * Time::_microSecondPerSecond +
        milliSeconds * Time::_microSecondPerMilliSecond + microSeconds;
}

inline TimeSlice::TimeSlice(const TimeSlice &slice)
    :_slice(slice._slice)
{
}

inline TimeSlice::TimeSlice(int days, int hours, int minutes, int seconds, int milliSeconds /* = 0 */, int microSeconds /* = 0 */)
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

inline FS_String TimeSlice::ToString() const
{
    int days = GetDays();
    if(days != 0)
        return FS_String().Format("%d %02d:%02d:%02d.%06d",
                                    days, GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
    else
        return FS_String().Format("%02d:%02d:%02d.%06d",
                                    GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
}

inline TimeSlice::TimeSlice(const Int64 &slice)
:_slice(slice)
{

}

FS_NAMESPACE_END

#endif
