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
    enum StatusEnum:int
    {
        #pragma region [-1-499]
        Error = -1,                                             // 错误
        Success = 0,                                            // 成功
        WaitEventFailure = 1,                                   // 等待错误
        WaitEventTimeOut = 2,                                   // 等待超时
        Repeat = 3,                                             // 重复
        ParamError = 4,                                         // 参数错误
        #pragma endregion

        #pragma region SOCKET[500-599]
        Socket_CreateFailure = 500,                             // 套接字创建失败
        Socket_NotInit = 501,                                   // 套接字未初始化
        #pragma endregion

        #pragma region Trigger[600-699]
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

        #pragma region SystemUtil[900-999]
        SystemUtil_GetKernel32HandleFailed = 900,               // 获取kernel32句柄失败
        SystemUtil_GetGlobalMemoryStatusExFuncFailed = 901,     // 获取globalmemorystatusex函数失败
        SystemUtil_GetGlobalMemoryStatusExFailed = 902,         // 获取内存信息失败
        #pragma endregion

    };
};

#endif //!__Base_Common_Status_Defs_StatusDefs_H__
