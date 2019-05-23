#ifndef __Base_Common_Component_Impl_Time__H__
#define __Base_Common_Component_Impl_Time__H__
/**
* @file    Time.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/03/01
* @brief
*/

#pragma once

#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/assist/utils/Impl/TimeUtil.h"
#include <chrono>

FS_NAMESPACE_BEGIN

class TimeSlice;

class BASE_EXPORT Time 
{
    #pragma region 时间单位互换
public:
    // 天
    static const Int64 _hourPerDay;
    static const Int64 _minutePerDay;
    static const Int64 _secondPerDay;
    static const Int64 _milliSecondPerDay;
    static const Int64 _microSecondPerDay;

    // 时
    static const Int64 _minutePerHour;
    static const Int64 _secondPerHour;
    static const Int64 _milliSecondPerHour;
    static const Int64 _microSecondPerHour;

    // 分
    static const Int64 _secondPerMinute;
    static const Int64 _milliSecondPerMinute;
    static const Int64 _microSecondPerMinute;

    // ->秒
    static const Int64 _microSecondPerSecond;
    static const Int64 _millisecondPerSecond;

    // ->毫秒
    static const Int64 _microSecondPerMilliSecond;

    // 系统时钟最小分辨率
    static const Int64 _resolutionPerMicroSecond;    // （系统时间单位精确到微妙的十分之一，不是真正意义上的纳秒）
    #pragma endregion 
 
public:
    Time();
    Time(const Time &other);
    virtual ~Time();

    #pragma region 获取时间
    static Time Now();
    static Int64 NowTimestamp();            // 默认秒
    static Int64 NowMilliTimestamp();       // 毫秒
    static Int64 NowMicroTimestamp();       // 微秒
    
    static Time FromSeconds(Int64 seconds);
    static Time FromMilliSeconds(Int64 milliSeconds);
    static Time FromMicroSeconds(Int64 microSeconds);
    static Time FromFmtString(const fs::FS_String &fmt);    // fmt格式：1970-07-01 12:12:12.000000055 注:.后面是微妙
    static Time FromTimeMoment(int year, int month, int day, int hour, int minute, int second, int milliSecond = 0, int microSecond = 0);
    static Time FromTimeStruct(const tm &timeStruct, int milliSecond = 0, int microSecond = 0);
    #pragma endregion

    #pragma region 获取时间戳
    Int64 GetSecTimestamp() const;
    Int64 GetMilliTimestamp() const;
    Int64 GetMicroTimestamp() const;
    #pragma endregion

    #pragma region 时间操作
    const Time &FlushTime();

    bool operator ==(const Time &time) const;
    bool operator !=(const Time &time) const;
    bool operator <(const Time &time) const;
    bool operator >(const Time &time) const;
    bool operator <=(const Time &time) const;
    bool operator >=(const Time &time) const;

    Time &operator =(const Time &other);
    #pragma endregion

    #pragma region 时间片段
    /**
     * Get current time of day.
     * @return TimeSlice - the current time of day.
     */
    TimeSlice GetTimeOfDay() const;
    /**
     * Get remaining seconds to nearest day special monent.
     * @param[in] slice        - slice value.
     * @param[in] hour        - hour.
     * @param[in] minute      - minute.
     * @param[in] second      - second.
     * @param[in] milliSecond - milli-second.
     * @param[in] microSecond - micro-second.
     * @param[in] from        - from time.
     * @return TimeSlice - timeslice value.
     */
    TimeSlice GetIntervalTo(const TimeSlice &slice) const;    // slice若小于当前时刻则计算下一天slice时刻距离当前的时间间隔
    TimeSlice GetIntervalTo(int hour, int minute, int second, int milliSecond = 0, int microSecond = 0) const;
    static TimeSlice GetIntervalTo(const Time &from, const TimeSlice &slice);
    static TimeSlice GetIntervalTo(const Time &from, int hour, int minute, int second, int milliSecond = 0, int microSecond = 0);

    /**
     * Time slice operations.
     */
    TimeSlice operator -(const Time &time) const;

    Time operator +(const TimeSlice &slice) const;
    Time operator -(const TimeSlice &span) const;
    #pragma endregion

    #pragma region 本地时间
    /**
     * Add specified time parts values.
     * Notes: These operations are thread-safe, all add parts added to new LLBC_Time object.
     * @param[in] <time parts> - the all time parts(year, month, day, ...).
     * @return LLBC_Time - the new time object.
     */
    Time AddYears(int years) const;
    Time AddMonths(int months) const;
    Time AddDays(int days) const;
    Time AddHours(int hours) const;
    Time AddMinutes(int minutes) const;
    Time AddSeconds(int seconds) const;
    Time AddMilliSeconds(int milliSeconds) const;
    Time AddMicroSeconds(int microSeconds) const;
    
    /**
     * Get GMT time struct.
     * @param[out] timeStruct - time struct object reference.
     * @return const tm & - time struct object.
     */
    const tm &GetGmtTime() const;
    void GetGmtTime(tm &timeStruct) const;

    /**
     * Get local time struct.
     * @param[out] timeStruct - time struct reference.
     * @return const tm & - time struct object.
     */
    const tm &GetLocalTime() const;
    void GetLocalTime(tm &timeStruct) const;

