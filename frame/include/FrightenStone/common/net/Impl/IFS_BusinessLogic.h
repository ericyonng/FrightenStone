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
 * @file  : IFS_BusinessLogic.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/20
 * @brief :
 * 
 *
 * 
 */

#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_BusinessLogic_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_BusinessLogic_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/net/Impl/FS_MsgReadStream.h"
#include "FrightenStone/common/net/Impl/FS_MsgWriteStream.h"
#include <FrightenStone/common/component/Impl/FS_Delegate.h>
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

struct NetMsg_DataHeader;
class  IFS_MsgDispatcher;
class IUserSys;
class IFS_FacadeFactory;
class IFS_Facade;

class BASE_EXPORT IFS_BusinessLogic
{
    OBJ_POOL_CREATE_DEF(IFS_BusinessLogic);
public:
    IFS_BusinessLogic();
    virtual ~IFS_BusinessLogic();
    virtual void Release() = 0;

public:
    virtual void OnRegisterFacades() = 0;
    // 注册facades
    virtual Int32 BeforeStart();
    // 启动facades
    virtual Int32 Start();
    virtual void AfterStart() { _isInit = true;}
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose() {}

    /* 网络事件 */
public:
    virtual void OnMsgDispatch(UInt64 sessionId, NetMsgDecoder *msgDecoder) = 0;
    virtual void OnSessionDisconnected(UInt64 sessionId, std::list<IDelegate<void, UInt64> *> *disconnectedDelegate) = 0;
    virtual void OnConnectorConnectOpFinish() = 0;
    virtual IUserSys *OnSessionConnected(UInt64 sessionId) = 0;

public:
    void SetDispatcher(IFS_MsgDispatcher *dispatcher);
    IFS_MsgDispatcher *GetDispatcher();
    void SetTimeWheel(TimeWheel *timeWheel);
    TimeWheel *GetTimeWheel();

    // 订阅协议
    virtual void SubscribeProtocol(UInt32 protocolCmd, IDelegate<void, UInt64, NetMsgDecoder *> *handler);
    virtual void InvokeProtocolHandler(UInt64 sessionId, NetMsgDecoder *msgDecoder);
    template <typename FacadeFactoryType>
    Int32 RegisterFacade();
    virtual Int32 RegisterFacade(IFS_FacadeFactory *facadeFactory);
    virtual Int32 RegisterFacade(IFS_Facade *facade);
    virtual IFS_Facade *GetFacade(const FS_String &facadeName);
    virtual std::vector<IFS_Facade *> GetFacades(const FS_String &facadeName);

protected:
    Int32 _InitFacades();
    Int32 _StartFacades();
    void _BeforeFacadesClose();
    void _CloseFacades();
    void _DestroyWillRegFacades();
    void _AddFacade(IFS_Facade *facade);

protected:

    // 注册facade
    class _WillRegFacade
    {
    public:
        IFS_Facade * facade;
        IFS_FacadeFactory *facadeFactory;

        _WillRegFacade(IFS_Facade *facade);
        _WillRegFacade(IFS_FacadeFactory *facadeFactory);
    };
    typedef std::vector<_WillRegFacade> _WillRegFacades;
    _WillRegFacades _willRegFacades;

    // facade对象
    typedef std::vector<IFS_Facade *> _Facades;
    _Facades _facades;
    typedef std::map<FS_String, _Facades> _Facades2;
    _Facades2 _facades2;

    std::atomic_bool _isInit;
    IFS_MsgDispatcher *_dispatcher;     // 消息分发
    TimeWheel *_timeWheel;              // 时间轮盘
    std::map<UInt32, IDelegate<void, UInt64, NetMsgDecoder *> *> _protocolCmdRefHandlers;
};


FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_BusinessLogicImpl.h"

#endif
