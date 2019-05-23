#ifndef __Base_Common_Status_Defs_StatusDefs_H__
#define __Base_Common_Status_Defs_StatusDefs_H__
/**
* @file StatusDefs.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

#include<base/exportbase.h>

class BASE_EXPORT StatusDefs
{
public:
    enum StatusEnum:int
    {
        #pragma region 通用码[-1-499]
        Error = -1,                                             // 错误
        Success = 0,                                            // 成功
        WaitEventFailure = 1,                                   // 时间对象等待失败
        WaitEventTimeOut = 2,                                   // 事件对象等待超时
        Repeat = 3,                                             // 重复
        ParamError = 4,                                         // 参数错误
        #pragma endregion

        #pragma region SOCKET[500-599]
        Socket_CreateFailure = 500,                             // 创建套接字失败
        Socket_NotInit = 501,                                   // 未初始化套接字
        #pragma endregion

        #pragma region 触发器[600-699]
        Trigger_InvalidOccasion = 600,                          // 无效触发时机
        Trigger_InvalidTriggerType = 601,                       // 无效触发器
        Trigger_UnkownAddType = 602,                            // 注册触发器时未知的加入方式
        Trigger_TriggerTypeRepeatInOccasion = 603,              // 在加入的时机中触发器类型重复
        #pragma endregion 

        #pragma region 登陆[600-699]
        #pragma endregion
    };
};

#endif //!__Base_Common_Status_Defs_StatusDefs_H__
