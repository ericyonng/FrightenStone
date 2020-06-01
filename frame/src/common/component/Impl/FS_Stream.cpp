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
 * @file  : FS_Stream.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/8/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/component/Impl/FS_Stream.h"
#include "FrightenStone/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_Stream, __DEF_OBJ_POOL_OBJ_NUM__);
FS_Stream::FS_Stream(int size)
{
    _size = size;
    g_MemoryPool->Lock();
    _buff = reinterpret_cast<char *>(g_MemoryPool->Alloc<char>(_size));
    g_MemoryPool->Unlock();
    _isPoolCreate = true;
    _needDelete = true;
}

FS_Stream::~FS_Stream()
{
    _Clear();
}

void FS_Stream::Release()
{
    delete this;
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

bool FS_Stream::DeserializeFrom(const FS_String &str)
{
    if(str.empty())
        return false;

    _Clear();

    const Int32 sizeBytes = sizeof(_size);
    const Int32 wrPosBytes = sizeof(_writePos);
    const Int32 rdPosBytes = sizeof(_readPos);
    const Int32 ndDelBytes = sizeof(_needDelete);
    const Int32 isPoolCreateBytes = sizeof(_isPoolCreate);

    Int32 leftBytes = static_cast<Int32>(str.size());
    if(leftBytes < sizeBytes)
        return false;
    Int32 offsetPos = 0;
    Int32 resSize = static_cast<Int32>(str.CopyTo(reinterpret_cast<char *>(&_size), sizeBytes, sizeBytes, offsetPos));
    if(resSize != sizeBytes)
        return false;
    leftBytes -= resSize;

    if(leftBytes < wrPosBytes)
        return false;
    offsetPos += resSize;
    Int32 resWr = static_cast<Int32>(str.CopyTo(reinterpret_cast<char *>(&_writePos), wrPosBytes, wrPosBytes, offsetPos));
    if(resWr != wrPosBytes)
        return false;
    leftBytes -= resWr;

    if(leftBytes < rdPosBytes)
        return false;
    offsetPos += resWr;
    Int32 resRd = static_cast<Int32>(str.CopyTo(reinterpret_cast<char *>(&_readPos), rdPosBytes, rdPosBytes, offsetPos));
    if(resRd != rdPosBytes)
        return false;
    leftBytes -= resRd;

    if(leftBytes < ndDelBytes)
        return false;
    offsetPos += resRd;
    Int32 resNdDel = static_cast<Int32>(str.CopyTo(reinterpret_cast<char *>(&_needDelete), ndDelBytes, ndDelBytes, offsetPos));
    if(resNdDel != ndDelBytes)
        return false;
    leftBytes -= resNdDel;

    if(leftBytes < isPoolCreateBytes)
        return false;
    offsetPos += resNdDel;
    Int32 resIsPoolCreate = static_cast<Int32>(str.CopyTo(reinterpret_cast<char *>(&_isPoolCreate), isPoolCreateBytes, isPoolCreateBytes, offsetPos));
    if(resIsPoolCreate != isPoolCreateBytes)
        return false;
    leftBytes -= resIsPoolCreate;

    auto validSize = _writePos > _readPos ? _writePos : _readPos;
    if(validSize)
    {
        if(_isPoolCreate)
        {
            g_MemoryPool->Lock();
            _buff = reinterpret_cast<char *>(g_MemoryPool->Alloc<char>(_size));
            g_MemoryPool->Unlock();
        }
        else
        {
            _buff = new char[_size];
        }
        offsetPos += resIsPoolCreate;
        Int32 resBuff = static_cast<Int32>(str.CopyTo(_buff, _size, validSize, offsetPos));
        if(resBuff != validSize)
            return false;
    }

    return true;
}

void FS_Stream::_Clear()
{
    if(_needDelete && _buff)
    {
        if(_isPoolCreate)
        {
            g_MemoryPool->Lock();
            g_MemoryPool->Free(_buff);
            g_MemoryPool->Unlock();
            _buff = NULL;
        }
        else
        {
            delete[]_buff;
            _buff = NULL;
        }
    }
}
FS_NAMESPACE_END