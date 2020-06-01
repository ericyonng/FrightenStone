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
 * @file  : NetDataHeader.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/11
 * @brief :
 */
#include "stdafx.h"
#include "protocols/ProtocolInterface/Impl/NetDataHeader.h"
#include "protocols/ProtocolInterface/Defs/NetMsgHeaderFmtType.h"
#include "protocols/ProtocolInterface/Impl/NetMsgCoder.h"
#include "protocols/ProtocolInterface/Impl/NetMsgDecoder.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(NetMsg_DataHeader, __DEF_OBJ_POOL_OBJ_NUM__);

NetMsg_DataHeader::~NetMsg_DataHeader()
{
    FS_Release(_coder);
    FS_Release(_decoder);
}

// 序列化将数据写入缓冲区 有越界风险,需要调整coder，应该序列化到string对象或者序列化接口可以支持循环序列化，
Int64 NetMsg_DataHeader::SerializeTo(char *buffer, UInt64 bufferSize)
{
    if(!_coder)
        _coder = new NetMsgCoder;

    // 1.绑定缓存并初始化
    _coder->BindBuffer(buffer, bufferSize);
    if (!_coder->Start<Byte8>())
        return -1;

    if (!_coder->SetCmd<Byte8>(_cmd))
        return -1;

    // 2.包体数据
    if (!SerializeToByteBuffer(_coder))
        return -1;

    // 3.缓冲空间是否不足
    if (_coder->IsBufferNotEnough())
        return -1;

    // 4.结束
    _coder->Finish<Byte8>();

    return _coder->GetTotalWriteBytes();
}

Int64 NetMsg_DataHeader::SerializeTo(FS_String &buffer)
{
    if (!_coder)
        _coder = new NetMsgCoder;

    // 1.绑定缓存并初始化
    _coder->BindBuffer(&buffer);
    _coder->Start<FS_String>();
    _coder->SetCmd<FS_String>(_cmd);

    // 2.包体数据
    SerializeToStringBuffer(_coder);

    // 3.结束
    _coder->Finish<FS_String>();

    return _coder->GetTotalWriteBytes();
}

// 从网络缓冲区反序列化得到消息
bool NetMsg_DataHeader::DeserializeFrom(const char *buffer)
{
    if(!_decoder)
        _decoder = new NetMsgDecoder;

    // 1.预解码
    _decoder->Decode(buffer);

    // 2.实际包体数据读取
    if(!DeserializeFrom(_decoder))
    {
        g_Log->w<NetMsg_DataHeader>(_LOGFMT_("cmd[%u] len[%u] DeserializeFrom fail")
                                    , _decoder->GetCmd(), _decoder->GetMsgLen());
    }

    return true;
}

bool NetMsg_DataHeader::IsCoderBufferNotEnough() const
{
    return _coder->IsBufferNotEnough();
}

Int64 NetMsg_DataHeader::GetCoderBytesWriten() const
{
    return _coder->GetTotalWriteBytes();
}
FS_NAMESPACE_END
