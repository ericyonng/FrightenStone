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
 * @file  : FS_NetDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/03
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Defs_FS_NetDefs_H__
#define __Base_Common_Net_Defs_FS_NetDefs_H__
#pragma once


 // SOCKET
#ifdef _WIN32
#undef FD_SETSIZE
#define FD_SETSIZE      100000
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#else
#ifdef __APPLE__
#define _DARWIN_UNLIMITED_SELECT
#endif // !__APPLE__
#include<unistd.h> //uni std
#include<arpa/inet.h>
#include<string.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/tcp.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

//缓冲区最小单元大小
#ifndef RECV_BUFF_SZIE
#define RECV_BUFF_SZIE 16384    // 16K
#define SEND_BUFF_SZIE 16384    // 16K
#define FS_BUFF_SIZE_DEF 16384  // 16K
#endif // !RECV_BUFF_SZIE


// 客户端心跳检测死亡计时时间(单位ms)
// 在间隔指定时间后才允许发送(单位ms)
#define CLIENT_SEND_BUFF_TIME 200
// connector的缓冲大小
#define IOCP_CONNECTOR_BUFFER 1024
// 统计服务器负载的时间间隔
#define IOCP_STATISTIC_INTERVAL     1000    // 时间间隔单位ms
#define SVR_CONFIG_PATH_NAME    "./ServerCfg.ini"   // 服务器配置

#pragma region Listener default config
#define SVR_CFG_LISTENER_SEG                "Listener"

#define SVR_CFG_LISTENER_IP_KEY             "ip"
#define SVR_CFG_LISTENER_IP                 "127.0.0.1"
#define SVR_CFG_LISTENER_PORT_KEY           "port"
#define SVR_CFG_LISTENER_PORT               "4567"
#define SVR_CFG_LISTENER_CLN_LIMIT_KEY      "MaxConnectQuantity"        // 最大连接数
#define SVR_CFG_LISTENER_CLN_LIMIT          "100064"                    // 默认的最大连接数

#pragma endregion

#pragma region Transfer default config
#define SVR_CFG_TRANSFER_SEG                            "Transfer"                      // 数据传输配置

#define SVR_CFG_TRANSFER_SEG_CNT_KEY                    "TransferCnt"
#define SVR_CFG_TRANSFER_SEG_CNT                        "8"                             // 数据传输支持8线程
#define SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL_KEY        "HeartbeatDeadTimeInterval"     // 心跳死亡时间间隔
#define SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL            "30000"                         // 默认30s心跳
#define SVR_CFG_PREPARE_POOL_BUFFER_CNT_KEY             "PreparePoolBufferCnt"          // 预先准备的网络buffer缓冲池的buffer个数
#define SVR_CFG_PREPARE_POOL_BUFFER_CNT                 "1024"                          // 预先准备的网络buffer缓冲池的buffer个数
#define SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER_KEY         "MaxMemPoolMBPerTransfer"       // 每个数据传输器内存池大小
#define SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER             "1024"                          // MB为单位
#pragma endregion

#pragma region Dispatcher default config
#define SVR_CFG_DISPATCHER_SEG                          "Dispatcher"                      // 消息分发配置

#define SVR_CFG_DISPATCHER_CNT_KEY                      "DispatcherCnt"                   // 消息分发器数量
#define SVR_CFG_DISPATCHER_CNT                          "1"                               // 消息分发器数量
#pragma endregion

#pragma region objpool default config
#define SVR_CFG_OBJPOOL_SEG                             "ObjPool"                         // 对象池配置

#define SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED_KEY       "MaxAllowMBOccupied"              // 最大允许占用内存大小
#define SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED           "2048"                            // 默认大小2GB
#pragma endregion

#pragma region memorypool default config
#define SVR_CFG_MEMORY_POOL_SEG                         "MemoryPool"                      // 内存池配置

#define SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED_KEY       "MaxAllowMBOccupied"              // 最大允许占用内存大小
#define SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED            "2048"                           // 默认大小2GB
#pragma endregion

// 心跳队列排序
// class FS_ClientComp
// {
// public:
//     bool operator(const FS_Client *l, const FS_Client *r) const;
// };

// 网络模型定义
class NetModuleType
{
public:
    enum
    {
        None = 0,
        Iocp,
        Epoll,
    };
};

#endif
