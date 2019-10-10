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
 * @file  : FS_TlsUtilImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/10
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Assist_Utils_Impl_FS_TlsUtil_H__
#pragma once

FS_NAMESPACE_BEGIN

inline Int32 FS_TlsUtil::CreateUtilTlsHandle()
{
    auto &tlsHandle = GetUtileTlsHandle();
    tlsHandle = CreateTlsHandle();
    if(tlsHandle == FS_INVALID_TLS_HANDLE)
        return StatusDefs::Tls_CreateHandleFail;

    return StatusDefs::Success;
}

inline void FS_TlsUtil::DestroyUtilTlsHandle()
{
    DestroyTlsHandle(GetUtileTlsHandle());
}

inline FS_TlsTable *FS_TlsUtil::GetUtilTlsTable()
{
    return GetTlsTable(GetUtileTlsHandle());
}

inline void FS_TlsUtil::FreeUtilTlsTable()
{
    FreeTlsTable(GetUtileTlsHandle());
}

template<typename ObjType>
inline ObjType *FS_TlsUtil::UtilGetAndCreateWithDefBuilder(Int32 elemType)
{
    auto tlsTable = GetUtilTlsTable();
    auto elem = tlsTable->GetElement<ObjType>(elemType);
    if(!elem)
        elem = tlsTable->AllocElement<ObjType>(elemType);

    return elem;
}

FS_NAMESPACE_END

#endif
