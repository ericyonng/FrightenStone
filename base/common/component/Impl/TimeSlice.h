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
 * @file  : TimeSlice.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_TimeSlice_H__
#define __Base_Common_Component_Impl_TimeSlice_H__

#pragma once

#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

class Time;
class FS_String;

class BASE_EXPORT TimeSlice
{
    OBJ_POOL_CREATE_DEF(TimeSlice);
public:
    /**

     */
    TimeSlice();

    /**
     * Construct by slice, in seconds.
     * @param[in] seconds      - the slice seconds part.
     * @param[in] milliSeconds - the slice milli-seconds part.
     * @param[in] microSeconds - the slice micro-seconds part.
     */
    explicit TimeSlice(int seconds, Int64 milliSeconds = 0, Int64 microSeconds = 0);

    /**
     * Construct by slice string representation(fmt: 00:00:00.xxxxxx).
     * @param[in] slice - the slice value string representation, fmt: 00:00:00.xxxxx, the micro-seconds is optional.
     */
    TimeSlice(const FS_String &fmtSlice);

    /**
     * Copy constructor.
     */
    TimeSlice(const TimeSlice &slice);

    /**
     * Time slice parts constructor.
     * @param[in] days         - the days part.
     * @param[in] hours        - the hours part.
     * @param[in] minutes      - the minutes part.
     * @param[in] seconds      - the seconds part.
     * @param[in] milliSeconds - the milli-seconds part, default is 0.
     * @param[in] microSeconds - the micro-seconds part, default is 0.
     */
    TimeSlice(int days, int hours, int minutes, Int64 seconds, Int64 milliSeconds = 0, Int64 microSeconds = 0);

    /**
     * Destructor.
     */
    ~TimeSlice();

    /**
     * Get days/hours/minutes/seconds/milli-seconds/micro-seconds.
     * @return int - the time slice parts value. slice 的一部分
     */
    int GetDays() const;
    int GetHours() const;
    int GetMinutes() const;
    int GetSeconds() const;
    int GetMilliSeconds() const;
    int GetMicroSeconds() const;

    int GetTotalDays() const;
    int GetTotalHours() const;
    int GetTotalMinutes() const;
    int GetTotalSeconds() const;
    Int64 GetTotalMilliSeconds() const;
    const Int64 &GetTotalMicroSeconds() const;

    TimeSlice operator +(const TimeSlice &slice) const;
    TimeSlice operator -(const TimeSlice &slice) const;

    TimeSlice &operator +=(const TimeSlice &slice);
    TimeSlice &operator -=(const TimeSlice &slice);

    bool operator ==(const TimeSlice &slice) const;
    bool operator !=(const TimeSlice &slice) const;
    bool operator <(const TimeSlice &slice)const;
    bool operator >(const TimeSlice &slice) const;
    bool operator <=(const TimeSlice &slice) const;
    bool operator >=(const TimeSlice &slice) const;

    TimeSlice &operator =(const TimeSlice &slice);
    TimeSlice &operator =(Int64 microSecSlice);

    FS_String ToString() const;
private:
    friend class Time;
    TimeSlice(const Int64 &slice);

private:
    Int64 _slice;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/TimeSliceImpl.h"

#endif
