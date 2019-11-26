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
 * @file  : GlobalDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/05/31
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_BaseDefs_Macro_MacroDefs_ForAll_GlobalDefs_H__
#define __Base_Common_BaseDefs_Macro_MacroDefs_ForAll_GlobalDefs_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

// openssl全局锁
class BASE_EXPORT Locker;
extern BASE_EXPORT Locker g_OpenSSL_Locker;

// 默认的时间轮盘对象
class BASE_EXPORT TimeWheel;
extern BASE_EXPORT TimeWheel g_TimeWheel;

FS_NAMESPACE_END

#endif
