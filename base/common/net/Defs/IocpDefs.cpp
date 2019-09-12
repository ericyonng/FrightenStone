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
 * @file  : IocpDefs.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/18
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Defs/FS_Packet.h"
#include "base/common/net/Defs/IocpDefs.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_IMPL(IO_DATA_BASE, _objPoolHelper, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_IMPL(IO_EVENT, _objPoolHelper, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DEF_IMPL(PacketQueueNode, __DEF_OBJ_POOL_OBJ_NUM__);

PacketQueueNode::~PacketQueueNode()
{
    Fs_SafeFree(_packet);
}

PacketQueueNode::PacketQueueNode()
   : _packet(NULL)
   , _isPost(false)
{
}

IO_DATA_BASE::~IO_DATA_BASE()
{
    Fs_SafeFree(_completedCallback);
}

IO_EVENT::IO_EVENT()
    :_data{NULL}
    , _ioData(NULL)
    , _bytesTrans(0)
{
}
FS_NAMESPACE_END
