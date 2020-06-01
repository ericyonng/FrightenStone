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
 * @file  : ProtocolProtectorGlobal.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/03/14
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/ProtocolProtector/Impl/ProtocolProtectorGlobal.h"

IProtocolProtectorGlobal *g_ProtocolProtectorGlobal = NULL;

ProtocolProtectorGlobal::ProtocolProtectorGlobal()
{
    g_ProtocolProtectorGlobal = this;
}

ProtocolProtectorGlobal::~ProtocolProtectorGlobal()
{
    _UnRegisterEvents();
}

void ProtocolProtectorGlobal::OnInitialize()
{
    _RegisterEvents();
}

Int32 ProtocolProtectorGlobal::CheckProtocol(UInt64 sessionId, fs::NetMsgDecoder *mesgDecoder)
{
    // 校验协议号
    // 校验包长度
    // 获取会话地址
    // 不合法则进acceptor黑名单
    // 
    return StatusDefs::Success;
}

void ProtocolProtectorGlobal::_RegisterEvents()
{

}

void ProtocolProtectorGlobal::_UnRegisterEvents()
{

}
