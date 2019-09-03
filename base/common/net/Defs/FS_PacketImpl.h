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
#ifdef __Base_Common_Net_Defs_FS_Packet_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_Packet::FS_Packet(UInt64 clientId)
    :_cmd(0)
    ,_size(0)
    ,_buff(NULL)
    ,_clientId(clientId)
    ,_lastPos(0)
{
}

inline FS_Packet::FS_Packet(UInt64 clientId, char *buff, UInt16 bufferSize)
    :_cmd(0)
    , _size(bufferSize)
    , _buff(buff)
    ,_clientId(clientId)
    ,_lastPos(bufferSize)
{
}

inline FS_Packet::~FS_Packet()
{
    ClearBuffer();
}

inline void FS_Packet::FromMsg(UInt64 clientId, NetMsg_DataHeader *header)
{
    ClearBuffer();
    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(header->_packetLength);
    g_MemoryPool->Unlock();
    _size = header->_packetLength;
    _cmd = header->_cmd;
    _clientId = clientId;
    _lastPos = header->_packetLength;
}

inline NetMsg_DataHeader *FS_Packet::CastToMsg()
{
    return reinterpret_cast<NetMsg_DataHeader *>(_buff);
}

inline bool FS_Packet::IsFullPacket()
{
    return _size!=0 && _size == _lastPos;
}

template<typename T>
inline T *FS_Packet::CastToMsg()
{
    return reinterpret_cast<T*>(_buff);
}

inline char *FS_Packet::GiveupBuffer(UInt16 &bufferSize, Int32 &lastPos)
{
    auto buff = _buff;
    bufferSize = _size;

    _cmd = 0;
    _size = 0;
    _buff = NULL;
    _clientId = 0;
    _lastPos = 0;
    return buff;
}

inline void FS_Packet::ClearBuffer()
{
    if(_buff)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(_buff);
        g_MemoryPool->Unlock();
        _buff = NULL;
        _size = 0;
        _cmd = 0;
        _clientId = 0;
        _lastPos = 0;
    }
}

FS_NAMESPACE_END

#endif
