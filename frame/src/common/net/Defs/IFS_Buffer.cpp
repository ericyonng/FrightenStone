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
 * @file  : IFS_Buffer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/27
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/IFS_Buffer.h"
#include "FrightenStone/common/assist/utils/utils.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"

#define __USE_FS_3RD_JSON___
#include "3rd/3rd.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(IFS_Buffer, __DEF_OBJ_POOL_OBJ_NUM__);

bool IFS_Buffer::PushBack(NetMsg_DataHeader *netMsg)
{
    // 1.判断剩余空间是否足够 外部判断canpush
    size_t rest = _bufferSize - _curPos;
    //size_t len = static_cast<size_t>(netMsg->_packetLength);
//     if(rest < len)
//     {
//         g_Log->e<IFS_Buffer>(_LOGFMT_("rest buffer size[%llu] not enough to match data len[%llu]"), rest, len);
//         return false;
//     }

    // 2.拷贝数据
    Int64 len = netMsg->SerializeTo(_buff + _curPos);
    if(len < 0)
    {
        g_Log->e<IFS_Buffer>(_LOGFMT_("netMsg serialize to buffer fail, cmd[%u], len[%llu]"), netMsg->_cmd, len);
        return false;
    }

    // 3.变更当前位置
    _curPos += len;
    return true;
}

FS_String IFS_Buffer::ToString() const
{
    FS_String info;
    info.AppendFormat("sessionId:%llu,\n", _sessionId)
        .AppendFormat("sock:%d,\n", _socket)
//        .AppendFormat("allctorInfo:");
//    _alloctor->MemInfoToString(info);
//    info.AppendFormat("\n")
        .AppendFormat("bufferSize:%lld\n", _bufferSize)
        .AppendFormat("buff address:0x%p\n", _buff)
        .AppendFormat("curPos:%lld\n", _curPos);

    return info;
}

FS_NAMESPACE_END
