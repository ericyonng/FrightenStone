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
 * @file  : NetMsgCoderImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_ProtocolInterface_Impl_NetMsgCoder_H__

#pragma once

FS_NAMESPACE_BEGIN

inline NetMsgCoder::NetMsgCoder()
    :_buffer(NULL)
    ,_curHeaderPos(0)
    ,_wrLen(0)
{
}

inline void NetMsgCoder::Release()
{
    delete this;
}

inline void NetMsgCoder::BindBuffer(Byte8 *msgBuffer)
{
    _buffer = msgBuffer;
    _wrLen = 0;
    _curHeaderPos = 0;
}

inline void NetMsgCoder::Start()
{
    _wrLen += NetMsgHeaderFmtType::_msgPacketLenSize;
}

inline void NetMsgCoder::SetCmd(UInt32 cmd)
{
    Write(reinterpret_cast<const Byte8 *>(&cmd), NetMsgHeaderFmtType::_msgCmdSize);
}

template<typename DataType>
inline void NetMsgCoder::Write(DataType &data)
{
    const UInt64 addSize = sizeof(data);
    ::memcpy(_buffer + _curHeaderPos + _wrLen, &data, addSize);
    _wrLen += addSize;
}

inline void NetMsgCoder::Write(const Byte8 *data, UInt64 dataLen)
{
    ::memcpy(_buffer + _curHeaderPos + _wrLen, data, dataLen);
    _wrLen += dataLen;
}

inline UInt64 NetMsgCoder::GetTotalWriteBytes() const
{
    return _curHeaderPos + _wrLen;
}

FS_NAMESPACE_END

#endif
