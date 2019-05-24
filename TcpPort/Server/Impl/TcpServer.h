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
 * @file  : TcpServer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#pragma once

#include <base/exportbase.h>
#include <vector>

class ClientSocket;

class TcpServer
{
public:
    TcpServer();
    virtual ~TcpServer();

public:
    #pragma region 
    Int32 InitSocket(bool blockSocket = false);
    Int32 Bind(const char* ip, unsigned short port) const;
    Int32 Listen(Int32 backLog) const;
    MYSOCKET Accept();
    void Close();
    void CloseClients();
    Int32 RecvData(ClientSocket *client);
    Int32 SendData(MYSOCKET socket, PacketHeader *header);
    void SendToAll(PacketHeader* header);
    #pragma endregion

    #pragma region 
    bool OnRun();
    bool IsRun() const;
    virtual void OnNetMsg(MYSOCKET socket, PacketHeader *header);
    #pragma endregion

private:
    #pragma region  
    MYSOCKET _socket = MYINVALID_SOCKET;
    std::vector<ClientSocket *> _clients;
    #pragma endregion

    char _recv[SOCKET_CACHE_SIZE] = {0};
    Int32 _count = 0;
};
