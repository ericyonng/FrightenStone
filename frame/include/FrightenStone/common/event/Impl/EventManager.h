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
 * @file  : EventManager.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Event_Impl_EventManager_H__
#define __Frame_Include_FrightenStone_Common_Event_Impl_EventManager_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/event/Defs/EventDefs.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/status/status.h"

FS_NAMESPACE_BEGIN

class FS_Event;

class BASE_EXPORT EventManager
{
public:
    EventManager();
    virtual ~EventManager();

public:
    /**
     * Add event listener.
     * @param[in] id         - event Id.
     * @param[in] listener   - event listener.
     * @param[in] bindedStub - the binded stub, if not specified, will auto gen stub. 指定的存根若存在则销毁当前的delegate
     * @return FS_ListenerStub - return FS_INVALID_LISTENER_STUB if failed, otherwise return validate stub.
     * delegate由内部释放，外部不释放
     */
    virtual FS_ListenerStub AddListener(int id,
                                          IDelegate<void, FS_Event *> *listener,
                                          const FS_ListenerStub &bindedStub = FS_INVALID_LISTENER_STUB);

    template <typename ObjectType>
    FS_ListenerStub AddListener(int id,
                                  ObjectType *obj,
                                  void (ObjectType::*listener)(FS_Event *),
                                  const FS_ListenerStub &bindedStub = FS_INVALID_LISTENER_STUB);

    /**
     * Remove event listener.
     * @param[in] id - event Id.
     * @return int - success if return StatusDefs::success, otherwise return Error.
     *               specially, if return Error,  and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    virtual int RemoveListener(int id);

    /**
     * Remove event listener using listener stub.
     * @param[in] stub - event listener stub.
     * @return int - success if return success, otherwise return Error,
     *               specially, if return Error, and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    virtual int RemoveListener(const FS_ListenerStub &stub);

    /**
     * Remove event listener using listener stub and clear the listener stub.
     * @param[in] stub - event listener stub.
     * @return int - success if return success, otherwise return Error,
     *               specially, if return Error, and fetch the last error is pending,
     *               it means operation will success on later, but pending at now.
     */
    virtual int RemoveListenerX(FS_ListenerStub &stub);

public:
    /**
     * Fire the event.
     * @param[in] event - event object.
     */
    virtual void FireEvent(FS_Event *event);

    /**
     * Check event manager is firing or not.
     * @return bool - firing flag.
     */
    virtual bool IsFiring() const;

protected:
    /**
     * \brief Wrap the event listener.
     */
    struct _Listener
    {
        FS_ListenerStub _stub;

        int _evId;
        IDelegate<void, FS_Event *> *_listenCallBack;

        _Listener();
    };

    /**
     * \brief Wrap the event operation information.
     */
    struct _Op
    {
        bool _addOp;
        _Listener _listener;
    };

protected:
    /**
     * Search given listen stub in the event manager.
     */
    bool _SearchStub(const FS_ListenerStub &stub) const;

    /**
     * Process event operation.
     */
    int ProcessEventOperation(_Op &op);

    /**
     * Before fire event method.
     */
    void BeforeFireEvent();

    /**
     * After fire event method.
     */
    void AfterFireEvent();

protected:
    int _firing;
    FS_ListenerStub _maxListenerStub;

    // 延迟到afterevent执行相关事件
    typedef std::vector<_Op> _DelayedOps;
    _DelayedOps _delayedOps;

    // 事件对应的监听回调
    typedef std::vector<_Listener> _Listeners;
    typedef std::map<int, _Listeners> _ListenersMap;
    _ListenersMap _listeners;

    // 存根对应的监听回调
    typedef std::map<FS_ListenerStub, _Listener> _StubIndexedListeners;
    _StubIndexedListeners _stubListeners;
};

FS_NAMESPACE_END

#include "FrightenStone/common/event/Impl/EventManagerImpl.h"

#define g_FSEventMgr fs::Singleton<fs::EventManager>::GetInstance()

#endif
