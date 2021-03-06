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
 * @file  : BlackListElement.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/31
 * @brief : 黑名单成员
 */

#pragma once

#include "Service/LogicSvc/Common.h"

// 黑民单过期队列成员
struct BlackListElement
{
    OBJ_POOL_CREATE_ANCESTOR(BlackListElement);
    BlackListElement();
    Int64 _expireTime;              // 过期时间最大值是永久黑名单
    UInt64 _breakRuleCount;         // 违规次数
    fs::FS_String _ip;              // ip地址
};
