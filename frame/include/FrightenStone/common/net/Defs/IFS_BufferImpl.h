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
 * @file  : IFS_BufferImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/13
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Defs_IFS_Buffer_H__
#pragma once

FS_NAMESPACE_BEGIN

inline IFS_Buffer::IFS_Buffer(size_t bufferSize, IMemoryAlloctor *memAlloctor)
    :_bufferSize(bufferSize)
    ,_buff(NULL)
    ,_curPos(0)
    ,_alloctor(memAlloctor)
{
    _Init();
}

inline IFS_Buffer::~IFS_Buffer()
{
    _Destroy();
}

inline void IFS_Buffer::PopFront(Int64 bytesLen)
{
    if(bytesLen > _curPos)
    {
        g_Log->e<IFS_Buffer>(_LOGFMT_("pop byteslen[%llu] bigger than curPos[%llu]"),
                             bytesLen, _curPos);
    }

    bytesLen = ((_curPos > bytesLen) ? bytesLen : _curPos);
    size_t n = _curPos - bytesLen;

    // TODO:如果考虑性能清零可不执行
    // ::memset(_buff, 0, bytesLen);

    // 后面还有没数据，若有数据挪到buf头
    if(n > 0)
        ::memcpy(_buff, _buff + bytesLen, n);

    _curPos = n;
}

inline void IFS_Buffer::PushBack(const Byte8 *data, size_t len)
{
    // 1.判断剩余空间是否足够 由外部判断canpush
    // size_t rest = _bufferSize - _curPos;
//     if(rest < len)
//     {
//         g_Log->e<IFS_Buffer>(_LOGFMT_("rest buffer size[%llu] not enough to match data len[%llu]"), rest, len);
//         return false;
//     }

    // 2.拷贝数据
    ::memcpy(_buff + _curPos, data, len);

    // 3.变更当前位置
    _curPos += len;
}

inline void IFS_Buffer::RollbackPush(Int64 len)
{
    _curPos < len ? (_curPos = 0) : (_curPos -= len);
}

inline bool IFS_Buffer::CanPush(size_t len)
{
    size_t rest = _bufferSize - _curPos;
    if(rest < len)
        return false;

    return true;
}

inline size_t IFS_Buffer::GetSuitableBytesToPush(size_t len)
{
    size_t rest = _bufferSize - _curPos;
    return len > rest ? rest : len;
}

inline void IFS_Buffer::Clear()
{
    // ::memset(_buff, 0, _bufferSize);
    _curPos = 0;
}

inline bool IFS_Buffer::IsEmpty() const
{
    return _curPos == 0;
}

inline bool IFS_Buffer::IsFull() const
{
    return _bufferSize <= _curPos;
}

inline Int64 IFS_Buffer::GetLength() const
{
    return _curPos;
}

inline const Int64 &IFS_Buffer::GetDataLen() const
{
    return _curPos;
}

inline Int64 IFS_Buffer::GetTotalSize() const
{
    return _bufferSize;
}

inline Int64 IFS_Buffer::GetRest() const
{
    return _bufferSize - _curPos;
}

inline Byte8 *IFS_Buffer::GetData()
{
    return _buff;
}

inline const Byte8 *IFS_Buffer::GetData() const
{
    return _buff;
}

inline Byte8 *IFS_Buffer::GetStartPush(Int64 *&curPos)
{
    curPos = &_curPos;
    return _buff + _curPos;
}

template<typename ObjType>
inline ObjType *IFS_Buffer::CastToBuffer()
{
    return static_cast<ObjType *>(this);
}

template<typename ObjType>
inline const ObjType *IFS_Buffer::CastToBuffer() const
{
    return static_cast<ObjType *>(this);
}

inline Int64 &IFS_Buffer::_GetCurPos()
{
    return _curPos;
}

inline void IFS_Buffer::_Init()
{
    if(!_bufferSize)
    {
        g_Log->w<IFS_Buffer>(_LOGFMT_("constructor buffersize is zero"));
        return;
    }

    if(_alloctor)
        _buff = _alloctor->MixAlloc<Byte8>(_bufferSize);
    else
        _buff = new Byte8[_bufferSize];
    _curPos = 0;
}

inline void IFS_Buffer::_Destroy()
{
    if(_buff)
    {
        if(_alloctor)
            _alloctor->MixFree(_buff);
        else
            Fs_SafeMultiDelete(_buff);

        _buff = NULL;
        _bufferSize = 0;
        _curPos = 0;
    }
}

FS_NAMESPACE_END

#endif
