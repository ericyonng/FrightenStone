/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : BriefAddrInfo.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/03/18
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_BriefAddrInfo_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_BriefAddrInfo_H__
#pragma once

#include <FrightenStone/exportbase.h>
#include <FrightenStone/common/basedefs/BaseDefs.h>
#include <FrightenStone/common/component/Impl/FS_String.h>

FS_NAMESPACE_BEGIN

struct BASE_EXPORT BriefAddrInfo
{
    OBJ_POOL_CREATE_ANCESTOR(BriefAddrInfo);
    BriefAddrInfo();

    FS_String _ip;
    UInt16 _port;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Defs/BriefAddrInfoImpl.h"
#endif
