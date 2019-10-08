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
 * @file  : FS_Session.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_Session.h"
#include "base/common/net/Impl/FS_Addr.h"
#include "base/common/net/Impl/FS_SessionMgr.h"
#include "base/common/net/Defs/FS_Packet.h"

#include "base/common/assist/utils/Impl/STLUtil.h"

OBJ_POOL_CREATE_DEF_IMPL(fs::FS_Session, __DEF_OBJ_POOL_OBJ_NUM__)

FS_NAMESPACE_BEGIN

FS_Session::FS_Session(UInt64 sessionId, SOCKET sock, FS_SessionMgr *sessionMgr)
    :_sessionId(sessionId)
    ,_sessionMgr(sessionMgr)
    ,_addr(new FS_Addr)
    ,_sock(sock)
{

}

FS_Session::~FS_Session()
{
    STLUtil::DelListContainer(_recvPackets);
    STLUtil::DelListContainer(_toSend);
}


bool FS_Session::HasMsgToRead() const
{
    return false;
}

bool FS_Session::HasMsgToSend() const
{
    return false;
}

void FS_Session::OnDestroy()
{

}

void FS_Session::OnConnect()
{

}

void FS_Session::OnHeartBeatTimeOut()
{

}

FS_NAMESPACE_END

