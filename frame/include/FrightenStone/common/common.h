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
#ifndef __Frame_Include_FrightenStone_Common_Common_H__
#define __Frame_Include_FrightenStone_Common_Common_H__
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
#include "FrightenStone/common/basedefs/Resource/Resource.h"
#pragma region base code
#ifndef __FRIGHTEN_STONE_FRAME_DLL__
#include <FrightenStone/common/status/status.h>
#include <FrightenStone/common/basedefs/BaseDefs.h>
#include <FrightenStone/common/asyn/asyn.h>
#include <FrightenStone/common/socket/socket.h>
#include <FrightenStone/common/assist/assist.h>
#include <FrightenStone/common/component/component.h>
#include <FrightenStone/common/crashhandle/CrashHandle.h>
#include <FrightenStone/common/log/Log.h>
#include <FrightenStone/common/event/event.h>
#include <FrightenStone/common/objpool/objpool.h>
#include <FrightenStone/common/memleak/memleak.h>
#include <FrightenStone/common/memorypool/memorypool.h>
#include <FrightenStone/common/memoryhelper/MemoryHelper.h>
#include <FrightenStone/common/net/net.h>
#include <FrightenStone/common/logicsys/logicsys.h>
#endif
#pragma endregion

#endif // !__Frame_Include_FrightenStone_Common_Common_H__
