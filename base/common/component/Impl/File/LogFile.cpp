/**
* @file    LogFile.cpp
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/23
* @brief
*/

#include "stdafx.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/component/Impl/File/LogFile.h"

FS_NAMESPACE_BEGIN

LogFile::LogFile()
{
    
}

LogFile::~LogFile()
{

}

bool LogFile::IsDayPass(const Time &lastModifyTime) const
{
    return (lastModifyTime - _lastModifyTime).GetTotalDays() != 0;
}

void LogFile::UpdateLastTimestamp()
{
    _lastModifyTime.FlushTime();
}

FS_NAMESPACE_END
