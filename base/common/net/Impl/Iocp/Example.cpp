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
 * @file  : Example.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/11
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/Iocp/Example.h"

#include <windows.h>
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
#include<MSWSock.h>
#pragma comment (lib, "MSWSock.lib")
FS_NAMESPACE_BEGIN

#define IO_DATA_BUFF_SIZE 1024
#define CLIENT_QUANTITY 10

class IO_Defs
{
public:
    enum IO_TYPE
    {
        IO_ACCEPT = 10,
        IO_RECV,
        IO_SEND,
    };
};

struct IO_DATA_BASE
{
    // 重叠体
    OVERLAPPED _overlapped;    // 使用重叠体可以关联到iodatabase
    SOCKET _sock;
    char _buff[IO_DATA_BUFF_SIZE];
    Int32 _length;
    IO_Defs::IO_TYPE _ioType;
};

void PostAccept(SOCKET sockServer, IO_DATA_BASE *ioData)
{
    ioData->_ioType = IO_Defs::IO_ACCEPT;
    ioData->_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(!AcceptEx(sockServer
                 , ioData->_sock
                 , ioData->_buff
                 , 0
                 , sizeof(sockaddr_in) + 16
                 , sizeof(sockaddr_in) + 16
                 , NULL
                 , &ioData->_overlapped)) // 可以是自定义的结构体
    {
        auto error = WSAGetLastError();
        if(error != ERROR_IO_PENDING)
        {
            printf("AcceptEx failed error[%d]", error);
            return;
        }
    }
}

// 投递接收数据
bool PostRecv(IO_DATA_BASE *ioData)
{
    ioData->_ioType = IO_Defs::IO_RECV;
    WSABUF wsBuff = {};
    wsBuff.buf = ioData->_buff;
    wsBuff.len = sizeof(ioData->_buff);
    DWORD flags = 0;
    memset(&ioData->_overlapped, 0, sizeof(ioData->_overlapped));
    if(SOCKET_ERROR == WSARecv(ioData->_sock, &wsBuff, 1, NULL, &flags, &ioData->_overlapped, NULL))
    {
        auto error = WSAGetLastError();
        if(error != ERROR_IO_PENDING)
        {
            printf("WSARecv failed error[%d]", error);
            return false;
        }
    }

    return true;
}

bool PostSend(IO_DATA_BASE *ioData)
{
    ioData->_ioType = IO_Defs::IO_SEND;
    WSABUF wsBuff = {};
    wsBuff.buf = ioData->_buff;
    wsBuff.len = ioData->_length;
    DWORD flags = 0;
    memset(&ioData->_overlapped, 0, sizeof(ioData->_overlapped));
    if(SOCKET_ERROR == WSASend(ioData->_sock, &wsBuff, 1, NULL, flags, &ioData->_overlapped, NULL))
    {
        auto error = WSAGetLastError();
        if(error != ERROR_IO_PENDING)
        {
            printf("WSASend failed error[%d]", error);
            return false;
        }
    }

    return true;
}

