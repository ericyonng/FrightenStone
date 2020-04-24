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
 * @file  : LogicSvc.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/15
 * @brief :
 */
#ifndef __Service_LogicSvc_LogicSvc_H__
#define __Service_LogicSvc_LogicSvc_H__

#pragma once

#include "FrightenStone/exportbase.h"

class ProtocolProtectorGlobal;

class LogicSvc :public fs::IFS_BusinessLogic
{
public:
    LogicSvc() {}
    ~LogicSvc() {}
    virtual void Release();

public:
    virtual void OnRegisterFacades();
    virtual void OnMsgDispatch(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder);

    virtual void OnSessionDisconnected(UInt64 sessionId, std::list<fs::IDelegate<void, UInt64> *> *disconnectedDelegate);
    virtual Int32 OnSessionConnected(UInt64 sessionId);
    virtual void OnConnectorConnectOpFinish();

protected:
    ProtocolProtectorGlobal *_protector;
};
#endif
