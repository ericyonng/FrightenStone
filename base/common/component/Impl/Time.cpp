/**
* @file    Time.cpp
* @author  Huiya Song
* @date    2019/05/10
* @brief
*/

#include "stdafx.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/assist/utils/utils.h"
#include "base/common/component/Impl/TimeSlice.h"

FS_NAMESPACE_BEGIN

#pragma region 
const Int64 Time::_hourPerDay = 24LL;
const Int64 Time::_minutePerDay = 1440LL;              // 24*60
const Int64 Time::_secondPerDay = 86400LL;             // 24*60*60
const Int64 Time::_milliSecondPerDay = 86400000LL;     // 24*60*60*1000
const Int64 Time::_microSecondPerDay = 86400000000LL;  // 24*60*60*1000*1000

const Int64 Time::_minutePerHour = 60L;
const Int64 Time::_secondPerHour = 3600LL;
const Int64 Time::_milliSecondPerHour = 3600000LL;
const Int64 Time::_microSecondPerHour = 3600000000LL;

const Int64 Time::_secondPerMinute = 60LL;
const Int64 Time::_milliSecondPerMinute = 60000LL;
const Int64 Time::_microSecondPerMinute = 60000000LL;

const Int64 Time::_microSecondPerSecond = 1000000LL;
const Int64 Time::_millisecondPerSecond = 1000LL;

const Int64 Time::_microSecondPerMilliSecond = 1000LL;

const Int64 Time::_resolutionPerMicroSecond = 10LL;

#pragma endregion

Time::Time()
    :_rawTime(0)
    ,_gmtTimeStruct{0}
    ,_localTimeStruct{0}
{

}

Time::Time(const Time &other)
{
    _rawTime = other._rawTime;
    _gmtTimeStruct = other._gmtTimeStruct;
    _localTimeStruct = other._localTimeStruct;
}

Time::~Time()
{

}

Time Time::FromSeconds(Int64 seconds)
{
    return Time(seconds * _microSecondPerSecond);
}

Time Time::FromMilliSeconds(Int64 milliSeconds)
{
    return Time(milliSeconds * _microSecondPerMilliSecond);
}

Time Time::FromMicroSeconds(Int64 microSeconds)
{
    return Time(microSeconds);
}

Time Time::FromFmtString(const fs::FS_String &fmt)
{
    if(UNLIKELY(fmt.empty()))
        return Time(0);

    // Split date, time
    auto dateTimes = fmt.Split(' ', 1);
    if(dateTimes.size() == 1) // Only has date part or time part(try guess).
    {
        if(dateTimes[0].GetRaw().find('-') != std::string::npos) // Is date part, append default time part.
            dateTimes.push_back("0:0:0.000");
        else // Is time part, insert default date part.
            dateTimes.insert(dateTimes.begin(), "1970-1-1");
    }

    const auto &datePart = dateTimes[0];
    const auto &timePart = dateTimes[1];

    // Split year,month,day
    auto dateParts = datePart.Split('-', 2);
    if(dateParts.size() == 1) // Only has day part.
    {
        dateParts.insert(dateParts.begin(), "1");
        dateParts.insert(dateParts.begin(), "1970");
    }
    else if(dateParts.size() == 2) // Only has day and month parts.
    {
        dateParts.insert(dateParts.begin(), "1970");
    }

    // Split hour,minute,second
    auto timeParts = timePart.Split(':', 2);
    if(timeParts.size() == 1) // Only has second part.
    {
        timeParts.insert(timeParts.begin(), "0");
        timeParts.insert(timeParts.begin(), "0");
    }
    else if(timeParts.size() == 2) // Only has second and minute parts.
    {
        timeParts.insert(timeParts.begin(), "0");
    }

    // Split time, microseconds 
    auto secondParts = timeParts[2].Split('.', 1);
    if(secondParts.size() == 1) // Only has second part.
        secondParts.push_back("0");

    // Convert it
    Int32 year = fs::StringUtil::StringToInt32(dateParts[0].c_str());
    Int32 month =  fs::StringUtil::StringToInt32(dateParts[1].c_str());
    Int32 day =  fs::StringUtil::StringToInt32(dateParts[2].c_str());

    Int32 hour =  fs::StringUtil::StringToInt32(timeParts[0].c_str());
    Int32 minute =  fs::StringUtil::StringToInt32(timeParts[1].c_str());
    Int32 second =  fs::StringUtil::StringToInt32(timeParts[2].c_str());
    Int32 microSecond =  fs::StringUtil::StringToInt32(secondParts[1].c_str());

    return FromTimeMoment(year,
                         month,
                         day,
                         hour,
                         minute,
                         second,
                         static_cast<int>(microSecond / _microSecondPerMilliSecond),
                         static_cast<int>(microSecond % _microSecondPerMilliSecond));
}

