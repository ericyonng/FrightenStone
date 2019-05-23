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
        #pragma region [-1-499]
        Error = -1,                                             // 
        Success = 0,                                            // 
        WaitEventFailure = 1,                                   // 
        WaitEventTimeOut = 2,                                   // 
        Repeat = 3,                                             // 
        ParamError = 4,                                         // 
        #pragma endregion

        #pragma region SOCKET[500-599]
        Socket_CreateFailure = 500,                             // 
        Socket_NotInit = 501,                                   // 
        #pragma endregion

        #pragma region [600-699]
        Trigger_InvalidOccasion = 600,                          // 
        Trigger_InvalidTriggerType = 601,                       // 
        Trigger_UnkownAddType = 602,                            // 
        Trigger_TriggerTypeRepeatInOccasion = 603,              // 
        #pragma endregion 

        #pragma region [600-699]
        #pragma endregion
    };
};

#endif //!__Base_Common_Status_Defs_StatusDefs_H__
