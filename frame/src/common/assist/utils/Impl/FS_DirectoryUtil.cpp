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
 * @file  : FS_DirectoryUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
/**
* @file FS_DirectoryUtil.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/08
* @brief
*/

#include "stdafx.h"
#include "FrightenStone/common/assist/utils/Impl/FS_DirectoryUtil.h"
#include<errno.h>
#include<stdlib.h>

#ifdef _WIN32
#include<direct.h>      //mkdir func
#include<io.h>          //access func
#else
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#endif

FS_NAMESPACE_BEGIN

bool FS_DirectoryUtil::CreateDir(const FS_String &path)
{
    if(path.empty())
        return false;

    FS_String rootDir, subDir;
#ifdef _WIN32
    auto startPos = path.GetRaw().find(':', 0);    // judge if include path
    if(startPos == std::string::npos)
    {
        rootDir = "";
        subDir << "\\";
        subDir << path.GetRaw();
    }
    else
    {
        rootDir = path.GetRaw().substr(startPos - 1, 1);
        rootDir << ":\\";
        startPos = path.GetRaw().find("\\", startPos + 1);
        if(startPos == std::string::npos)
            return false;

        subDir = path.GetRaw().substr(startPos, path.GetLength() - startPos);
    }
#else
    if(path.GetRaw().at[0] == '/')
    {
        rootDir = "";
        subDir = path;
    }
    else
    {
        auto startPos = path.GetRaw().find_first_of('/', 0);
        if(startPos == std::string::npos)
            return false;

        subDir = path.GetRaw().substr(startPos, path.GetLength() - startPos);
        rootDir = path.GetRaw().substr(0, startPos);
    }
#endif

    return _CreateRecursiveDir(rootDir, subDir);
}

FS_String FS_DirectoryUtil::GetFileNameInPath(const FS_String &path)
{
    if(!(path.GetRaw().length() > 0))
        return "";

    Byte8 c = 0;
    Int32 i = 0;
    const Int32 len = static_cast<Int32>(path.GetRaw().length());
    for(i = len - 1; i >= 0; --i)
    {
        c = path.GetRaw().at(i);
        if(c == '\\' || c == '/')
        {
            ++i;
            break;
        }
    }

    if(UNLIKELY(i < 0))
        return "";


    return path.GetRaw().substr(i, len - i);
}

FS_String FS_DirectoryUtil::GetFileDirInPath(const FS_String &path)
{
    if(path.GetRaw().length() <= 0)
        return "";

    char c = 0;
    int i = 0;
    const Int32 len = static_cast<Int32>(path.GetRaw().length());
    for(i = len - 1; i >= 0; --i)
    {
        c = path.GetRaw().at(i);
        if(c == '\\' || c == '/')
        {
            ++i;
            break;
        }
    }

    if(i < 0)
        return "";

    return path.GetRaw().substr(0, i);
}

bool FS_DirectoryUtil::_CreateRecursiveDir(const FS_String &masterDir, const FS_String &subDir)
{
    std::string dir = "";
    std::string strtocreate = "";
    std::string strMasterPath = masterDir.GetRaw();
    int startPos = 0;
    int revStartPos = 0;
    int endPos = 0;
    int revEndPos = 0;

    int finalStartPos = 0, finalCount = 0;
    const int spritLen = static_cast<int>(strlen("\\"));
    const int revSpritLen = static_cast<int>(strlen("/"));
    while(true)
    {
        startPos = static_cast<int>(subDir.GetRaw().find("\\", startPos));
        revStartPos = static_cast<int>(subDir.GetRaw().find("/", revStartPos));
        if(startPos == std::string::npos && 
           revStartPos == std::string::npos) 
            break;

        if(startPos == std::string::npos && revStartPos != std::string::npos)
        {
            revEndPos = static_cast<int>(subDir.GetRaw().find("/", revStartPos + revSpritLen));
            if(revEndPos == std::string::npos) 
                break;

            finalStartPos = revStartPos + revSpritLen;
            finalCount = revEndPos - revStartPos - revSpritLen;
            revStartPos += revSpritLen;
        }
        else if(startPos != std::string::npos && revStartPos == std::string::npos)
        {
            endPos = static_cast<int>(subDir.GetRaw().find("\\", startPos + spritLen));
            if(endPos == std::string::npos)
                break;

            finalStartPos = startPos + spritLen;
            finalCount = endPos - startPos - spritLen;
            startPos += spritLen;
        }
        else if(startPos < revStartPos)
        {
            //"\\"
            endPos = static_cast<int>(subDir.GetRaw().find("\\", startPos + spritLen));
            revEndPos = static_cast<int>(subDir.GetRaw().find("/", startPos + spritLen));
            if(endPos == std::string::npos && 
               revEndPos == std::string::npos) 
                break;

            if(endPos != std::string::npos && revEndPos == std::string::npos)
            {
                finalStartPos = startPos + spritLen;
                finalCount = endPos - startPos - spritLen;
            }
            else if(endPos == std::string::npos && revEndPos != std::string::npos)
            {
                finalStartPos = startPos + spritLen;
                finalCount = revEndPos - startPos - spritLen;
            }
            else if(endPos < revEndPos)
            {
                //"\\"
                finalStartPos = startPos + spritLen;
                finalCount = endPos - startPos - spritLen;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }
            else
            {
                //"/"
                finalStartPos = startPos + spritLen;
                finalCount = revEndPos - startPos - spritLen;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }

            startPos += spritLen;
        }
        else
        {
            //"/"
            finalStartPos = revStartPos + revSpritLen;
            endPos = int(subDir.GetRaw().find("\\", revStartPos + revSpritLen));
            revEndPos = int(subDir.GetRaw().find("/", revStartPos + revSpritLen));
            if(endPos == std::string::npos &&
               revEndPos == std::string::npos) 
                break;

            if(endPos != std::string::npos && revEndPos == std::string::npos)
            {
                finalCount = endPos - finalStartPos;
            }
            else if(endPos == std::string::npos && revEndPos != std::string::npos)
            {
                finalCount = revEndPos - finalStartPos;
            }
            else if(endPos < revEndPos)
            {
                //"\\"
                finalCount = endPos - finalStartPos;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }
            else
            {
                //"/"
                finalCount = revEndPos - finalStartPos;
                //dir = strMasterPath + strSubDir.substr(iStartPos + strlen("\\"), iEndPos - iStartPos - strlen("\\"));
            }

            revStartPos += revSpritLen;
        }

        strtocreate.clear();
        dir = strMasterPath;
        if(dir.length() != 0) 
            dir += "/";

        dir += subDir.GetRaw().substr(finalStartPos, finalCount);
        strtocreate = dir;
        strtocreate += "/";
        if(strcmp((char *)strtocreate.c_str(), "./") != 0 && 
           strcmp((char *)strtocreate.c_str(), ".\\") != 0)
        {
            _CreateSubDir(strtocreate);
        }

        strMasterPath.clear();
        strMasterPath = dir;
    }

    return true;
}

bool FS_DirectoryUtil::_CreateSubDir(const std::string &subDir)
{
    if(subDir.length() <= 0)
        return false;

#ifdef _WIN32    
    if(::_mkdir(subDir.c_str()) != 0)
    {
        if(::_access(subDir.c_str(), 0) == -1)
            return false;
    }
#else
    if(mkdir(subDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
    {
        if(access(subDir.c_str(), 0) == -1)
            return false;
    }
#endif

    return true;
}

FS_NAMESPACE_END

