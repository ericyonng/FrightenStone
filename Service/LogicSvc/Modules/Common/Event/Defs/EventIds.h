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
 * @file  : EventIds.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/15
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_Common_Event_Defs_EventIds_H__
#define __Service_LogicSvc_Modules_Common_Event_Defs_EventIds_H__

#pragma once

 // 参数在EventParamDefs中定义

class EventIds
{
public:
    enum
    {
        Begin = 0,                              // 开始
#pragma region Logic[1, 999]
        Logic_OnSessionConnected = 1,           /* 会话连入
                                                * @param["SessionId"]:会话id
                                                */

        Logic_OnAfterSessionConnected = 2,      /* 会话连入
                                                * @param["SessionId"]:会话id
                                                */

        Logic_OnSessionWillDisconnect = 3,      /* 会话断开
                                                * @param["SessionId"]:会话id
                                                */

        Logic_OnAfterSessionDisconnected = 4,   /* 会话断开
                                                * @param["SessionId"]:会话id
                                                */

        Logic_OnAllConnectToRemoteOpFinish = 5, /* 所有连接远程的操作完成,但并不意味着真正的连接成功
                                                */

        Logic_BeforeMsgDispatch = 6,            /* 消息调度
                                                * @param["SessionId"]:会话id
                                                * @param["MsgDecoder"]:NetMsgDecoder * 解码器对象
                                                */
        #pragma endregion
    };
};
#endif
