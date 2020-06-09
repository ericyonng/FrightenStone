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
 * @file  : IFS_Buffer.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/27
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/IFS_Buffer.h"
#include "FrightenStone/common/assist/utils/utils.h"

#define __USE_FS_3RD_JSON___
#include "3rd/3rd.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_ANCESTOR_IMPL(IFS_Buffer, __DEF_OBJ_POOL_OBJ_NUM__);

FS_String IFS_Buffer::ToString() const
{
    FS_String info;
    FS_String raw;
    StringUtil::ToHexString(_buff, _curPos, raw);
    info.AppendFormat("bufferSize:%lld\n", _bufferSize)
        .AppendFormat("buff address:0x%p\n", _buff)
        .AppendFormat("curPos:%lld\n", _curPos)
        .AppendFormat("buffer data:\n%s\n", raw.c_str());

    return info;
}

FS_NAMESPACE_END
