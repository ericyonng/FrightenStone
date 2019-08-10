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
 * @file  : FS_GuidUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/assist/utils/Impl/FS_GuidUtil.h"

FS_NAMESPACE_BEGIN

FS_GUID FS_GuidUtil::Gen()
{
    FS_GUID guid;
    ::memset(&guid, 0, sizeof(FS_GUID));

#ifndef _WIN32
    uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#else
    ::CoCreateGuid(&guid);
#endif

    return guid;
}

FS_String FS_GuidUtil::Format(const FS_GUID &guid)
{
    FS_String str;
    str.AppendFormat("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
               guid.Data1,
               guid.Data2,
               guid.Data3,
               guid.Data4[0],
               guid.Data4[1],
               guid.Data4[2],
               guid.Data4[3],
               guid.Data4[4],
               guid.Data4[5],
               guid.Data4[6],
               guid.Data4[7]);

    return str;
}

FS_String FS_GuidUtil::GenStr()
{
    return Format(Gen());
}

FS_NAMESPACE_END
