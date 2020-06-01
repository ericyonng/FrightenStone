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
 * @file  : ProtocolProtectorGlobalFacade.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/03/14
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_ProtocolProtector_Impl_ProtocolProtectorGlobalFacade_H__
#define __Service_LogicSvc_Modules_ProtocolProtector_Impl_ProtocolProtectorGlobalFacade_H__

#pragma once

#include "Service/LogicSvc/Common.h"

class ProtocolProtectorGlobalFacade : public fs::IFS_Facade
{
public:
    virtual Int32 OnInitialize();

private:
    void _RegisterProtocols();
    void _OnLoginRes(UInt64 sessionId, fs::NetMsgDecoder *decoder);
};
#endif
