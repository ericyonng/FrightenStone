/**
* @file TcpClient.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

#include <base/exportbase.h>

class TcpClient
{
public:
    TcpClient();
    virtual ~TcpClient();

public:
    #pragma region 
    Int32 InitSocket(bool blockSocket = false);
    Int32 Connect(const char *ip, UInt16 port) const;
    Int32 RecvData();
    Int32 SendData(PacketHeader* header) const;
    void Close();
    #pragma endregion

    #pragma region 
    bool OnRun();
    bool IsRun() const;
    virtual void OnNetMsg(PacketHeader *header);
    #pragma endregion

private:
    #pragma region  
    char _msgBuf[SOCKET_CACHE_SIZE * 10] = {0};
    char _recv[SOCKET_CACHE_SIZE] = {0};
    int _lastPos = 0;
    #pragma endregion

    MYSOCKET _sock = MYINVALID_SOCKET;
};
