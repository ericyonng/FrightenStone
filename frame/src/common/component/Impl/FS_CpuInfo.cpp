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
 * @file  : FS_CpuInfo.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
/**
* @file FS_CpuInfo.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/20
* @brief
*/

#include "stdafx.h"
#include "FrightenStone/common/component/Impl/FS_CpuInfo.h"

FS_NAMESPACE_BEGIN

FS_CpuInfo::FS_CpuInfo()
    :_isInit(false)
#ifdef _WIN32
    ,_preidleTime{0}
    ,_preKernalTime{0}
    , _preUserTime{0}
#endif
{

}

bool FS_CpuInfo::Initialize()
{
    if(UNLIKELY(_isInit))
        return true;

#ifdef _WIN32
    if(UNLIKELY(!GetSystemTimes(&_preidleTime, &_preKernalTime, &_preUserTime)))
        return false;
#endif

    _isInit = true;
    return true;
}

Double FS_CpuInfo::GetUsage()
{
#ifdef _WIN32
    if(!_isInit)
        return -1;

    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if(!GetSystemTimes(&idleTime, &kernelTime, &userTime))
        return -1;

    Int64 diffIdleTime = _CompareFileTime(_preidleTime, idleTime);          // free time
    Int64 diffKernelTime = _CompareFileTime(_preKernalTime, kernelTime);    // kernal time
    Int64 diffUserTime = _CompareFileTime(_preUserTime, userTime);          // user time

    if(diffKernelTime + diffUserTime == 0)
        return -1;

    // cpu usage =（total-idle）/total
    double usage = static_cast<Double>(diffKernelTime + diffUserTime - diffIdleTime) / (diffKernelTime + diffUserTime);

    _preidleTime = idleTime;
    _preKernalTime = kernelTime;
    _preUserTime = userTime;
    return usage;
#else
    // TODO
    return -1;
#endif
}

Int32 FS_CpuInfo::GetCpuCoreCnt()
{
    Int32 count = 1; // at least one
#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    count = si.dwNumberOfProcessors;
#else
    count = sysconf(_SC_NPROCESSORS_CONF);
#endif  
    return count;
}

#ifdef _WIN32
Int64 FS_CpuInfo::_CompareFileTime(FILETIME time1, FILETIME time2)
{
    Int64 a = (static_cast<Int64>(time1.dwHighDateTime) << 32) | static_cast<Int64>(time1.dwLowDateTime);
    Int64 b = (static_cast<Int64>(time2.dwHighDateTime) << 32) | static_cast<Int64>(time2.dwLowDateTime);

    return abs(b - a);
}
#endif

FS_NAMESPACE_END
