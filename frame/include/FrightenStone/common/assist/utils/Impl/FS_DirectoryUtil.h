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
 * @file  : FS_DirectoryUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_FS_DirectoryUtil_H__
#define __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_FS_DirectoryUtil_H__
/**
* @file FS_DirectoryUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/08
* @brief
*/

#pragma once

#include<FrightenStone/exportbase.h>
#include "FrightenStone/common/basedefs/Macro/ForAll/ForAllMacro.h"
#include "FrightenStone/common/component/component.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_DirectoryUtil
{
public:
    // create folder
    static bool CreateDir(const FS_String &path);

    // get file/dir from path
    static FS_String GetFileNameInPath(const FS_String &path);
    static FS_String GetFileNameInPath(const char *path);
    static FS_String GetFileDirInPath(const FS_String &path);
    static FS_String GetFileDirInPath(const char *path);

private:
    // create sub dir
    static bool _CreateSubDir(const FS_String &subDir);
    static bool _CreateSubDir(const std::string &subDir);
    // recursive create dir
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
