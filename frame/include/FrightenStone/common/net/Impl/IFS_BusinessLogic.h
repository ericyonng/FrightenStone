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
 * @file  : IFS_BusinessLogic.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/20
 * @brief :
 * 
 *      facade逻辑:
                    1.在beforestart时候先通过factory将每个系统的facade缓存与willregfacade中
                    2.在start的时候先将每个facade initialize
                    3.再执行facade的beforestart与start启动系统(基于facade启动)
                    4.可以通过globalsys与usersys在执行到beforestart的时候二次注册到globalsys或者usersys，让globalsys与usersys模块在start的时候创建出真正的系统对象(基于globalsysy或者usersys启动)并执行对应的beforestart与start启动系统,
                    5.若注册到globalsys或者usersys时,对象的获取可以根据globalsysy通过getsys或者usersys通过getsys取到
        usersys或者globalsys的创建过程:
                    1.先在initilize时候将factory重新注册到globalsysmgr或者usermgr中,这时候global或者user会createsysinfo取得子系统字符串等系统信息,并映射到字典
                    2.user connected之后会创建user对象这个时候usermgr会通过createsys将子系统关联到user身上等
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
#include <FrightenStone/common/component/Impl/FS_Delegate.h>
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/objpool/objpool.h"
#include "FrightenStone/common/status/status.h"

FS_NAMESPACE_BEGIN

struct NetMsg_DataHeader;
class  IFS_MsgDispatcher;
class IUserSys;
class IFS_FacadeFactory;
class IFS_Facade;
struct  FS_ConnectInfo;
class FS_Session;

class BASE_EXPORT IFS_BusinessLogic
{
    OBJ_POOL_CREATE_DEF(IFS_BusinessLogic);
public:
    IFS_BusinessLogic(Int32 serviceId);
    virtual ~IFS_BusinessLogic();
    virtual void Release() = 0;

public:
    virtual void OnRegisterFacades() = 0;
    virtual Int32 OnLoadCfgs() { return StatusDefs::Success; }
    virtual void OnClose() {}
    // 注册facades
    virtual Int32 BeforeStart();
    // 启动facades
    virtual Int32 Start();
    virtual void AfterStart() { }
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose() { if(!_isInit)return; }

    /* 网络事件 */
public:
    virtual void OnLocalServerReady() {}
    virtual void WillStartup() {}
    virtual void OnSessionDisconnected(UInt64 sessionId, std::list<IDelegate<void, UInt64> *> *disconnectedDelegate) = 0;
    virtual void OnConnectFailureRes(const FS_ConnectInfo &connectInfo, Int32 resultCode) = 0;
    virtual Int32 OnSessionConnected(UInt64 sessionId, UInt64 stub) = 0;
    virtual void OnSessionMsgHandle(FS_Session *session) = 0;

public:
    void SetDispatcher(IFS_MsgDispatcher *dispatcher);
    IFS_MsgDispatcher *GetDispatcher();
    void SetTimeWheel(TimeWheel *timeWheel);
    TimeWheel *GetTimeWheel();
    Int32 GetServiceId() const;

    template <typename FacadeFactoryType>
    Int32 RegisterFacade();
    virtual Int32 RegisterFacade(IFS_FacadeFactory *facadeFactory);
    virtual Int32 RegisterFacade(IFS_Facade *facade);
    virtual IFS_Facade *GetFacade(const FS_String &facadeName);
    virtual std::vector<IFS_Facade *> GetFacades(const FS_String &facadeName);

protected:
    Int32 _InitFacades();
    Int32 _StartFacades();

    // will close->beforeclose->close->destroy都是在业务线程做
    void _WillCloseFacades();
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
    Int32 _serviceId;
};


FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_BusinessLogicImpl.h"

#endif
