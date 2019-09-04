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
 * @file  : FS_Client.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_Client.h"
#include "base/common/socket/socket.h"
#include "base/common/net/Defs/FS_NetBuffer.h"
#include "base/common/log/Log.h"
#include "base/common/net/Defs/FS_NetDefs.h"
#include "base/common/net/Defs/FS_NetBufferArray.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_IMPL(FS_Client, _objPoolHelper, __DEF_OBJ_POOL_OBJ_NUM__)

FS_Client::FS_Client(UInt64 clientId
                            , SOCKET sockfd
                            , Int32 sendSize
                            , Int32 recvSize)
    : _id(clientId)
    , _sockfd(sockfd)
    , _recvBuff(new  FS_Packet(clientId, recvSize))
{
    UpdateHeartBeatExpiredTime();
}

#ifdef FS_USE_IOCP
IO_DATA_BASE *FS_Client::MakeRecvIoData()
{
    if(_isPostRecv)
        return NULL;

    _isPostRecv = true;
    return _recvBuff->MakeRecvIoData();
}

IO_DATA_BASE *FS_Client::MakeSendIoData()
{
    if(_isPostSend || _sendBuff.empty())
        return NULL;

    _isPostSend = true;
    return _sendBuff.front()->MakeSendIoData();
}

void FS_Client::ResetPostRecv()
{
    if(!_isPostRecv)
        g_Log->e<FS_Client>(_LOGFMT_("recv from _isPostRecv is false"));

    _isPostRecv = false;
}

void FS_Client::ResetPostSend()
{
    if(!_isPostSend)
        g_Log->e<FS_Client>(_LOGFMT_("send2iocp _isPostSend is false"));

    _isPostSend = false;
}

#endif

void FS_Client::Close()
{
    if(INVALID_SOCKET != _sockfd)
    {
        // CELLLog_Info("CELLClient::destory[sId=%d id=%d socket=%d]", serverId, id, (int)_sockfd);
        SocketUtil::DestroySocket(_sockfd);
        _sockfd = INVALID_SOCKET;
    }
}

FS_NAMESPACE_END
