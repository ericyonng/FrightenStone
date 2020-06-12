/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : StatusDefs.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Status_Defs_StatusDefs_H__
#define __Frame_Include_FrightenStone_Common_Status_Defs_StatusDefs_H__

#pragma once

#include<FrightenStone/exportbase.h>

namespace StatusDefs
{
    enum StatusEnum :int
    {
        #pragma region [-1, 499]
        Error = -1,                                             // 错误
        SockError = -1,                                         // 套接字错误
        Success = 0,                                            // 成功
        WaitFailure = 1,                                        // 等待错误
        WaitTimeOut = 2,                                        // 等待超时
        Repeat = 3,                                             // 重复
        ParamError = 4,                                         // 参数错误
        Failed = 5,                                             // 失败
        AppInitFail = 6,                                        // 初始化失败
        Unknown = 7,                                            // 未知错误
        NotWorking = 8,                                         // 没有工作
        NotInit = 9,                                            // 未初始化
        InvalidFd = 10,                                         // 无效文件描述符
        BindFail = 11,                                          // 绑定失败
        ListenFail = 12,                                        // 监听失败
        Ignore = 13,                                            // 忽略
        #pragma endregion

        #pragma region SOCKET[500, 599]
        Socket_CreateFailure = 500,                             // 套接字创建失败
        Socket_NotInit = 501,                                   // 套接字未初始化
        Socket_ParamError = 502,                                // 套接字参数错误
        Socket_InvalidSocket = 503,                             // 无效套接字
        Socket_SetSockOptFailed = 504,                          // 设置套接字参数失败
        Socket_GetsockoptFailed = 505,                          // 获取套接字参数时出错
        Socket_Unknown = 506,                                   // 未知
        Socket_SetBlockParamError = 507,                        // 设置套接字阻塞参数失败
        Socket_Error = 508,                                     // 套接字操作错误
        Socket_SetNoBlockParamError = 508,                      // 设置套接字非阻塞失败
        Socket_FillTcpAddrInfoFailure = 509,                    // 填充地址信息失败
        Socket_FcntlError = 510,                                // fcnt函数调用失败
        #pragma endregion

        #pragma region Trigger[600, 699]
        Trigger_InvalidOccasion = 600,                          // 无效的时机
        Trigger_InvalidTriggerType = 601,                       // 无效的触发类型
        Trigger_UnkownAddType = 602,                            // 未知的添加方式
        Trigger_TriggerTypeRepeatInOccasion = 603,              // 触发类型已存在，请不要重复添加
        #pragma endregion 

        #pragma region Aes[700-799]
        Aes_TextLengthNotEnough = 700,                          // 长度不足
        Aes_Not16BytesMultiple = 701,                           // 不是16字节倍数
        Aes_CyphertextIsEmpty = 702,                            // 密文不能为空
        Aes_PlaintextIsEmpty = 703,                             // 明文不能为空
        #pragma endregion

        #pragma region TimeWheel[800-899]
        TimeWheel_ExpiredTimeIsNull = 800,                      // 过期时间为空
        TimeWheel_RegisterAExistsTimeData = 801,                // 重复注册
        TimeWheel_CantRegisterWhenRotatingWheel = 802,          // 转动轮盘时不可在非转动转盘的其他地方注册定时器
        #pragma endregion

