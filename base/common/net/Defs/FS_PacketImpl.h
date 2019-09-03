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

inline FS_Packet::FS_Packet(UInt64 ownerId)
    : _packetSize(0)
    , _buff(NULL)
    , _ownerId(ownerId)
    , _lastPos(0)
{

}

inline FS_Packet::FS_Packet(UInt64 ownerId, Int32 packetSize)
    :_packetSize(packetSize)
    ,_buff(NULL)
    ,_ownerId(ownerId)
    ,_lastPos(0)
{
    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(packetSize);
    g_MemoryPool->Unlock();
}

inline FS_Packet::FS_Packet(UInt64 ownerId, char *buff, Int32 packetSize)
    : _packetSize(packetSize)
    , _buff(buff)
    ,_ownerId(ownerId)
    ,_lastPos(packetSize)
{
}

inline FS_Packet::~FS_Packet()
{
    ClearBuffer();
}

inline void FS_Packet::FromMsg(UInt64 ownerId, NetMsg_DataHeader *header)
{
    ClearBuffer();
    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(static_cast<size_t>(header->_packetLength));
    g_MemoryPool->Unlock();
    _packetSize = header->_packetLength;
    _ownerId = ownerId;
    _lastPos = header->_packetLength;
}

inline void FS_Packet::FromMsg(UInt64 ownerId, const char *data, Int32 len)
{
    ClearBuffer();
    g_MemoryPool->Lock();
    _buff = g_MemoryPool->Alloc<char>(static_cast<size_t>(len));
    g_MemoryPool->Unlock();
    _packetSize = len;
    _ownerId = ownerId;
    _lastPos = len;

    ::memcpy(_buff, data, len);
}

inline NetMsg_DataHeader *FS_Packet::CastToMsg()
{
    return reinterpret_cast<NetMsg_DataHeader *>(_buff);
}

template<typename T>
inline T *FS_Packet::CastToMsg()
{
    return reinterpret_cast<T*>(_buff);
}

inline char *FS_Packet::GiveupBuffer(Int32 &packetSize, Int32 &lastPos)
{
    auto buff = _buff;
    packetSize = _packetSize;

    _packetSize = 0;
    _buff = NULL;
    _ownerId = 0;
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
        _packetSize = 0;
        _ownerId = 0;
        _lastPos = 0;
        Fs_SafeFree(_ioData._completedCallback);
    }
}

inline IO_DATA_BASE *FS_Packet::MakeRecvIoData()
{
    int len = _packetSize - _lastPos;
    if(len > 0)
    {
        _ioData._wsaBuff.buf = _buff + _lastPos;
        _ioData._wsaBuff.len = len;
        _ioData._ownerId = _ownerId;
        if(!_ioData._completedCallback)
            _ioData._completedCallback = DelegatePlusFactory::Create(this, &FS_Packet::_OnRecvSucCallback);
        return &_ioData;
    }

    return NULL;
}

inline IO_DATA_BASE *FS_Packet::MakeSendIoData()
{
    if(_lastPos > 0)
    {
        _ioData._wsaBuff.buf = _buff;
        _ioData._wsaBuff.len = _lastPos;
        _ioData._ownerId = _ownerId;
        if(!_ioData._completedCallback)
            _ioData._completedCallback = DelegatePlusFactory::Create(this, &FS_Packet::_OnSendSucCallback);
        return &_ioData;
    }

    return NULL;
}

inline bool FS_Packet::IsFullPacket()
{
    return _packetSize!=0 && _packetSize == _lastPos;
}

inline bool FS_Packet::IsEmpty()
{
    return _lastPos == 0;
}

inline bool FS_Packet::HasMsg() const
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

inline bool FS_Packet::NeedWrite() const
{
    return _lastPos > 0;
}

FS_NAMESPACE_END

#endif
