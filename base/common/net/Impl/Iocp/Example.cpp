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
#include "base/common/net/Impl/FS_Iocp.h"
#include "base/common/status/status.h"
#include "base/common/log/Log.h"
#include "base/common/net/Defs/IocpDefs.h"

#include <windows.h>
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
#include<MSWSock.h>
#pragma comment (lib, "MSWSock.lib")
FS_NAMESPACE_BEGIN

#define IO_DATA_BUFF_SIZE 1024
#define CLIENT_QUANTITY 10

// LPFN_ACCEPTEX __g_fnAccept = NULL;
// 
// class IO_Defs
// {
// public:
//     enum IO_TYPE
//     {
//         IO_ACCEPT = 10,
//         IO_RECV,
//         IO_SEND,
//     };
// };
// 
// struct IO_DATA_BASE
// {
//     // 重叠体
//     OVERLAPPED _overlapped;    // 使用重叠体可以关联到iodatabase
//     SOCKET _sock;
//     char _buff[IO_DATA_BUFF_SIZE];
//     Int32 _length;
//     IO_Defs::IO_TYPE _ioType;
// };
// 
// void LoadAcceptEx(SOCKET listenSocket)
// {
//     DWORD dwBytes = 0;
//     GUID guidAcceptEx = WSAID_ACCEPTEX;
//     if(WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
//              &guidAcceptEx, sizeof(guidAcceptEx),
//              &__g_fnAccept, sizeof(__g_fnAccept),
//              &dwBytes, NULL, NULL)!=0)
//     {
//         auto error = WSAGetLastError();
//         printf("load acceptex fail error[%d]", error);
//     }
// }
// 
// void PostAccept(SOCKET sockServer, IO_DATA_BASE *ioData)
// {
//     ioData->_ioType = IO_Defs::IO_ACCEPT;
//     ioData->_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if(!__g_fnAccept(sockServer
//                  , ioData->_sock
//                  , ioData->_buff
//                  , 0
//                  , sizeof(sockaddr_in) + 16
//                  , sizeof(sockaddr_in) + 16
//                  , NULL
//                  , &ioData->_overlapped)) // 可以是自定义的结构体
//     {
//         auto error = WSAGetLastError();
//         if(error != ERROR_IO_PENDING)
//         {
//             printf("AcceptEx failed error[%d]", error);
//             return;
//         }
//     }
// }
// 
// // 投递接收数据
// bool PostRecv(IO_DATA_BASE *ioData)
// {
//     ioData->_ioType = IO_Defs::IO_RECV;
//     WSABUF wsBuff = {};
//     wsBuff.buf = ioData->_buff;
//     wsBuff.len = sizeof(ioData->_buff);
//     DWORD flags = 0;
//     memset(&ioData->_overlapped, 0, sizeof(ioData->_overlapped));
//     if(SOCKET_ERROR == WSARecv(ioData->_sock, &wsBuff, 1, NULL, &flags, &ioData->_overlapped, NULL))
//     {
//         auto error = WSAGetLastError();
//         if(error != ERROR_IO_PENDING)
//         {
//             printf("WSARecv failed error[%d]", error);
//             return false;
//         }
//     }
// 
//     return true;
// }
// 
// bool PostSend(IO_DATA_BASE *ioData)
// {
//     ioData->_ioType = IO_Defs::IO_SEND;
//     WSABUF wsBuff = {};
//     wsBuff.buf = ioData->_buff;
//     wsBuff.len = ioData->_length;
//     DWORD flags = 0;
//     memset(&ioData->_overlapped, 0, sizeof(ioData->_overlapped));
//     if(SOCKET_ERROR == WSASend(ioData->_sock, &wsBuff, 1, NULL, flags, &ioData->_overlapped, NULL))
//     {
//         auto error = WSAGetLastError();
//         if(error != ERROR_IO_PENDING)
//         {
//             printf("WSASend failed error[%d]", error);
//             return false;
//         }
//     }
// 
//     return true;
// }

