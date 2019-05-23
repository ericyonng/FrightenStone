/**
* @file CpuUtil.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/20
* @brief
*/

#include "stdafx.h"
#include "base/common/assist/utils/Impl/FS_CpuUtil.h"

FS_NAMESPACE_BEGIN

FILETIME FS_CpuUtil::_preidleTime = FILETIME{0};
FILETIME FS_CpuUtil::_preKernalTime = FILETIME{0};
FILETIME FS_CpuUtil::_preUserTime = FILETIME{0};
bool FS_CpuUtil::_isInit = false;

bool FS_CpuUtil::Initialize()
{
    if(UNLIKELY(_isInit))
        return true;

    if(UNLIKELY(!GetSystemTimes(&_preidleTime, &_preKernalTime, &_preUserTime)))
        return false;

    _isInit = true;
    return true;
}

Double FS_CpuUtil::GetUsage()
{
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

    // cpu usage =£¨total-idle£©/total
    double usage = static_cast<Double>(diffKernelTime + diffUserTime - diffIdleTime) / (diffKernelTime + diffUserTime);

    _preidleTime = idleTime;
    _preKernalTime = kernelTime;
    _preUserTime = userTime;
    return usage;
}

Int32 FS_CpuUtil::GetCpuCoreCnt()
{
    Int32 count = 1; // at least one
#if defined (LINUX)  
    count = sysconf(_SC_NPROCESSORS_CONF);
#elif defined(_WIN32)  
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    count = si.dwNumberOfProcessors;
#endif  
    return count;
}

Int64 FS_CpuUtil::_CompareFileTime(FILETIME time1, FILETIME time2)
{
    Int64 a = (static_cast<Int64>(time1.dwHighDateTime) << 32) | static_cast<Int64>(time1.dwLowDateTime);
    Int64 b = (static_cast<Int64>(time2.dwHighDateTime) << 32) | static_cast<Int64>(time2.dwLowDateTime);

    return abs(b - a);
}

FS_NAMESPACE_END
