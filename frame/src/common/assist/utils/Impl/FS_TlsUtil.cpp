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
 * @file  : FS_TlsUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/assist/utils/Impl/FS_TlsUtil.h"
#include "FrightenStone/common/component/Defs/Tls/FS_TlsHandle.h"
//#include "FrightenStone/common/component/Impl/FS_TlsTable.h"
#include "FrightenStone/common/status/status.h"

FS_NAMESPACE_BEGIN

// 定义全局的tlshandle
static FS_TlsHandle g_FsUtilTlsHandle = FS_INVALID_TLS_HANDLE;

FS_TlsHandle &FS_TlsUtil::GetUtileTlsHandle()
{
    return g_FsUtilTlsHandle;
}

FS_TlsHandle FS_TlsUtil::CreateTlsHandle()
{
    bool tlsCreated = false;
    FS_TlsHandle tlsHandle = FS_INVALID_TLS_HANDLE;
#ifndef _WIN32
    tlsCreated = (pthread_key_create(&tlsHandle, NULL) == 0) ? true : false;
#else
    tlsCreated = ((tlsHandle = ::TlsAlloc()) != TLS_OUT_OF_INDEXES) ? true : false;
#endif

    if(!tlsCreated)
        return FS_INVALID_TLS_HANDLE;

    return tlsHandle;
}

void FS_TlsUtil::DestroyTlsHandle(FS_TlsHandle &tlsHandle)
{
#ifndef _WIN32
    (void)pthread_key_delete(tlsHandle);
#else
    (void)::TlsFree(tlsHandle);
#endif

    tlsHandle = FS_INVALID_TLS_HANDLE;
}

FS_TlsTable *FS_TlsUtil::GetTlsTable(FS_TlsHandle tlsHandle)
{
    if(UNLIKELY(tlsHandle == FS_INVALID_TLS_HANDLE))
        return NULL;

    FS_TlsTable *tlsTable = NULL;
#ifndef _WIN32
    tlsTable = reinterpret_cast<FS_TlsTable *>(pthread_getspecific(tlsHandle));
#else
    tlsTable = reinterpret_cast<FS_TlsTable *>(::TlsGetValue(tlsHandle));
#endif

    if(UNLIKELY(!tlsTable))
    {
        tlsTable = new FS_TlsTable();

#ifndef _WIN32
        (void)pthread_setspecific(tlsHandle, tlsTable);
#else
        (void)::TlsSetValue(tlsHandle, tlsTable);
#endif
    }

    return tlsTable;
}

void FS_TlsUtil::FreeTlsTable(FS_TlsHandle tlsHandle)
{
    FS_TlsTable *tlsTable = NULL;
#ifndef _WIN32
    if((tlsTable = reinterpret_cast<FS_TlsTable *>(
        pthread_getspecific(tlsHandle))))
    {
        Fs_SafeFree(tlsTable);
        pthread_setspecific(tlsHandle, NULL);
    }
#else
    if((tlsTable = reinterpret_cast<FS_TlsTable *>(
        ::TlsGetValue(tlsHandle))))
    {
        Fs_SafeFree(tlsTable);
        ::TlsSetValue(tlsHandle, NULL);
    }
#endif
}

FS_NAMESPACE_END
