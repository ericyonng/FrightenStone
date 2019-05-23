#include "stdafx.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/assist/utils/Impl/StringUtil.h"
#include <string>

FS_NAMESPACE_BEGIN

TimeSlice::TimeSlice(const FS_String &fmtSlice)
{

    // Ensure the slice string is time format, not datetime format.
    FS_String sliceRepr = fmtSlice;
    std::string::size_type spaceIdx = fmtSlice.GetRaw().find(' ');
    if(spaceIdx != std::string::npos)
        sliceRepr = fmtSlice.GetRaw().substr(spaceIdx + 1);

    // If the slice string is empty, set span value to 0.
    if(sliceRepr.empty())
    {
        _slice = 0;
        return;
    }

    // Split by ':', fetch hour,minute,second, microsecond parts.
    auto sliceParts = fmtSlice.Split(':');
    if(sliceParts.size() == 1) // Only has second part.
    {
        sliceParts.insert(sliceParts.begin(), "0");
        sliceParts.insert(sliceParts.begin(), "0");
    }
    else if(sliceParts.size() == 2) // Only has second and minute parts.
    {
        sliceParts.insert(sliceParts.begin(), "0");
    }

    _slice = StringUtil::StringToInt32(sliceParts[0].c_str()) * Time::_microSecondPerHour +
        StringUtil::StringToInt32(sliceParts[1].c_str()) * Time::_microSecondPerMinute;

    auto secParts = sliceParts[2].Split('.', 1);
    _slice += StringUtil::StringToInt32(secParts[0].c_str()) * Time::_microSecondPerSecond;
    if(secParts.size() == 2)
        _slice += StringUtil::StringToInt32(secParts[1].c_str());
}
FS_NAMESPACE_END
