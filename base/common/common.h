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
 * @file  : common.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Common_H__
#define __Base_Common_Common_H__
/**
* @file net.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

// defs ...
// Impl ...
// Interface ...
#include "base/common/basedefs/Resource/Resource.h"
#pragma region base code
#ifndef FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL
#include<base/common/socket/socket.h>
#include <base/common/net/net.h>
#include <base/common/status/status.h>
#include <base/common/asyn/asyn.h>
#include <base/common/basedefs/BaseDefs.h>
#include <base/common/assist/assist.h>
#include <base/common/component/component.h>
#include <base/common/log/Log.h>
#include <base/common/Global/EasyGlobal.h>
#include <base/common/crashhandle/CrashHandle.h>
#include <base/common/objpool/objpool.h>
#include <base/common/Application/Application.h>
#include <base/common/memleak/memleak.h>
#include <base/common/event/event.h>
#include <base/common/memorypool/memorypool.h>
#endif
#pragma endregion

#endif // !__Base_Common_Common_H__