Time Time::FromTimeMoment(int year, int month, int day, int hour, int minute, int second, int milliSecond, int microSecond)
{
    tm timeStruct;
    timeStruct.tm_year = year - 1900;
    timeStruct.tm_mon = month - 1;
    timeStruct.tm_mday = day;

    if(year == 1970 && month == 1 && day == 1)
    {
        int tz = fs::TimeUtil::GetTimeZone();
        int totalSeconds = hour * _secondPerHour +
            minute * _secondPerMinute + second;
        if(tz < 0 && totalSeconds < -tz)
        {
            hour = -tz / _secondPerHour;
            minute = (-tz % _secondPerHour) / _secondPerMinute;
            second = -tz % _secondPerMinute;
        }
    }

    timeStruct.tm_hour = hour;
    timeStruct.tm_min = minute;
    timeStruct.tm_sec = second;

    return FromTimeStruct(timeStruct, milliSecond, microSecond);
}

Time Time::FromTimeStruct(const tm &timeStruct, int milliSecond, int microSecond)
{
    time_t clanderTimeInSecs = ::mktime(const_cast<tm *>(&timeStruct));
    return Time(clanderTimeInSecs*_microSecondPerSecond + 
                milliSecond * _microSecondPerMilliSecond + microSecond);
}

Time &Time::operator =(const Time &other)
{
    if(this == &other)
        return *this;
    else if(*this == other)
        return *this;

    _rawTime = other._rawTime;
    memcpy(&_localTimeStruct, &other._localTimeStruct, sizeof(tm));
    memcpy(&_gmtTimeStruct, &other._gmtTimeStruct, sizeof(tm));

    return *this;
}

TimeSlice Time::GetTimeOfDay() const
{
    Int64 timeZone = TimeUtil::GetTimeZone() * _microSecondPerSecond;

    Int64 localTime = _rawTime - timeZone;
    return localTime % _microSecondPerDay;
}

TimeSlice Time::GetIntervalTo(const TimeSlice &slice) const
{
    // Get past time(local time zone).
    Int64 localTime = _rawTime - TimeUtil::GetTimeZone() * _microSecondPerSecond;
    Int64 todayElapsed = localTime % _microSecondPerDay;

    // Calculate span value.
    Int64 sliceVal = slice.GetTotalMicroSeconds() - todayElapsed;
    if(sliceVal < 0)
        sliceVal = _microSecondPerDay + sliceVal;    // 

    return TimeSlice(sliceVal);
}

TimeSlice Time::GetIntervalTo(int hour, int minute, int second, int milliSecond /*= 0*/, int microSecond /*= 0*/) const
{
    return GetIntervalTo(TimeSlice(0, hour, minute, second, milliSecond, microSecond));
}

TimeSlice Time::GetIntervalTo(const Time &from, const TimeSlice &slice)
{
    return from.GetIntervalTo(slice);
}

TimeSlice Time::GetIntervalTo(const Time &from, int hour, int minute, int second, int milliSecond /*= 0*/, int microSecond /*= 0*/)
{
    return from.GetIntervalTo(hour, minute, second, milliSecond, microSecond);
}

TimeSlice Time::operator -(const Time &time) const
{
    return TimeSlice(_rawTime - time._rawTime);
}

Time Time::operator +(const TimeSlice &slice) const
{
    return Time(_rawTime + slice.GetTotalMicroSeconds());
}

Time Time::operator -(const TimeSlice &slice) const
{
    return Time(_rawTime - slice.GetTotalMicroSeconds());
}

Time Time::AddYears(int years) const
{
    if(years == 0)
        return *this;

    tm newTimeStruct;
    GetLocalTime(newTimeStruct);

    newTimeStruct.tm_year += years;
    bool isLeap = TimeUtil::IsLeapYear(GetLocalYear());
    if(isLeap &&
       GetLocalMonth() == 2 && GetLocalDay() == 29)
    {
        if(!TimeUtil::IsLeapYear(GetLocalYear() + years))
            newTimeStruct.tm_mday -= 1;
    }

    return FromTimeStruct(newTimeStruct, GetLocalMilliSecond(), GetLocalMicroSecond());
}