int Example::Run()
{
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);

    // 1. 创建监听socket
    SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);   // 默认内部设置了重叠标志

    // 2.绑定端口
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4567);    // 转成网络字节序
    _sin.sin_addr.s_addr = INADDR_ANY;  // 任意ip
    if(SOCKET_ERROR == bind(sockServer, reinterpret_cast<sockaddr *>(&_sin), sizeof(_sin)))
    {
        const auto err = GetLastError();
        printf("错误，绑定网络断开失败error<%d>...\n", err);
        return err;
    }
    else
    {
        printf("监听网络端口成功\n");
    }

    // 3.监听端口
    if(SOCKET_ERROR == listen(sockServer, 64))
    {// 最大64个等待监听的队列，
        printf("错误,监听网络端口失败...");
    }
    else
    {
        printf("监听网络端口成功...\n");
    }

    // 创建完成端口IOCP NumberOfConcurrentThreads=0表示默认cpu核数
    auto _completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if(!_completionPort)
    {
        auto err = GetLastError();
        printf("CreateIoCompletionPort failed error code<%d>", err);
        return err;
    }

    // 关联IOCP 与 ServerSocket
    // completionKey传入的一个数值，完成时会原样传回来; NumberOfConcurrentThreads这个参数在关联完成端口时被忽略
    auto ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sockServer), _completionPort, (ULONG_PTR)(sockServer), 0); // completekey可以是自定义的结构体指针或者其他数据的指针，便于获取完成状态时候识别
    if(!ret)
    {
        auto err = GetLastError();
        printf("CreateIoCompletionPort associated failure error code<%d>", err);
        return err;
    }

    // 向IOCP投递接受客户端连接的任务
    // sAcceptSocket预先创建的socket，创建socket会消耗系统资源，socket资源有限固需要创建一个socket池，避免资源过度消耗
    // dwReceiveDataLength 有效接受数据长度，若为0表示连接时不必等待客户端发送数据acceptex即完成，若有值表示需要等待客户端发送数据才完成
    // lpdwBytesReceived返回接受数据长度，若不想等待客户端发送数据，这个地方可以填0
    IO_DATA_BASE ioData[CLIENT_QUANTITY] = {};
    for(Int32 i = 0; i < CLIENT_QUANTITY; ++i)
        PostAccept(sockServer, &ioData[i]);

    Int32 msgCount = 0;
    while(true)
    {
        // 获取完成端口状态
        DWORD bytesTrans = 0;
        SOCKET sock = INVALID_SOCKET;
        IO_DATA_BASE *ioDataPtr = NULL;
        // 关键在于 completekey(关联iocp端口时候传入的自定义完成键)
        // 以及重叠结构ioDataPtr 用于获取数据
        if(FALSE == GetQueuedCompletionStatus(_completionPort, &bytesTrans, (PULONG_PTR)&sock, (LPOVERLAPPED *)&ioDataPtr, INFINITE))
        {
            const Int32 error = GetLastError();
            printf("GetQueuedCompletionStatus failed with error %d\n", error);
            if(ERROR_NETNAME_DELETED == error)
            {
                printf("客户端断开，关闭 sockfd=%d\n", static_cast<Int32>(ioDataPtr->_sock));
                closesocket(ioDataPtr->_sock);
                continue;
            }
            break;
        }

        // 有连接连入
        if(ioDataPtr->_ioType == IO_Defs::IO_ACCEPT)
        {
            printf("新客户端连入 sockfd=%d\n", static_cast<Int32>(ioDataPtr->_sock));

            // clientsocket关联完成端口
            auto associateRet = CreateIoCompletionPort(reinterpret_cast<HANDLE>(ioDataPtr->_sock), _completionPort, (ULONG_PTR)(ioDataPtr->_sock), 0);
            if(!associateRet)
            {
                auto err = GetLastError();
                printf("CreateIoCompletionPort associated clientsock[%d] failure error code<%d>", static_cast<Int32>(ioDataPtr->_sock), err);
                closesocket(ioDataPtr->_sock);
                continue;
            }

            // 投递接收数据
            if(!PostRecv(ioDataPtr))
            {
                printf("post recv fail sock[%d]\n", static_cast<Int32>(ioDataPtr->_sock));
                closesocket(ioDataPtr->_sock);
                continue;
            }

            // 投递接收数据
//             for(Int32 i=0;i<10;++i)
//                 if(!PostRecv(ioDataPtr))
//                 {
//                     printf("post recv fail sock[%d]\n", static_cast<Int32>(ioDataPtr->_sock));
//                     closesocket(ioDataPtr->_sock);
//                     continue;
//                 }
        }
        else if(ioDataPtr->_ioType == IO_Defs::IO_RECV)
        {
            if(bytesTrans <= 0)
            {
                printf("recv error socket[%d], bytesTrans[%d]\n"
                       , static_cast<Int32>(ioDataPtr->_sock), bytesTrans);
                closesocket(ioDataPtr->_sock);
                continue;
            }

            // 打印接收到的数据
            printf("recv data :socket[%d], bytesTrans[%d] msgCount[%d]\n"
                   , static_cast<Int32>(ioDataPtr->_sock), bytesTrans, ++msgCount);

            // 不停的接收数据
            ioDataPtr->_length = bytesTrans;
            PostSend(ioDataPtr);
        }
        else if(ioDataPtr->_ioType == IO_Defs::IO_SEND)
        {
            // 客户端断开处理
            if(bytesTrans <= 0)
            {
                printf("send error socket[%d], bytesTrans[%d]\n"
                       , static_cast<Int32>(ioDataPtr->_sock), bytesTrans);
                closesocket(ioDataPtr->_sock);
                continue;
            }

            // 打印发送的数据
            printf("send data :socket[%d], bytesTrans[%d] msgCount[%d]\n"
                   , static_cast<Int32>(ioDataPtr->_sock), bytesTrans, msgCount);

            PostRecv(ioDataPtr);
        }
        else
        {
            printf("未定义行为 sockefd=%d", static_cast<Int32>(sock));
        }
        // 检查是否有事件发生，和selet,epoll_wait类似
        // 接受连接 完成
        // 接受数据 完成 completion
        // 发送数据 完成
        // 向IOCP 投递接收数据任务
    }

    // ------------ IOCP end ------------ //
    // 关闭clientsocket
    for(Int32 i = 0; i < CLIENT_QUANTITY; ++i)
        closesocket(ioData[i]._sock);
    // 关闭serversocket
    closesocket(sockServer);
    // 关闭完成端口
    CloseHandle(_completionPort);

    // 清除windows socket环境
    WSACleanup();
    return 0;
}

FS_NAMESPACE_END
