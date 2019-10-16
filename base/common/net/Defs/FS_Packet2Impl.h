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
 * @file  : FS_PacketImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/31
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Net_Defs_FS_Packet2_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_Packet2::FS_Packet2(UInt64 ownerId)
    : _packetSize(0)
    , _buff(NULL)
    , _sessionId(ownerId)
    ,_socket(0)
    , _lastPos(0)
{

}


inline FS_Packet2::~FS_Packet2()
{
    Clear();
}

inline void FS_Packet2::FromMsg(UInt64 ownerId, SOCKET socket, NetMsg_DataHeader *header)
{
    Clear();
    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(static_cast<size_t>(header->_packetLength));
    g_MemoryPool->Unlock();
    _packetSize = header->_packetLength;
    _sessionId = ownerId;
    _socket = socket;
    _lastPos = header->_packetLength;
}

inline void FS_Packet2::FromMsg(NetMsg_DataHeader *header)
{
    FromMsg(reinterpret_cast<const char *>(header), header->_packetLength);
}

inline void FS_Packet2::FromMsg(const char *data, Int32 len)
{
    if(_buff)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(_buff);
        g_MemoryPool->Unlock();
        _buff = NULL;
        _packetSize = 0;
        _lastPos = 0;
        Fs_SafeFree(_ioData._callback);
    }

    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(static_cast<size_t>(len));
    g_MemoryPool->Unlock();
    _packetSize = len;
    _lastPos = len;
    ::memcpy(_buff, data, len);
}

inline void FS_Packet2::FromMsg(UInt64 ownerId, SOCKET socket, const char *data, Int32 len)
{
    Clear();
    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(static_cast<size_t>(len));
    g_MemoryPool->Unlock();
    _packetSize = len;
    _sessionId = ownerId;
    _lastPos = len;

    ::memcpy(_buff, data, len);
}

inline NetMsg_DataHeader *FS_Packet2::CastToMsg()
{
    return reinterpret_cast<NetMsg_DataHeader *>(_buff);
}

template<typename ObjType>
inline ObjType *FS_Packet2::CastToMsg()
{
    return reinterpret_cast<ObjType*>(_buff);
}

inline char *FS_Packet2::GiveupBuffer(Int32 &packetSize, Int32 &lastPos)
{
    auto buff = _buff;
    packetSize = _packetSize;

    _packetSize = 0;
    _buff = NULL;
    _sessionId = 0;
    _lastPos = 0;
    return buff;
}

inline void FS_Packet2::Clear()
{
    if(_buff)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(_buff);
        g_MemoryPool->Unlock();
        _buff = NULL;
        _packetSize = 0;
        _sessionId = 0;
        SocketUtil::DestroySocket(_socket);
        _socket = INVALID_SOCKET;
        _lastPos = 0;
        Fs_SafeFree(_ioData._callback);
    }
}

inline bool FS_Packet2::IsFullPacket()
{
    return _packetSize!=0 && _packetSize == _lastPos;
}

inline bool FS_Packet2::IsEmpty()
{
    return _lastPos == 0;
}

inline bool FS_Packet2::HasMsg() const
{
    // 判断消息缓冲区的数据长度大于消息头NetMsg_DataHeader长度
    if(_lastPos >= sizeof(NetMsg_DataHeader))
    {
        // 这时就可以知道当前消息的长度
        NetMsg_DataHeader *header = (NetMsg_DataHeader*)_buff;
        // 判断消息缓冲区的数据长度大于消息长度
        return _lastPos >= header->_packetLength;
    }

    return false;
}

inline bool FS_Packet2::NeedWrite() const
{
    return _lastPos > 0;
}

inline UInt64 FS_Packet2::GetSessionId() const
{
    return _sessionId;
}

inline SOCKET FS_Packet2::GetSocket() const
{
    return _socket;
}

inline void FS_Packet2::_FromMsg(const char *data, Int32 len)
{
    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(static_cast<size_t>(len));
    g_MemoryPool->Unlock();
    _packetSize = len;
    _lastPos = len;
    ::memcpy(_buff, data, len);
}

inline void FS_Packet2::_Clear()
{
    if(_buff)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(_buff);
        g_MemoryPool->Unlock();
        _buff = NULL;
        _packetSize = 0;
        _lastPos = 0;
        Fs_SafeFree(_ioData._callback);
        ::memset(&_ioData, 0, sizeof(_ioData));
    }
}

FS_NAMESPACE_END

#endif
