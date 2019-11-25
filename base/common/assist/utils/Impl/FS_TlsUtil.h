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
 * @file  : FS_TlsUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/10
 * @brief :
 *          线程局部存储工具类
 *
 * 
 */
#ifndef __Base_Common_Assist_Utils_Impl_FS_TlsUtil_H__
#define __Base_Common_Assist_Utils_Impl_FS_TlsUtil_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Defs/Tls/FS_TlsHandle.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_TlsTable.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_TlsTable;

class BASE_EXPORT FS_TlsUtil
{
public:
    // 公用的util的handle
    static FS_TlsHandle &GetUtileTlsHandle();
    static Int32 CreateUtilTlsHandle();
    static void DestroyUtilTlsHandle();
    static FS_TlsTable *GetUtilTlsTable();
    static void FreeUtilTlsTable();
    template<typename ObjType>
    static ObjType *UtilGetAndCreateWithDefBuilder(Int32 elemType);

    // 指定tlshandle
    static FS_TlsHandle CreateTlsHandle();
    static void DestroyTlsHandle(FS_TlsHandle &tlsHandle);
    static FS_TlsTable *GetTlsTable(FS_TlsHandle tlsHandle);
    static void FreeTlsTable(FS_TlsHandle tlsHandle);
};

FS_NAMESPACE_END

#include "base/common/assist/utils/Impl/FS_TlsUtilImpl.h"

#endif