Time Time::AddMonths(int months) const
{
    Time yearAddedTime = AddYears(months / 12);

    months %= 12;
    tm newTimeStruct;
    yearAddedTime.GetLocalTime(newTimeStruct);

    if(months >= 0)
    {
        int remainingMonths = 11 - newTimeStruct.tm_mon;
        if(months > remainingMonths)
        {
            newTimeStruct.tm_year += 1;
            newTimeStruct.tm_mon = months - (12 - newTimeStruct.tm_mon);
        }
        else
        {
            newTimeStruct.tm_mon += months;
        }
    }
    else
    {
        months = -months;
        int elapsedMonths = newTimeStruct.tm_mon + 1;
        if(months >= elapsedMonths)
        {
            newTimeStruct.tm_year -= 1;
            newTimeStruct.tm_mon = 12 - (months - elapsedMonths) - 1;
        }
        else
        {
            newTimeStruct.tm_mon -= months;
        }
    }

    newTimeStruct.tm_mday = std::min<Int32>(newTimeStruct.tm_mday,
                                            TimeUtil::GetMonthMaxDays(yearAddedTime.GetLocalYear(), newTimeStruct.tm_mon + 1));

    return FromTimeStruct(newTimeStruct, GetLocalMilliSecond(), GetLocalMicroSecond());
}

Time Time::AddDays(int days) const
{
    return *this + TimeSlice(days * _microSecondPerDay);
}

Time Time::AddHours(int hours) const
{
    return *this + TimeSlice(hours * _microSecondPerHour);
}

Time Time::AddMinutes(int minutes) const
{
    return *this + TimeSlice(minutes * _microSecondPerMinute);
}

Time Time::AddSeconds(int seconds) const
{
    return *this + TimeSlice(seconds * _microSecondPerSecond);
}

Time Time::AddMilliSeconds(int milliSeconds) const
{
    return *this + TimeSlice(milliSeconds * _microSecondPerMilliSecond);
}

Time Time::AddMicroSeconds(int microSeconds) const
{
    return *this + TimeSlice(static_cast<Int64>(microSeconds));
}

Time Time::GetZeroTime() const
{
    // 转换成本地时间
    Int64 zoneTime = static_cast<Int64>(TimeUtil::GetTimeZone()*Time::_microSecondPerSecond);
    Int64 localTime = _rawTime - zoneTime;
    Int64 localZeroTime = localTime / _microSecondPerDay * _microSecondPerDay;
    localZeroTime += zoneTime;
    return Time(localZeroTime);
}

FS_String Time::Format(const Byte8 *outFmt) const
{
    BUFFER32 buf = {0};
    if(outFmt)
        strftime(buf, sizeof(buf), outFmt, &_localTimeStruct);
    else
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &_localTimeStruct);

    return buf;
}

FS_String Time::FormatAsGmt(const char *outFmt) const
{
    BUFFER32 buf = {0};
    if(outFmt)
        strftime(buf, sizeof(buf), outFmt, &_gmtTimeStruct);
    else
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &_gmtTimeStruct);

    return buf;
}

FS_String Time::ToString() const
{
    FS_String repr;
    auto localTime = _rawTime - static_cast<Int64>(TimeUtil::GetTimeZone()*Time::_microSecondPerSecond);
    return repr.Format("%s.%06d", Format().c_str(), localTime%_microSecondPerSecond);
}

Time::Time(Int64 microSecTimestamp)
    :_rawTime(microSecTimestamp)
{
    _UpdateTimeStructs();
}

Time::Time(const std::chrono::system_clock::time_point &now)
{
    _rawTime = now.time_since_epoch().count() / _resolutionPerMicroSecond;    // 
    _UpdateTimeStructs();
}

void Time::_UpdateTimeStructs()
{
    time_t calendarTime = static_cast<time_t>(_rawTime / _microSecondPerSecond);
#ifdef _WIN32
    ::localtime_s(&_localTimeStruct, &calendarTime);
    ::gmtime_s(&_gmtTimeStruct, &calendarTime);
#else
    ::localtime_r(&calendarTime, &_localTimeStruct);
    ::gmtime_r(&calendarTime, &_gmtTimeStruct);
#endif
}

FS_NAMESPACE_END
