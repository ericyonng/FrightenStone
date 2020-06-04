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
 * @file  : LogicSvc.h
 * @author: Eric Yonng<120453674@qq.com>
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
    OBJ_POOL_CREATE_DERIVE(LogicSvc, IFS_BusinessLogic);
public:
    LogicSvc(Int32 serviceId);
    ~LogicSvc() {}
    virtual void Release();
    virtual void OnClose();

public:
    virtual Int32 OnLoadCfgs();
    virtual void OnRegisterFacades();
    virtual void OnLocalServerReady();
    virtual void WillStartup();

    virtual void OnSessionDisconnected(UInt64 sessionId, std::list<fs::IDelegate<void, UInt64> *> *disconnectedDelegate);
    virtual Int32 OnSessionConnected(UInt64 sessionId, UInt64 stub);
    virtual void OnConnectFailureRes(const fs::FS_ConnectInfo &connectInfo, Int32 resultCode);
    virtual void OnSessionMsgHandle(fs::FS_Session *session);

protected:
//     ProtocolProtectorGlobal *_protector;
// 
//     std::set<Int32> _pendingBeforeStartup;          // 简单的startup之前的事情比如互联,不可进行数据交换
};

#pragma region inline
inline LogicSvc::LogicSvc(Int32 serviceId)
    :fs::IFS_BusinessLogic(serviceId)
{
}

#pragma endregion
#endif
