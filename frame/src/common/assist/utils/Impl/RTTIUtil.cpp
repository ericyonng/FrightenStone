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
 * @file  : RTTIUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/assist/utils/Impl/RTTIUtil.h"
#include "FrightenStone/common/component/Defs/TlsElementDefs.h"
#include "FrightenStone/common/component/Impl/FS_TlsTable.h"
#include "FrightenStone/common/component/Impl/ThreadTlsTableMgr.h"
#include "FrightenStone/common/assist/utils/Impl/FS_TlsUtil.h"

#ifndef _WIN32
// linux下类型识别接口相关
#include <cxxabi.h>
#endif

FS_NAMESPACE_BEGIN

// 从rtti中剔除str
#define __FS_GET_TYPE_NAME_Trim(str, len)        \
    it = rtti;                                   \
    skipCopy = 0;                                \
    itEnd = rtti + rawTyNameLen - totalSkipCopy; \
    while ((it = strstr(it, str)))               \
    {                                            \
        size_t copyLen = itEnd - it - len;       \
        memmove(it, it + len, copyLen);          \
        itEnd -= len;                            \
        skipCopy += len;                         \
        if (copyLen == 0)                        \
            break;                               \
    }                                            \
    totalSkipCopy += skipCopy;                   \
    rtti[rawTyNameLen - totalSkipCopy] = '\0'    \

const char *RTTIUtil::GetByTypeName(const char *rawTypeName)
{
#if _WIN32
    auto tlsTable = fs::FS_TlsUtil::GetUtilTlsTable();
    auto tlsRtti = tlsTable->GetElement<fs::Tls_Rtti>(fs::TlsElemType::Tls_Rtti);
    if(!tlsRtti)
        tlsRtti = tlsTable->AllocElement<fs::Tls_Rtti>(fs::TlsElemType::Tls_Rtti);

    char *rtti = tlsRtti->rtti;
    size_t rawTyNameLen = strlen(rawTypeName);
    memcpy(rtti, rawTypeName, rawTyNameLen);
    rtti[rawTyNameLen] = '\0';

    size_t totalSkipCopy = 0;

    char *it;
    char *itEnd;
    size_t skipCopy;
    __FS_GET_TYPE_NAME_Trim("class ", 6);
    __FS_GET_TYPE_NAME_Trim("struct ", 7);
    __FS_GET_TYPE_NAME_Trim(" *", 2);

    char *anonBeg = rtti;
    while((anonBeg = strchr(anonBeg, '`')))
    {
        *anonBeg = '(';
        char *anonEnd = strchr(anonBeg + 1, '\'');
        *anonEnd = ')';
    }

    return rtti;
#else // Non-Win32
    return GetCxxDemangle(rawTypeName);
#endif // LLBC_TARGET_PLATFORM_WIN32
}

#undef __FS_GET_TYPE_NAME_Trim

#ifndef _WIN32
const char *RTTIUtil::GetCxxDemangle(const char *name)
{
    auto tlsTable = fs::FS_TlsUtil::GetUtilTlsTable();
    if(UNLIKELY(!tlsTable))
        return "";

    auto tlsRtti = tlsTable->GetElement<fs::Tls_Rtti>(fs::TlsElemType::Tls_Rtti);
    if(!tlsRtti)
        tlsRtti = tlsTable->AllocElement<fs::Tls_Rtti>(fs::TlsElemType::Tls_Rtti);

    if(UNLIKELY(!tlsRtti))
        return "";

    int status = 0;
    size_t length = sizeof(tlsRtti->rtti);

    // 名字重整技术应用
    abi::__cxa_demangle(name, tlsRtti->rtti, &length, &status);
    if(status != 0)
        return "";

    return tlsRtti->rtti;
}
#endif

FS_NAMESPACE_END