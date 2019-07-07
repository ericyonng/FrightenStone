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
 * @file  : MemoryBlock.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/memorypool/Defs/MemoryBlock.h"

FS_NAMESPACE_BEGIN

MemoryBlock::MemoryBlock(IMemoryAlloctor *alloctor, const FS_String &objName)
    :_ref(0)
    ,_objSize(0)
    ,_alloctor(alloctor)
    ,_nextBlock(NULL)
    ,_isInPool(false)
    ,_objName{0}
//     ,_reserver1(0)
//     ,_reserver2(0)
//     ,_reserver3(0)
{
    auto len = sprintf(_objName, "%s", objName.c_str());
    if(len < 0)
        _objName[0] = 0;
    else
    {
        _objName[len >= BUFFER_LEN256 ? (BUFFER_LEN256 - 1) : len] = 0;
    }
}

FS_NAMESPACE_END
