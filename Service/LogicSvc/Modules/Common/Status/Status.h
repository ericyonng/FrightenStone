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
 * @file  : Status.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/24
 * @brief : 状态码
 */

#ifndef __Service_LogicSvc_Modules_Common_Status_Status_H__
#define __Service_LogicSvc_Modules_Common_Status_Status_H__

#pragma once

#include "FrightenStone/exportbase.h"

namespace StatusDefs
{
    enum LogicSvc :int
    {
        LogicSvcStatusStart = StatusDefs::FrameStatusEnd,

        #pragma region NormalHandler[70000, 70099]
        NormalMsgHandler_DecodeFail = 70000,                    // 解码失败
        NormalMsgHandler_PackageLenOverBufferSize = 70001,      // 网络包大小大于缓冲区大小
        #pragma endregion

        #pragma region User[70100, 70199]
        #pragma endregion

        #pragma region IniMgr[70200, 70299]
        Cfg_PathError = 70200,                                   // 路径错误
        Cfg_InitFail = 70201,                                    // 配置初始化错误
        Cfg_HaveNoCfgs = 70202,                                  // 没有配置
        Cfg_NotLoadBefore = 70203,                               // 未曾加载过
        Cfg_ReadValueFail = 70204,                               // 读取失败
        Cfg_NoKeys = 70205,                                      // 没有主键
        #pragma endregion

        #pragma region NodeMgr[70300, 70399]
        NodeConnectionMgr_ConnetingFail = 70300,                 // 连入时被判触犯规则等导致连入失败
        #pragma endregion
    };
};

#endif
