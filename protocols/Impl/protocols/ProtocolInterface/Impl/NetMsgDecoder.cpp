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
 * @file  : NetMsgDecoder.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/12
 * @brief :
 */
#include "stdafx.h"
#include "protocols/ProtocolInterface/Impl/NetMsgDecoder.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_ANCESTOR_IMPL(NetMsgDecoder, __DEF_OBJ_POOL_OBJ_NUM__);

bool NetMsgDecoder::Read(Byte8 *data, UInt64 readLen)
{
    if(_curPos + readLen > _len)
    {
        g_Log->w<NetMsgDecoder>(_LOGFMT_("cmd[%u] curPos[%llu], readLen[%llu], _len[%llu] read fail data[%p]")
                                , _cmd, _curPos, readLen, _len, data);
        return false;
    }

    ::memcpy(data, _buffer + _curPos, readLen);

    _curPos += readLen;
    return true;
}

FS_String NetMsgDecoder::ToString() const
{
    // cmd
    // len
    // 
    FS_String info;
    FS_String raw;
    StringUtil::ToHexString(_buffer, _len, raw);
    info.AppendFormat("cmd[%u], len[%u], package raw:\n%s\n", _cmd, _len, raw.c_str());
    return info;
}

FS_NAMESPACE_END