#ifndef __Base_Common_Component_Impl_File_LogFile__H__
#define __Base_Common_Component_Impl_File_LogFile__H__
/**
* @file    LogFile.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/23
* @brief
*/

#pragma once

#include "base/common/component/Impl/File/FS_File.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT LogFile :public FS_File
{
public:
    LogFile();
    virtual ~LogFile();

public:
    template<typename T>
    Int64 Write(FS_String &logStr);
    bool IsDayPass(const Time &lastModifyTime) const;
    void UpdateLastTimestamp();

protected:
    Time    _lastModifyTime;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/File/LogFileImpl.h"
#endif
