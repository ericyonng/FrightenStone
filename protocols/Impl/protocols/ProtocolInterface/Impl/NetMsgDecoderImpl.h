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
 * @file  : NetMsgDecoderImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#ifdef __Protocols_Impl_ProtocolInterface_Impl_NetMsgDecoder_H__

#pragma once

FS_NAMESPACE_BEGIN
inline NetMsgDecoder::NetMsgDecoder()
    :_cmd(0)
    ,_len(0)
    ,_buffer(NULL)
    ,_curPos(0)
{
}

inline void NetMsgDecoder::Release()
{
    delete this;
}

inline bool NetMsgDecoder::Decode(const char *msgBuffer)
{
    // 1.记录缓冲
    _buffer = msgBuffer;

    // 2.解包长度
    ::memcpy(&_len, msgBuffer, NetMsgHeaderFmtType::_msgPacketLenSize);
    // 空间偏移NetMsgHeaderFmtType::LenBytes
    msgBuffer += NetMsgHeaderFmtType::_msgPacketLenSize;

    // 3.解协议号
    ::memcpy(&_cmd, msgBuffer, NetMsgHeaderFmtType::_msgCmdSize);

    // 4.初始化位置从消息和长度后面开始
    _curPos = NetMsgHeaderFmtType::_msgHeaderSize;
    return true;
}

template<typename DataType>
inline bool NetMsgDecoder::Read(DataType &dataOut)
{
    const UInt64 dataSize = sizeof(dataOut);
    return Read(reinterpret_cast<Byte8 *>(&dataOut), dataSize);
}

inline UInt64 NetMsgDecoder::GetTotalReadBytes() const
{
    return _curPos;
}

inline UInt32 NetMsgDecoder::GetMsgLen() const
{
    return _len;
}

inline UInt32 NetMsgDecoder::GetCmd() const
{
    return _cmd;
}

inline const Byte8 *NetMsgDecoder::GetMsgBuffer() const
{
    return _buffer;
}

FS_NAMESPACE_END

#endif
