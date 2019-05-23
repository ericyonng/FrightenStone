#ifdef __Base_Common_Component_Impl_File_LogFile__H__
/**
* @file    LogFileImpl.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/23
* @brief
*/

#pragma once

#include "base/common/component/Impl/FS_String.h"

FS_NAMESPACE_BEGIN

template<typename T>
inline Int64 LogFile::Write(FS_String &logStr)
{
    logStr <<"<"<<typeid(T).name()<< ">" << reinterpret_cast<const char *>(buffer) << FS_String::endl;
    return FS_File::Write(logStr.c_str(), logStr.GetLength());
}

FS_NAMESPACE_END

#endif
