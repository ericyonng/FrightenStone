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
 * @file  : IFS_BufferImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/13
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Net_Defs_IFS_Buffer_H__
#pragma once

FS_NAMESPACE_BEGIN

inline IFS_Buffer::IFS_Buffer(size_t bufferSize)
    :_bufferSize(bufferSize)
    ,_buff(NULL)
    ,_curPos(0)
    , _sessionId(0)
    , _socket(INVALID_SOCKET)
{
    _Init();
}

inline IFS_Buffer::~IFS_Buffer()
{
    _Destroy();
}

inline bool IFS_Buffer::BindTo(UInt64 sessionId, SOCKET sock)
{
    if(_sessionId)
    {
        g_Log->w<IFS_Buffer>(_LOGFMT_("this buffer has owner sessionId[%llu] socket[%llu] cant bind to new sessionId[%llu] new socket[%llu]")
                                , _sessionId, _socket, sessionId, sock);
        return false;
    }
    _sessionId = sessionId;
    _socket = sock;
    return true;
}

inline UInt64 IFS_Buffer::GetSessionId() const
{
    return _sessionId;
}

inline SOCKET IFS_Buffer::GetSocket() const
{
    return _socket;
}

inline void IFS_Buffer::PopFront(size_t bytesLen)
{
    if(bytesLen > _curPos)
    {
        g_Log->e<IFS_Buffer>(_LOGFMT_("pop byteslen[%llu] bigger than curPos[%llu]"),
                             bytesLen, _curPos);
    }

    bytesLen = _curPos > bytesLen ? bytesLen : _curPos;
    size_t n = _curPos - bytesLen;

    // TODO:如果考虑性能清零可不执行
    // ::memset(_buff, 0, bytesLen);

//     if(n > 0)
//         ::memmove(_buff, _buff + bytesLen, n);
    if(n > 0)
        ::memcpy(_buff, _buff + bytesLen, n);

    _curPos = n;
}

inline bool IFS_Buffer::PushBack(const Byte8 *data, size_t len)
{
    // 1.判断剩余空间是否足够
    size_t rest = _bufferSize - _curPos;
    if(rest < len)
    {
        g_Log->e<IFS_Buffer>(_LOGFMT_("rest buffer size[%llu] not enough to match data len[%llu]"), rest, len);
        return false;
    }

    // 2.拷贝数据
    ::memcpy(_buff + _curPos, data, len);

    // 3.变更当前位置
    _curPos += len;
    return true;
}

inline bool IFS_Buffer::CanPush(size_t len)
{
    size_t rest = _bufferSize - _curPos;
    if(rest < len)
        return false;

    return true;
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
    return _bufferSize == _curPos;
}

inline size_t IFS_Buffer::GetLength() const
{
    return _curPos;
}

inline size_t IFS_Buffer::GetTotalSize() const
{
    return _bufferSize;
}

inline size_t IFS_Buffer::GetRest() const
{
    return _bufferSize - _curPos;
}

inline char *IFS_Buffer::GetData()
{
    return _buff;
}

inline const char *IFS_Buffer::GetData() const
{
    return _buff;
}

template<typename ObjType>
inline ObjType *IFS_Buffer::CastToData()
{
    return reinterpret_cast<ObjType *>(_buff);
}

template<typename ObjType>
inline const ObjType *IFS_Buffer::CastToData() const
{
    return reinterpret_cast<const ObjType *>(_buff);
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

inline size_t &IFS_Buffer::_GetCurPos()
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

    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(_bufferSize);
    g_MemoryPool->Unlock();
    _curPos = 0;
}

inline void IFS_Buffer::_Destroy()
{
    if(_buff)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(_buff);
        g_MemoryPool->Unlock();
        _buff = NULL;
        _bufferSize = 0;
        _curPos = 0;
    }
}

FS_NAMESPACE_END

#endif
