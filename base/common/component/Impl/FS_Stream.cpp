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
 * @file  : FS_Stream.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/FS_Stream.h"
#include "base/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_IMPL(FS_Stream, _objPoolHelper, __DEF_OBJ_POOL_OBJ_NUM__)
FS_Stream::FS_Stream(int size)
{
    _size = size;
    g_MemoryPool->Lock();
    _buff = reinterpret_cast<char *>(g_MemoryPool->Alloc(_size));
    g_MemoryPool->Unlock();
    _isPoolCreate = true;
    _needDelete = true;
}

FS_Stream::~FS_Stream()
{
    if(_needDelete && _buff)
    {
        if(_isPoolCreate)
        {
            g_MemoryPool->Lock();
            g_MemoryPool->Free(_buff);
            g_MemoryPool->Unlock();
        }
        else
        {
            delete []_buff;
            _buff = NULL;
        }
    }
}

bool FS_Stream::ReadString(std::string &str)
{
    UInt32 len = 0;
    Read(len, false);

    if(len > 0)
    {
        // 判断能不能读出
        if(CanRead(len + sizeof(UInt32)))
        {
            // 计算已读位置+数组长度所占有空间
            OffsetOnReadChange(sizeof(UInt32));

            // 将要读取的数据 拷贝出来
            str.insert(0, _buff + _readPos, len);

            // 计算已读数据位置
            OffsetOnReadChange(len);
            return true;
        }
    }

    return false;
}

FS_NAMESPACE_END