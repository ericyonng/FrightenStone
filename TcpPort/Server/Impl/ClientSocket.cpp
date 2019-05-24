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
 * @file  : ClientSocket.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "pch.h"
#include "TcpPort/Server/Impl/ClientSocket.h"

ClientSocket::ClientSocket(MYSOCKET socket /*= MYINVALID_SOCKET*/, bool blockSocket)
    :_socket(socket)
{
    if(blockSocket)
        fs::SocketUtil::SetBlock(_socket);
    else
        fs::SocketUtil::SetNoBlock(_socket);
}

ClientSocket::~ClientSocket()
{
    if(_socket != MYINVALID_SOCKET)
    {
        #ifdef _WIN32
        closesocket(_socket);
        #else
        close(_socket);
        #endif
        _socket = MYINVALID_SOCKET;
    }
}
