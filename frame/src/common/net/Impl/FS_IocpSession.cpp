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
 * @file  : FS_IocpSession.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/14
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_IocpSession.h"
#include "FrightenStone/common/net/Defs/FS_IocpBuffer.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN


FS_IocpSession::~FS_IocpSession()
{
    if(!CanDestroy())
        g_Log->e<FS_IocpSession>(_LOGFMT_("has io uncompleted please check"));

}

NetMsg_DataHeader *FS_IocpSession::FrontRecvMsg()
{
    return _recvBuffer->CastToData<NetMsg_DataHeader>();
}

void FS_IocpSession::PopFrontRecvMsg()
{
    if(HasMsgToRead())
        _recvBuffer->PopFront(FrontRecvMsg()->_packetLength);
}

IoDataBase *FS_IocpSession::MakeRecvIoData()
{
    if(!CanPost())
        return NULL;

    if(_isPostRecv)
        return NULL;

    _isPostRecv = true;
    return _recvBuffer->CastToBuffer<FS_IocpBuffer>()->MakeRecvIoData();
}

IoDataBase *FS_IocpSession::MakeSendIoData()
{
    if(!CanPost())
        return NULL;

    if(_isPostSend || _toSend.empty())
        return NULL;

    _isPostSend = true;
    auto buffer = _toSend.front()->CastToBuffer<FS_IocpBuffer>();
    auto ioData = buffer->MakeSendIoData();
    if(!ioData)
        return NULL;
    
    if(!ioData->_node)
        ioData->_node = new BufferQueueNode;

    auto node = ioData->_node;
    node->_isPost = true;
    node->_iterNode = _toSend.begin();
    return ioData;
}

FS_NAMESPACE_END

#endif
