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
 * @file  : MemoryPoolDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/6
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_MemoryPool_Defs_MemoryPoolDefs_H__
#define __Base_Common_MemoryPool_Defs_MemoryPoolDefs_H__
#pragma once

#undef __MEMORY_POOL_ALIGN_BYTES__
#define __MEMORY_POOL_ALIGN_BYTES__          (sizeof(void *)<<1)    // 默认16字节对齐 涉及到跨cache line开销

#undef __FS_MEMORY_ALIGN__
#define __FS_MEMORY_ALIGN__(BYTES)  \
(BYTES) / __MEMORY_POOL_ALIGN_BYTES__ * __MEMORY_POOL_ALIGN_BYTES__ + ((BYTES)%__MEMORY_POOL_ALIGN_BYTES__ ? __MEMORY_POOL_ALIGN_BYTES__ : 0)

#undef __MEMORY_POOL_MINIMUM_BLOCK__
#define __MEMORY_POOL_MINIMUM_BLOCK__        64          // 最小内存块64字节

#undef __MEMORY_POOL_MAXIMUM_BLOCK__
#define __MEMORY_POOL_MAXIMUM_BLOCK__        131072       // 最大内存块128K 只支持64的倍数

#undef __MEMORY_POOL_MAXBLOCK_LIMIT__
#define __MEMORY_POOL_MAXBLOCK_LIMIT__      __MEMORY_POOL_MAXIMUM_BLOCK__   // 能够支持的最大内存块范围

#undef __MEMORY_POOL_MAX_EXPAND_BYTES__
#define __MEMORY_POOL_MAX_EXPAND_BYTES__    4294967296llu      // 内存池最大只支持到4GB，其他情况使用系统分配

#ifndef BLOCK_AMOUNT_DEF
#define BLOCK_AMOUNT_DEF    128    // 默认内存块数量
#endif

#endif