        #pragma region SystemUtil[900, 999]
        SystemUtil_GetKernel32HandleFailed = 900,               // 获取kernel32句柄失败
        SystemUtil_GetGlobalMemoryStatusExFuncFailed = 901,     // 获取globalmemorystatusex函数失败
        SystemUtil_GetGlobalMemoryStatusExFailed = 902,         // 获取内存信息失败
        SystemUtil_GetModuleFileNameFailed = 903,               // 获取模块名失败
        SystemUtil_OpenProcessQueryInfomationFailed = 904,      // 查询进程信息失败
        SystemUtil_LoadKernel32LibraryFailed = 905,             // 加载kernel32.dll失败
        SystemUtil_QueryFullProcessImageNameFailed = 906,       // 查询全进程镜像名失败
        SystemUtil_GetProcessImageFileNameFailed = 907,         // 获取进程镜像文件名失败
        SystemUtil_QueryDosDeviceError = 908,                   // 查询驱动器错误
        SystemUtil_GetDriveError = 909,                         // 获取盘符失败
        SystemUtil_GetProcNameFail = 910,                       // 获取进程名失败
        SystemUtil_NotLittleEndian = 911,                       // 非小端字节序
        #pragma endregion

        #pragma region FS_IPUtil[1000, 1099]
        FS_IPUtil_ParamError = 1000,                            // 参数错误
        FS_IPUtil_NotFound = 1001,                              // 未找到
        FS_IPUtil_GetAddrInfoFailed = 1002,                     // 获取地址信息出错
        FS_IPUtil_GetPeerNameFailed = 1003,                     // 获取对方地址信息失败
        FS_IPUtil_GetSockNameFailed = 1004,                     // 获取套接字地址信息失败
        #pragma endregion

        #pragma region Socket[1100, 1199]

        #pragma endregion

        #pragma region Log[1200, 1299]
        Log_CreateDirFail = 1200,                               // 创建目录失败
        Log_CreateLogFileFail = 1201,                           // 创建日志文件失败
        #pragma endregion

        #pragma region XorEncrypt[1300, 1399]
        XorEncrypt_PlainTextLenNotEnough = 1300,                // 明文数据块长度不够
        XorEncrypt_CypherTextSizeNotEnough = 1301,              // 密文空间不足
        XorEncrypt_CypherTextLenNotEnough = 1302,               // 密文数据块长度不够
        XorEncrypt_PlainTextSizeNotEnough = 1303,               // 明文空间不足
        #pragma endregion

        #pragma region CrashHandleUtil[1400, 1499]
        CrashHandleUtil_SymInitializeFail = 1400,               // 符号pdb初始化失败
        #pragma endregion

        #pragma region IOCP[1500, 1599]
        IOCP_CreateCompletionPortFail = 1500,                   // 创建完成端口失败
        IOCP_LoadAcceptExError = 1501,                          // 加载AcceptEx函数失败
        IOCP_RegSocketToCompletionPortFail = 1502,              // 套接字关联到完成端口失败
        IOCP_PostAcceptFail = 1503,                             // 向完成端口投递io连接请求失败
        IOCP_PostRecvFail = 1504,                               // 向完成端口投递io接收请求失败
        IOCP_PostSendFail = 1505,                               // 向完成端口投递io发送请求失败
        IOCP_LoadAcceptExFailForListenSocketIsInvalid = 1506,   // 加载acceptex失败，监听端口无效
        IOCP_WaitTimeOut = 1507,                                // 超时等待
        IOCP_IODisconnect = 1508,                               // io连接断开
        IOCP_WaitOtherError = 1509,                             // 其他错误
        IOCP_ClientForciblyClosed = 1510,                       // 远程客户端被强行关闭
        IOCP_PostQuitFail = 1511,                               // 向完成端口投递quit失败
        IOCP_Quit = 1512,                                       // iocp退出
        IOCP_LoadGetAcceptExSockAddrFunFail = 1513,             // 加载获取客户端地址函数指针出错
        #pragma endregion

        #pragma region Tls[1600, 1699]
        Tls_CreateHandleFail = 1600,                            // tlshandle创建失败
        #pragma endregion

        #pragma region IocpAcceptor[1700, 1799]
        IocpAcceptor_StartFailOfMonitorTaskFailure = 1700,     // iocpconnector 添加monitor task失败
        IocpAcceptor_CreateCfgIniFail = 1701,                  // 创建配置文件失败
        IocpAcceptor_InitDefIniFail = 1702,                    // 初始化写入默认配置失败
        IocpAcceptor_InitListenSocketFail = 1703,              // 初始化监听套接字失败
        #pragma endregion

