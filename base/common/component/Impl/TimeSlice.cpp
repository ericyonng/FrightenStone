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
 * @file  : TimeSlice.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/assist/utils/Impl/StringUtil.h"
#include <string>

FS_NAMESPACE_BEGIN

MEM_POOL_CREATE_IMPL_DEF(TimeSlice);

TimeSlice::TimeSlice(const FS_String &fmtSlice)
{

    // Ensure the slice string is time format, not datetime format.
    FS_String sliceRepr = fmtSlice;
    std::string::size_type spaceIdx = fmtSlice.GetRaw().find(' ');
    if(spaceIdx != std::string::npos)
        sliceRepr = fmtSlice.GetRaw().substr(spaceIdx + 1);

    // If the slice string is empty, set slice value to 0.
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
