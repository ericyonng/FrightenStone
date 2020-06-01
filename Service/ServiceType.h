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
 * @file  : ServiceType.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/15
 * @brief :
 */
#ifndef __Service_ServiceType_H__
#define __Service_ServiceType_H__

#pragma once

class ServiceType
{
public:
    enum 
    {
        Begin = 0,              // 开始
        BusinessLogic = 1,      // 业务逻辑服务
        Account = 2,            // 账户服务
        Center = 3,             // 中心控制服务
        DB = 4,                 // db服务
        Gateway = 5,            // 网关服务
        ClientSimulation = 6,   // 模拟客户端
        End,
    };

    static const char *GetStr(Int32 serviceId);
};
#endif