        #pragma region IocpMsgTransfer[1800, 1899]
        IocpMsgTransfer_StartFailOfMoniterMsgFailure = 1800,    // iocpmsgtransfer 添加monitor任务时候失败
        IocpMsgTransfer_IocpQuit = 1801,                        // iocp退出
        IocpMsgTransfer_CfgError = 1802,                        // 配置错误
        #pragma endregion

        #pragma region FS_IocpMsgHandler[1900, 1999]
        FS_IocpMsgHandler_StartFailOfBusinessProcessThreadFailure = 1901,   // FS_IocpMsgHandler 添加线程任务失败
        #pragma endregion

        #pragma region FS_ServerCore[2000, 2099]
        FS_ServerCore_StartFailOfSvrRuningTaskFailure = 2000,       // 添加svrruing线程任务时候失败
        #pragma endregion

        #pragma region FS_TcpClient[3000, 3099]
        FS_TcpClient_ConnectFail = 3000,                        // 连接失败
        #pragma endregion

        #pragma region FS_IocpTcpClient[4000, 4099]
        FS_IocpTcpClient_IocpQuit= 4000,                        // iocp退出
        #pragma endregion

        #pragma region FS_IocpConnector[4100, 4199]
        FS_IocpConnector_CreateSocketFail = 4100,               // 创建套接字失败
        FS_IocpConnector_ConnectRepeated = 4101,                // 重复连接目标
        FS_IocpConnector_ConnectFail = 4102,                    // 连接失败
        FS_IocpConnector_ConnectOverLimit = 4103,               // 连接数过多
        FS_IocpConnector_SessionIdOverLimit = 4104,             // sessionId无法再分配
        FS_IocpConnector_ConnectTimeOut = 4105,                 // 连接超时
        #pragma endregion

        #pragma region sock error code[4200, 4299]
        FS_SockError_EWOULDBLOCK = 4200,                        // 网络出错，此时应该终止网络io（具体未明，需要查资料，这里阐述个大概）
        FS_SockError_EAGAIN = 4201,                             // 网络io结束
        FS_SockError_UnknownError = 4202,                       // 网络io未知错误
        FS_SockError_BufferFull = 4203,                         // 缓冲区满
        FS_SockError_HaveNoDataToSend = 4204,                   // 没有可发送的数据
        #pragma endregion

        #pragma region FS_EpollConnector[4300, 4399]
        FS_EpollConnector_CreateSocketFail = 4300,              // 创建套接字失败
        FS_EpollConnector_ConnectOverLimit = 4301,              // 连接数过多
        FS_EpollConnector_ConnectTimeOut = 4302,                // 连接超时
        #pragma endregion

        #pragma region FS_EpollAcceptor[4400, 4499]
        FS_EpollAcceptor_InitListenSocketFail = 4400,           // 初始化监听套接字失败
        #pragma endregion

        #pragma region NetModule[4500, 4599]
        NetModule_ConnectorsNotExists = 4500,                   // 连接器不存在
        #pragma endregion

        #pragma region IFS_Acceptor[4600, 4699]
        IFS_Acceptor_InitListenSocketFail = 4600,               // 监听套接字失败
        #pragma endregion

        #pragma region IFS_Connector[4800, 4899]
        IFS_Connector_ConnectOverLimit = 4800,                  // 连接过多
        IFS_Connector_ConnectTimeOut = 4801,                    // 连接超时
        #pragma endregion

        #pragma region Session[4900, 4999]
        Session_Abort = 4900,                                   // 断线
        #pragma endregion

        FrameStatusEnd = 65535,                                 // 框架层状态码截至
    };
};

#endif //!__Frame_Include_FrightenStone_Common_Status_Defs_StatusDefs_H__
