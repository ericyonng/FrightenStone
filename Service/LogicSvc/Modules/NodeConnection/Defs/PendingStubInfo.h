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
 * @file  : PendingStubInfo.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/12
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_NodeConnection_Defs_PendingStubInfo_H__
#define __Service_LogicSvc_Modules_NodeConnection_Defs_PendingStubInfo_H__

#pragma once

#include "FrightenStone/exportbase.h"

class PendingStubInfo
{
    OBJ_POOL_CREATE_ANCESTOR(PendingStubInfo);
public:
    PendingStubInfo();
    ~PendingStubInfo();

public:
    UInt64 _pendingStub;
    fs::FS_String _ip;
    UInt16 _port;
    fs::IDelegate<void, UInt64, PendingStubInfo *, Int32> *_callback;  /* 连接回调
                                                             * @param:sessionId 会话id,连接失败情况会为0
                                                             * @param:Stub 存根
                                                             * @param:resultCode 返回的状态码
                                                            */
};

#endif
