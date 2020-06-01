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
 * @file  : NetMsgCoderImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#ifdef __Protocols_Impl_ProtocolInterface_Impl_NetMsgCoder_H__

#pragma once

FS_NAMESPACE_BEGIN

inline NetMsgCoder::NetMsgCoder()
    :_buffer(NULL)
    , _curHeaderPos(0)
    , _wrLen(0)
    , _bufferSize(0)
    , _isBufferNotEnough(false)
{
}

inline void NetMsgCoder::Release()
{
    delete this;
}

inline void NetMsgCoder::BindBuffer(Byte8 *msgBuffer, UInt64 bufferSize)
{
    _buffer = msgBuffer;
    _bufferSize = bufferSize;
    _wrLen = 0;
    _curHeaderPos = 0;
    _isBufferNotEnough = false;
}

inline void NetMsgCoder::BindBuffer(FS_String *msgBuffer)
{
    _buffer = msgBuffer;
    _bufferSize = 0;
    _wrLen = 0;
    _curHeaderPos = 0;
    _isBufferNotEnough = false;
}

template<>
inline bool NetMsgCoder::Start<Byte8>()
{
    if (!_CheckSizeAndMaskNotEnough(NetMsgHeaderFmtType::_msgPacketLenSize))
        return false;

    _wrLen += NetMsgHeaderFmtType::_msgPacketLenSize;
    return true;
}

template<>
inline bool NetMsgCoder::Start<FS_String>()
{
    _wrLen += NetMsgHeaderFmtType::_msgPacketLenSize;
    auto buffer = reinterpret_cast<FS_String *>(_buffer);
    buffer->AppendBitData(reinterpret_cast<const char *>(_wrLen), NetMsgHeaderFmtType::_msgPacketLenSize);
    return true;
}

template<>
inline bool NetMsgCoder::SetCmd<Byte8>(UInt32 cmd)
{
    return WriteByte(reinterpret_cast<const Byte8 *>(&cmd), NetMsgHeaderFmtType::_msgCmdSize);
}

template<>
inline bool NetMsgCoder::SetCmd<FS_String>(UInt32 cmd)
{
    WriteToString(reinterpret_cast<const Byte8 *>(&cmd), NetMsgHeaderFmtType::_msgCmdSize);
    return true;
}

template<typename DataType>
inline bool NetMsgCoder::WriteByte(DataType &data)
{
    const UInt64 addSize = sizeof(data);
    if (!_CheckSizeAndMaskNotEnough(addSize))
        return false;

    ::memcpy((char *)(_buffer) + _curHeaderPos + _wrLen, &data, addSize);
    _wrLen += addSize;
    return true;
}

inline bool NetMsgCoder::WriteByte(const Byte8 *data, UInt64 dataLen)
{
    if (!_CheckSizeAndMaskNotEnough(dataLen))
        return false;

    ::memcpy((char *)(_buffer) + _curHeaderPos + _wrLen, data, dataLen);
    _wrLen += dataLen;
    return true;
}

template<typename DataType>
inline void NetMsgCoder::WriteToString(DataType &data)
{
    const UInt64 addSize = sizeof(data);
    auto buffer = reinterpret_cast<FS_String *>(_buffer);
    buffer->AppendBitData(reinterpret_cast<char *>(&data), addSize);
    _wrLen += addSize;
}

inline void NetMsgCoder::WriteToString(const Byte8 *data, UInt64 dataLen)
{
    auto buffer = reinterpret_cast<FS_String *>(_buffer);
    buffer->AppendBitData(data, dataLen);
    _wrLen += dataLen;
}

template<>
inline void NetMsgCoder::Finish<Byte8>()
{
    // 1.起始位置写入长度
    *(reinterpret_cast<NetMsgHeaderFmtType::PacketLenDataType *>((char *)(_buffer)+_curHeaderPos)) =
        static_cast<NetMsgHeaderFmtType::PacketLenDataType>(_wrLen);
    // 2.记入当前偏移位置
    _curHeaderPos += _wrLen;
    // 3.更新剩余空间
    // 4.写入的数据长度归零
    _wrLen = 0;
}

template<>
inline void NetMsgCoder::Finish<FS_String>()
{
    // 1.起始位置写入长度
    auto buffer = reinterpret_cast<FS_String *>(_buffer);
    auto &raw = buffer->GetRaw();
    for (UInt32 i = 0; i < NetMsgHeaderFmtType::_msgPacketLenSize; ++i)
        raw.at(_curHeaderPos + i) = *(reinterpret_cast<char *>(&_wrLen) + i);
    // 2.记入当前偏移位置
    _curHeaderPos += _wrLen;
    // 3.更新剩余空间
    // 4.写入的数据长度归零
    _wrLen = 0;
}

inline bool NetMsgCoder::IsBufferNotEnough() const
{
    return _isBufferNotEnough;
}

inline UInt64 NetMsgCoder::GetTotalWriteBytes() const
{
    return _curHeaderPos + _wrLen;
}

inline bool NetMsgCoder::_CheckSizeAndMaskNotEnough(size_t appendSize)
{
    if (_curHeaderPos + _wrLen + appendSize > _bufferSize)
    {
        _isBufferNotEnough = true;
        return false;
    }

    return true;
}

inline bool NetMsgCoder::_CheckSizeNoMaskNotEnough(size_t appendSize) const
{
    if (_curHeaderPos + _wrLen + appendSize > _bufferSize)
        return false;

    return true;
}

FS_NAMESPACE_END

#endif
