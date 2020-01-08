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
 * @file  : User.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#include "stdafx.h"
#include "TestInst/TestServer/User/User.h"

User::User(UInt64 sessionId, UInt64 userId, fs::IFS_MsgDispatcher *dispatcher)
    :_sessionId(sessionId)
    , _userId(userId)
    , _recvMsgId(1)
    , _sendMsgId(1)
    , _dispatcher(dispatcher)
{

}

User::~User()
{

}

UInt64 User::GetSessionId() const
{
    return _sessionId;
}

UInt64 User::GetUseId() const
{
    return _userId;
}

void User::Close()
{
    // TODO:抛一个断开session的消息到transfer 通过dispatcher
    _dispatcher->CloseSession(_sessionId);
}

// NetMsg_DataHeader内部会拷贝到缓冲区
void User::SendData(UInt64 sessionId, fs::NetMsg_DataHeader *msgData)
{
    _dispatcher->SendData(sessionId, msgData);
}

void User::OnDisconnect()
{
    g_Log->net<User>(" session id[%llu] user disconnect", _sessionId);
}
