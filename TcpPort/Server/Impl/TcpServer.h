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
    #pragma region 套接字操作
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

    #pragma region 消息处理
    bool OnRun();
    bool IsRun() const;
    virtual void OnNetMsg(MYSOCKET socket, PacketHeader *header);
    #pragma endregion

private:
    #pragma region 套接字与客户端消息 
    MYSOCKET _socket = MYINVALID_SOCKET;
    std::vector<ClientSocket *> _clients;
    #pragma endregion

    // 处理网络消息
    char _recv[SOCKET_CACHE_SIZE] = {0};
    Int32 _count = 0;
};
