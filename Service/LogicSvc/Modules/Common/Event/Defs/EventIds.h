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
 * @file  : EventIds.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/15
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_Common_Event_Defs_EventIds_H__
#define __Service_LogicSvc_Modules_Common_Event_Defs_EventIds_H__

#pragma once

 // 参数在EventParamDefs中定义 事件不支持跨线程

class EventIds
{
public:
    enum
    {
        Begin = 0,                              // 开始
        #pragma region Logic[1, 999]
        Logic_OnSessionWillConnected = 1,       /* 会话连入 user系统创建
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["Stub"]:存根UInt64
                                                */
        Logic_OnSessionConnecting = 2,          /* 会话连入 用于处理黑名单等连入规则 业务的东西请勿在这个时候做
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["Stub"]:存根UInt64
                                                */
        Logic_OnSessionConnectFail = 3,         /* 会话连入失败 当被黑名单等连入规则判定为失败的连入时需要派遣或监听该事件
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["Stub"]:存根UInt64
                                                */
        Logic_OnSessionConnected = 4,           /* 会话连入
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["Stub"]:存根UInt64
                                                */

        Logic_OnAfterSessionConnected = 5,      /* 会话连入
                                                * @param["SessionId"]:会话id UInt64
                                                * @param[Stub]:存根UInt64
                                                */

        Logic_OnSessionWillDisconnect = 6,      /* 会话断开 
                                                * @param["SessionId"]:会话id UInt64
                                                */

        Logic_OnSessionDisconnect = 7,          /* 会话断开 NodeMgr会在这个地方移除本地主动连接的节点
                                                * @param["SessionId"]:会话id UInt64
                                                */

        Logic_OnAfterSessionDisconnected = 8,   /* 会话断开 这时候user将被移除，所有依赖user对象的不可在这个时候处理业务
                                                * @param["SessionId"]:会话id UInt64
                                                */

        Logic_LocalServerReady = 9,             /* 本地服务器准备就绪 此时应加载各个系统配置,进行互联,并设置eventcareBeforeStartup等
                                                */
        Logic_WillStartup = 10,                  /* 服务器即将启动,需要判断是否可以startup等startup之前的检查
                                                */
        Logic_TurnStartup = 11,                  /* 服务器完全启动,可以接入协议处理
                                                */
        Logic_TurnStartupFinish = 12,            /* 服务器完全启动,可以处理其他逻辑了
                                                */
        Logic_ConnectFailure = 13,              /* 连接失败
                                                * @param[TargetIp]:目标ip string
                                                * @param[TargetPort]:目标端口 UInt16
                                                * @param[Stub]:存根   UInt64
                                                * @param[ResultCode]:失败状态码 Int32
                                                */
        Logic_AsynConnectResult = 14,           /* 异步连接结果
                                                * @param[SessionId]:会话id UInt64 有可能为0,为0是连接失败情况
                                                * @param[TargetIp]:目标ip string
                                                * @param[TargetPort]:目标端口 UInt16
                                                * @param[Stub]:存根   UInt64
                                                * @param[ResultCode]:状态码 见StatusDefs Int32
                                                */
        #pragma endregion

        #pragma region User[1000, 1099]
        User_Login = 1000,                      /* 登陆
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["UserId"]:UserId UInt64
                                                * @param["RecvMsgId"]:收到的消息id Int32
                                                */
        User_Created = 1001,                    /* 用户创建完成
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["UserId"]:UserId UInt64
                                                */
        User_LoginRes = 1002,                   /* 用户登陆结果
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["UserId"]:UserId UInt64
                                                * @param["RecvMsgId"]:收到的消息id Int32
                                                */
        User_WillClose = 1003,                  /* 用户即将关闭
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["UserId"]:UserId UInt64
                                                * @param["Reason"]:原因 Int32
                                                */
        User_Close = 1004,                      /* 用户即将关闭
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["UserId"]:UserId UInt64
                                                * @param["Reason"]:原因 Int32
                                                */
        User_CloseFinish = 1005,                /* 用户关闭结束
                                                * @param["SessionId"]:会话id UInt64
                                                * @param["UserId"]:UserId UInt64
                                                * @param["Reason"]:原因 Int32
                                                */
        #pragma endregion

        #pragma region Cfgs[1100, 1199]
        // reload待测试
        Cfgs_Reload = 1100,                     /* 配置重载 只更新配置,并记录配置变更情况（增删改）
                                                * @param["CfgFileType"]:配置文件类型 Int32
                                                * @param["OldCfgFileMd5"]:旧的配置文件md5 string
                                                * @param["NewCfgFileMd5"]:新的配置文件md5 string
                                                */
        Cfgs_WillReloadEnd = 1101,              /* 配置重载即将结束 从记录下来的配置变更情况,对数据进行修正操作 TODO未接入
                                                */
        Cfgs_ReloadEnd = 1102,                  /* 配置重载即将结束 时序先后顺序 TODO:未接入
                                                */
        #pragma endregion
    };
};
#endif
