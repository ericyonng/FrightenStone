/**
* @file TcpServer.h
* @author Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
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
