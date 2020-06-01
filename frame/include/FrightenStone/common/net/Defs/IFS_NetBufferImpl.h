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
 * @file  : IFS_NetBufferImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/4/16
 * @brief :
 */

#ifdef __Frame_Include_FrightenStone_Common_Net_Defs_IFS_NetBuffer_H__

#pragma once

FS_NAMESPACE_BEGIN

inline IFS_NetBuffer::IFS_NetBuffer(size_t bufferSize, IMemoryAlloctor *memAlloctor)
    :IFS_Buffer(bufferSize, memAlloctor)
    , _sessionId(0)
    , _socket(INVALID_SOCKET)
{
}

inline IFS_NetBuffer::~IFS_NetBuffer()
{
}

inline bool IFS_NetBuffer::BindTo(UInt64 sessionId, SOCKET sock)
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

inline UInt64 IFS_NetBuffer::GetSessionId() const
{
    return _sessionId;
}

inline SOCKET IFS_NetBuffer::GetSocket() const
{
    return _socket;
}
// 
// inline bool IFS_NetBuffer::HasMsg() const
// {
//     // 判断消息缓冲区的数据长度大于消息头NetMsg_DataHeader长度
//     auto len = GetLength();
//     if(len >= NetMsgHeaderFmtType::_msgHeaderSize)
//     {
//         // 这时就可以知道当前消息的长度
//         const NetMsgHeaderFmtType::PacketLenDataType *packetLen =
//             reinterpret_cast<const NetMsgHeaderFmtType::PacketLenDataType *>(_buff);
// 
//         // 判断消息缓冲区的数据长度大于消息长度
//         return len >= *packetLen;
//     }
// 
//     return false;
// }

inline bool IFS_NetBuffer::NeedWrite() const
{
    return GetLength() > 0;
}
/* 网络io接口 */
#ifdef _WIN32
inline IoDataBase * IFS_NetBuffer::MakeRecvIoData()
#else
inline EpollIoData * IFS_NetBuffer::MakeRecvIoData()
#endif
{
//     if(!_sessionId)
//     {
//         g_Log->e<IFS_NetBuffer>(_LOGFMT_("this buffer has no owner, please bind to sesion firstly"));
//         return NULL;
//     }

    // 外部判断
//     const size_t rest = GetRest();
//     if(rest == 0)
//         return NULL;

#ifdef _WIN32
    _ioData._wsaBuff.buf = GetData() + _curPos;
    _ioData._wsaBuff.len = static_cast<ULong>(_bufferSize - _curPos);
#else
    _ioData._data = GetData() + _curPos;
    _ioData._dateLen = _bufferSize - _curPos;
#endif

    _ioData._sessionId = _sessionId;
    _ioData._sock = _socket;

//     // recv可以构成一个缓冲链表
//     if (!_ioData._node)
//         _ioData._node = new BufferQueueNode;

    if(!_ioData._callback)
        _ioData._callback = DelegatePlusFactory::Create(this, &IFS_NetBuffer::_OnRecvSucCallback);

    return &_ioData;
}

#ifdef _WIN32
inline IoDataBase *IFS_NetBuffer::MakeSendIoData()
#else
inline EpollIoData *IFS_NetBuffer::MakeSendIoData()
#endif
{
//     if(!_sessionId)
//     {
//         g_Log->e<IFS_NetBuffer>(_LOGFMT_("this buffer has no owner, please bind to sesion firstly"));
//         return NULL;
//     }

//     auto len = GetLength();
//     if (len == 0)
//     {
//         g_Log->w<IFS_NetBuffer>(_LOGFMT_("this buffer have no data to send sessionId[%llu]"), _sessionId);
//         return NULL;
//     }

#ifdef _WIN32
    _ioData._wsaBuff.buf = GetData();
    _ioData._wsaBuff.len = static_cast<ULong>(_curPos);
#else
    _ioData._data = GetData();
    _ioData._dateLen = _curPos;
#endif
    _ioData._sessionId = _sessionId;
    _ioData._sock = _socket;

    // 只有send的时候才需要node 因为send使用缓冲链表外部不需要关注node是否创建与释放
    if(!_ioData._node)
        _ioData._node = new BufferQueueNode;

    if(!_ioData._callback)
        _ioData._callback = DelegatePlusFactory::Create(this, &IFS_NetBuffer::_OnSendSucCallback);

    return &_ioData;
}

inline FS_String IFS_NetBuffer::ToString() const
{
    FS_String info;

#ifdef _WIN32
    info.AppendFormat("%s\n", IFS_Buffer::ToString().c_str())
        .AppendFormat("_sessionId[%llu], _sock[%d], ioData info: _sessionId[%llu], _sock[%d], _data[0x%p], _datalen[%lld]"
                      , _sessionId, _socket, _ioData._sessionId, _ioData._sock, _ioData._wsaBuff.buf, _ioData._wsaBuff.len);
#else
    info.AppendFormat("%s\n", IFS_Buffer::ToString().c_str())
        .AppendFormat("_sessionId[%llu], _sock[%d], ioData info: _sessionId[%llu], _sock[%d], _data[0x%p], _datalen[%lld]"
                      , _sessionId, _socket, _ioData._sessionId, _ioData._sock, _ioData._data, _ioData._dateLen);
#endif

    return info;
}

inline void IFS_NetBuffer::_OnSendSucCallback(Int64 transferBytes)
{
    // 写入iocp完成多少字节则buffer响应减少多少字节
    auto curPos = GetLength();
    if(curPos < transferBytes)
    {
        g_Log->e<IFS_NetBuffer>(_LOGFMT_("sessionId[%llu] bufferSize<%llu> curPos<%llu> transferBytes<%lld>")
                                , _ioData._sessionId, GetTotalSize(), curPos, transferBytes);
        return;
    }

    if(curPos == transferBytes)
    {// _lastPos=2000 实际发送sendBytes=2000
     // 数据尾部位置清零
        Clear();
    }
    else {
        // _lastPos=2000 实际发送ret=1000
        PopFront(transferBytes);
    }
}

inline void IFS_NetBuffer::_OnRecvSucCallback(Int64 transferBytes)
{
    // 从iocp读入buffer则buffer数据增加相应字节
    if(transferBytes > 0 && GetRest() >= transferBytes)
    {
        _curPos += transferBytes;
        return;
    }

    g_Log->e<IFS_NetBuffer>(_LOGFMT_("_sessionId<%llu> _bufferSize<%llu> _curPos<%llu> transferBytes<%llu>")
                            , _ioData._sessionId, GetTotalSize(), GetLength(), transferBytes);
}

FS_NAMESPACE_END

#endif
