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
 * @file  : BlackListCfg.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/31
 * @brief : 黑名单配置子项
 */
#ifndef __Service_LogicSvc_Cfgs_Impl_BlackListCfg_H__
#define __Service_LogicSvc_Cfgs_Impl_BlackListCfg_H__
#pragma once

#include "FrightenStone/exportbase.h"

class BlackListCfg
{
    OBJ_POOL_CREATE_DEF(BlackListCfg);

public:
    BlackListCfg();
    ~BlackListCfg();

public:
    Int32 _breakCount;                  // 违规次数
    Int64 _expireTimeSlice;             // 违规次数对应的黑名单失效时长,0表示永不失效
};
#endif
