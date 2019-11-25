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
 * @file  : ObjPoolDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/25
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_ObjPool_Defs_ObjPoolDefs_H__
#define __Base_Common_ObjPool_Defs_ObjPoolDefs_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/FS_Delegate.h"

#pragma region macro
#undef __DEF_OBJ_POOL_OBJ_NUM__
#define __DEF_OBJ_POOL_OBJ_NUM__            512
// #undef __OBJPOOL_ALIGN_BYTES__
// #define __OBJPOOL_ALIGN_BYTES__             (sizeof(void *)<<1)    // 默认16字节对齐 涉及到跨cache line开销
#undef __OBJ_POOL_MAX_ALLOW_BYTES_DEF__
#define __OBJ_POOL_MAX_ALLOW_BYTES_DEF__       2147483648llu              // 默认最大允许占用的对象池大小（2GB）
#pragma endregion

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_String;

class BASE_EXPORT ObjPoolDefs
{
public:
    static const Int32 __g_FreeRate;      // 对象池空闲率
};

class BASE_EXPORT ObjPoolMethods
{
public:
    static void PrintObjPoolInfo(const char *objName, size_t nodeCnt, size_t totalObjBlocks, size_t objBlockSizes, size_t bytesOccupied, size_t memObjInUsingCnt, size_t memInUsingBytes, const char *extStr);
    static void RegisterToMemleakMonitor(const char *objName, IDelegate<size_t, Int64 &, Int64 &, const char *> *callback);
    static void UnRegisterMemleakDelegate(const char *objName);

    static void RegisterModifyAllowMaxBytes(IDelegate<void, UInt64> *callback);
};

FS_NAMESPACE_END

#endif
