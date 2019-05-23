#ifndef __Base_Common_Assist_Utils_Impl_FS_DirectoryUtil_H__
#define __Base_Common_Assist_Utils_Impl_FS_DirectoryUtil_H__
/**
* @file FS_DirectoryUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/08
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/Macro/ForAll/ForAllMacro.h"
#include "base/common/component/component.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_DirectoryUtil
{
public:
    // 递归创建目录
    static bool CreateDir(const FS_String &path);

    //路径中截取文件
    static FS_String GetFileNameInPath(const FS_String &path);
    static FS_String GetFileNameInPath(const char *path);
    static FS_String GetFileDirInPath(const FS_String &path);
    static FS_String GetFileDirInPath(const char *path);

private:
    // 创建子文件夹
    static bool _CreateSubDir(const FS_String &subDir);
    static bool _CreateSubDir(const std::string &subDir);
    // 递归创建子目录
    static bool _CreateRecursiveDir(const FS_String &masterDir, const FS_String &subDir);
};

#pragma region Inline
inline FS_String FS_DirectoryUtil::GetFileNameInPath(const char *path)
{
    return GetFileNameInPath(FS_String(path));
}

inline FS_String FS_DirectoryUtil::GetFileDirInPath(const char *path)
{
    return GetFileDirInPath(FS_String(path));
}

inline bool FS_DirectoryUtil::_CreateSubDir(const FS_String &subDir)
{
    return _CreateSubDir(subDir.GetRaw());
}

#pragma endregion

FS_NAMESPACE_END

#endif
