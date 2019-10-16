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
 * @file  : StatusDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Status_Defs_StatusDefs_H__
#define __Base_Common_Status_Defs_StatusDefs_H__

#pragma once

#include<base/exportbase.h>

class BASE_EXPORT StatusDefs
{
public:
    enum StatusEnum :int
    {
#pragma region [-1, 499]
        Error = -1,                                             // 错误
        Success = 0,                                            // 成功
        WaitEventFailure = 1,                                   // 等待错误
        WaitEventTimeOut = 2,                                   // 等待超时
        Repeat = 3,                                             // 重复
        ParamError = 4,                                         // 参数错误
        Failed = 5,                                             // 失败
        AppInitFail = 6,                                        // 初始化失败
        Unknown = 7,                                            // 未知错误
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
        #pragma endregion

        #pragma region FS_IPUtil[1000, 1099]
        FS_IPUtil_ParamError = 1000,                            // 参数错误
        FS_IPUtil_NotFound = 1001,                              // 未找到
        FS_IPUtil_GetAddrInfoFailed = 1002,                     // 获取地址信息出错
        FS_IPUtil_GetPeerNameFailed = 1003,                     // 获取对方地址信息失败
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
        IOCP_PostSendFail = 1504,                               // 向完成端口投递io发送请求失败
        IOCP_LoadAcceptExFailForListenSocketIsInvalid = 1505,   // 加载acceptex失败，监听端口无效
        IOCP_WaitTimeOut = 1506,                                // 超时等待
        IOCP_IODisconnect = 1507,                               // io连接断开
        IOCP_WaitOtherError = 1508,                             // 其他错误
        IOCP_ClientForciblyClosed = 1509,                       // 远程客户端被强行关闭
        IOCP_PostQuitFail = 1510,                               // 向完成端口投递quit失败
        IOCP_Quit = 1511,                                       // iocp退出
        IOCP_LoadGetAcceptExSockAddrFunFail = 1512,             // 加载获取客户端地址函数指针出错
        #pragma endregion

        #pragma region Tls[1600, 1699]
        Tls_CreateHandleFail = 1600,                            // tlshandle创建失败
        #pragma endregion

        #pragma region IocpConnector[1700, 1799]
        IocpConnector_StartFailOfMonitorTaskFailure = 1700,     // iocpconnector 添加monitor task失败
        #pragma endregion

    };
};

#endif //!__Base_Common_Status_Defs_StatusDefs_H__
