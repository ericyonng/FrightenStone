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
 * @file  : NetMsgCoder.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/12
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/ProtocolInterface/Impl/NetMsgCoder.h"
#include "FrightenStone/common/net/ProtocolInterface/Impl/NetDataHeader.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(NetMsgCoder, __DEF_OBJ_POOL_OBJ_NUM__);

void NetMsgCoder::Finish()
{
    // 1.起始位置写入长度
    *(reinterpret_cast<NetMsgHeaderFmtType::PacketLenDataType *>(_buffer + _curHeaderPos)) =
        static_cast<NetMsgHeaderFmtType::PacketLenDataType>(_wrLen);
    // 2.记入当前偏移位置
    _curHeaderPos += _wrLen;
    // 3.更新剩余空间
    // 4.写入的数据长度归零
    _wrLen = 0;
}
FS_NAMESPACE_END
