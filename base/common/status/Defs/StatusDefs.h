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