    Int32 GetLocalYear() const; // since 1900
    Int32 GetLocalMonth() const;    // start by 1
    Int32 GetLocalDay() const;
    Int32 GetLocalDayOfWeek() const;    // start by 0
    Int32 GetLocalDayOfYear() const;    // start by 1
    Int32 GetLocalHour() const;
    Int32 GetLocalMinute() const;
    Int32 GetLocalSecond() const;
    Int32 GetLocalMilliSecond() const;
    Int32 GetLocalMicroSecond() const;

    Time GetZeroTime() const;

    FS_String Format(const Byte8 *outFmt = NULL) const;
    static FS_String Format(time_t timestamp, const Byte8 *outFmt);

    FS_String FormatAsGmt(const char *outFmt = NULL) const;
    static FS_String FormatAsGmt(time_t timestamp, const char *outFmt);
    
    /**
     * Get the time object string representation.
     * @return FS_String - the object string representation.
     */
    FS_String ToString() const;
    #pragma endregion

    #pragma region 内部方法
private:
    explicit Time(Int64 microSecTimestamp);
    explicit Time(const std::chrono::system_clock::time_point &now);
    void _UpdateTimeStructs();
    // 
    #pragma endregion

private:
    Int64 _rawTime;    // microsecond (微妙级时间戳)
    tm _gmtTimeStruct;  // 格林尼治时间
    tm _localTimeStruct;    // 本地时间
};

#pragma region Inline
inline Time Time::Now()
{
    return Time(std::chrono::system_clock::now());
}

inline Int64 Time::NowTimestamp()
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

inline Int64 Time::NowMilliTimestamp()
{
    const auto &nowTime = std::chrono::system_clock().now();
    return (nowTime.time_since_epoch().count() / _resolutionPerMicroSecond) / _microSecondPerMilliSecond;
}

inline Int64 Time::NowMicroTimestamp()
{
    const auto &nowTime = std::chrono::system_clock().now();
    return (nowTime.time_since_epoch().count() / _resolutionPerMicroSecond);
}

inline Int64 Time::GetSecTimestamp() const
{
    return _rawTime / _microSecondPerSecond;
}

inline Int64 Time::GetMilliTimestamp() const
{
    return _rawTime / _microSecondPerMilliSecond;
}

inline Int64 Time::GetMicroTimestamp() const
{
    return _rawTime;
}

inline const Time &Time::FlushTime()
{
    _rawTime = std::chrono::system_clock::now().time_since_epoch().count() / _resolutionPerMicroSecond;
    _UpdateTimeStructs();
    return *this;
}

inline bool Time::operator ==(const Time &time) const
{
    return _rawTime == time._rawTime;
}

inline bool Time::operator !=(const Time &time) const
{
    return _rawTime != time._rawTime;
}

inline bool Time::operator <(const Time &time) const
{
    return _rawTime < time._rawTime;
}

inline bool Time::operator >(const Time &time) const
{
    return _rawTime > time._rawTime;
}

inline bool Time::operator >=(const Time &time) const
{
    return _rawTime >= time._rawTime;
}

inline bool Time::operator <=(const Time &time) const
{
    return _rawTime <= time._rawTime;
}

inline const tm &Time::GetGmtTime() const
{
    return _gmtTimeStruct;
}

inline void Time::GetGmtTime(tm &timeStruct) const
{
     memcpy(&timeStruct, &_gmtTimeStruct, sizeof(tm));
}

inline const tm &Time::GetLocalTime() const
{
    return _localTimeStruct;
}

inline void Time::GetLocalTime(tm &timeStruct) const
{
    memcpy(&timeStruct, &_localTimeStruct, sizeof(tm));
}

inline Int32 Time::GetLocalYear() const
{
    return _localTimeStruct.tm_year + 1900;     // since 1900
}

inline Int32 Time::GetLocalMonth() const
{
    return _localTimeStruct.tm_mon + 1;     // start by 1
}

inline Int32 Time::GetLocalDay() const
{
    return _localTimeStruct.tm_mday;
}

inline Int32 Time::GetLocalDayOfWeek() const
{
    return _localTimeStruct.tm_wday;        // start by 0
}

inline Int32 Time::GetLocalDayOfYear() const
{
    return _localTimeStruct.tm_yday + 1;    // start by 1
}

inline Int32 Time::GetLocalHour() const
{
    return _localTimeStruct.tm_hour;
}

inline Int32 Time::GetLocalMinute() const
{
    return _localTimeStruct.tm_min;
}

inline Int32 Time::GetLocalSecond() const
{
    return _localTimeStruct.tm_sec;
}

inline Int32 Time::GetLocalMilliSecond() const
{
    // 转化成本地时间
    auto localTime = _rawTime - static_cast<Int64>(TimeUtil::GetTimeZone()*Time::_microSecondPerSecond);
    return static_cast<Int32>(localTime%Time::_microSecondPerSecond / Time::_microSecondPerMilliSecond);
}

inline Int32 Time::GetLocalMicroSecond() const
{
    auto microsecond = _rawTime - TimeUtil::GetTimeZone()*Time::_microSecondPerSecond;
    return static_cast<Int32>(microsecond % Time::_microSecondPerSecond);
}

inline FS_String Time::Format(time_t timestamp, const Byte8 *outFmt)
{
    return FromSeconds(timestamp).Format(outFmt);
}

inline FS_String Time::FormatAsGmt(time_t timestamp, const char *outFmt)
{
    return FromSeconds(timestamp).FormatAsGmt(outFmt);
}
#pragma endregion

FS_NAMESPACE_END

#endif
