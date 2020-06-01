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
 * @file  : ProtocolMsgHandlerMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/5/19
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_ProtocolStack_Impl_ProtocolMsgHandlerMgr_H__
#define __Service_LogicSvc_Modules_ProtocolStack_Impl_ProtocolMsgHandlerMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/ProtocolStack/Interface/IProtocolMsgHandlerMgr.h"

class IProtocolMsgHandler;

class ProtocolMsgHandlerMgr : public IProtocolMsgHandlerMgr
{
public:
    ProtocolMsgHandlerMgr();
    ~ProtocolMsgHandlerMgr();

public:
    virtual Int32 OnInitialize();

private:
    void _RegisterEvents();
    void _UnRegisterEvents();
    void _OnReload(fs::FS_Event *ev);
    void _OnWillReloadEnd(fs::FS_Event *ev);
    void _OnSessionConnected(fs::FS_Event *ev);

    fs::IDelegate<void, fs::FS_Session *> *_GetMsgHandler(Int32 handlerType);

private:

    // 协议handler
    std::map<Int32, fs::IDelegate<void, fs::FS_Session *> *> _handlerTypeRefMsgHandlerDeleg;

    // 事件
    fs::FS_ListenerStub _reloadEvStub;
    fs::FS_ListenerStub _willReloadEndEvStub;
    fs::FS_ListenerStub _sessionConnected;
};

#pragma region inline
inline fs::IDelegate<void, fs::FS_Session *> *ProtocolMsgHandlerMgr::_GetMsgHandler(Int32 handlerType)
{
    auto iterHandler = _handlerTypeRefMsgHandlerDeleg.find(handlerType);
    return iterHandler == _handlerTypeRefMsgHandlerDeleg.end() ? NULL : iterHandler->second;
}
#pragma endregion
#endif