int Example::Run()
{
    //g_Log->InitModule("IOCP_TEST");
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
    FS_Iocp iocp;
    iocp.Create();
//     auto _completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//     if(!_completionPort)
//     {
//         auto err = GetLastError();
//         printf("CreateIoCompletionPort failed error code<%d>", err);
//         return err;
//     }

    // 关联IOCP 与 ServerSocket
    // completionKey传入的一个数值，完成时会原样传回来; NumberOfConcurrentThreads这个参数在关联完成端口时被忽略
    iocp.Reg(sockServer);
//     auto ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sockServer), _completionPort, (ULONG_PTR)(sockServer), 0); // completekey可以是自定义的结构体指针或者其他数据的指针，便于获取完成状态时候识别
//     if(!ret)
//     {
//         auto err = GetLastError();
//         printf("CreateIoCompletionPort associated failure error code<%d>", err);
//         return err;
//     }

    // 向IOCP投递接受客户端连接的任务
    // sAcceptSocket预先创建的socket，创建socket会消耗系统资源，socket资源有限固需要创建一个socket池，避免资源过度消耗
    // dwReceiveDataLength 有效接受数据长度，若为0表示连接时不必等待客户端发送数据acceptex即完成，若有值表示需要等待客户端发送数据才完成
    // lpdwBytesReceived返回接受数据长度，若不想等待客户端发送数据，这个地方可以填0
    iocp.LoadAcceptEx(sockServer);
    // LoadAcceptEx(sockServer);
    IO_DATA_BASE ioData[CLIENT_QUANTITY] = {};
    for(Int32 i = 0; i < CLIENT_QUANTITY; ++i)
        iocp.PostAccept(sockServer, &ioData[i]);

    Int32 msgCount = 0;
    IO_EVENT ioEvent = {};
    while(true)
    {
        // 等待io完成
        const Int32 st = iocp.WaitForCompletion(ioEvent);
        if(st != StatusDefs::Success)
        {
//             if(st== StatusDefs::IOCP_IODisconnect)
//                 g_Log->sys(_LOGFMT_("客户端断开链接 sockfd=%llu"), ioEvent._ioData->_sock);
            continue;
        }

        // 处理iocp退出
        if(ioEvent._data._code == IocpDefs::IO_QUIT)
        {
            g_Log->sys<Example>(_LOGFMT_("iocp退出 code=%lld"), ioEvent._data._code);
            break;
        }

        // 有连接连入
        if(ioEvent._ioData->_ioType == IocpDefs::IO_ACCEPT)
        {
            g_Log->sys<Example>(_LOGFMT_("新客户端连入 sockfd=%llu"), ioEvent._ioData->_sock);

            // 新客户端关联完成端口
            if(StatusDefs::Success != iocp.Reg(ioEvent._ioData->_sock))
                continue;

            // 投递接收数据
            if(StatusDefs::Success != iocp.PostRecv(ioEvent._ioData))
            {
                g_Log->e<Example>(_LOGFMT_("post recv fail sock[%llu]"), ioEvent._ioData->_sock);
                closesocket(ioEvent._ioData->_sock);
                iocp.PostAccept(sockServer, ioEvent._ioData);
                continue;
            }

            // 继续接受下一个连接 并判断已投递的连接不可大于限制
            // iocp.PostAccept(sockServer, )
        }
        else if(ioEvent._ioData->_ioType == IocpDefs::IO_RECV)
        {
            if(ioEvent._bytesTrans <= 0)
            {
                // 客户端断开
                // g_Log->e<Example>(_LOGFMT_("recv error socket[%llu], bytesTrans[%d]"), ioEvent._ioData->_sock, ioEvent._bytesTrans);
                g_Log->sys<Example>(_LOGFMT_("客户端断开链接 sockfd=%llu bytestrans[%lu]"), ioEvent._ioData->_sock, ioEvent._bytesTrans);
                closesocket(ioEvent._ioData->_sock);
                iocp.PostAccept(sockServer, ioEvent._ioData);
                continue;
            }

            // 打印接收到的数据
            g_Log->sys<Example>(_LOGFMT_("recv data :socket[%llu], bytesTrans[%d] msgCount[%d]")
                       , ioEvent._ioData->_sock, ioEvent._bytesTrans, ++msgCount);

            // 不停的接收数据
            ioEvent._ioData->_wsaBuff.len = ioEvent._bytesTrans;
            auto st = iocp.PostSend(ioEvent._ioData);
            if(st == StatusDefs::IOCP_ClientForciblyClosed)
            {// 远端关闭
                closesocket(ioEvent._ioData->_sock);
                iocp.PostAccept(sockServer, ioEvent._ioData);
            }
        }
        else if(ioEvent._ioData->_ioType == IocpDefs::IO_SEND)
        {
            // 客户端断开处理
            if(ioEvent._bytesTrans <= 0)
            {
                g_Log->sys<Example>(_LOGFMT_("客户端断开链接 sockfd=%llu bytestrans[%lu]"), ioEvent._ioData->_sock, ioEvent._bytesTrans);
                closesocket(ioEvent._ioData->_sock);
                iocp.PostAccept(sockServer, ioEvent._ioData);
                continue;
            }

            // 打印发送的数据
            g_Log->sys<Example>(_LOGFMT_("send data :socket[%llu], bytesTrans[%d] msgCount[%d]")
                       , ioEvent._ioData->_sock, ioEvent._bytesTrans, msgCount);

            // ioEvent._ioData->_wsaBuff.len = 
            auto st = iocp.PostRecv(ioEvent._ioData);
            if(st == StatusDefs::IOCP_ClientForciblyClosed)
            {// 远端关闭
                closesocket(ioEvent._ioData->_sock);
                iocp.PostAccept(sockServer, ioEvent._ioData);
            }
        }
        else
        {
            g_Log->e<Example>(_LOGFMT_("未定义行为 sockefd=%llu"), ioEvent._data._socket);
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
    iocp.Destroy();
    // CloseHandle(_completionPort);

    // 清除windows socket环境
    WSACleanup();
    return 0;
}

FS_NAMESPACE_END
