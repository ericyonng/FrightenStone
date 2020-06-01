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
 * @file  : EventManagerImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Event_Impl_EventManager_H__
#pragma once

FS_NAMESPACE_BEGIN

inline EventManager::_Listener::_Listener()
    : _stub(FS_INVALID_LISTENER_STUB)
    , _evId(0)
    , _listenCallBack(NULL)
{
}


inline EventManager::EventManager()
: _firing(0)
,_maxListenerStub(0)
{

}

inline EventManager::~EventManager()
{
    for(_DelayedOps::iterator it = _delayedOps.begin();
        it != _delayedOps.end();
        it++)
    {
        _Op &op = *it;
        FS_Release(op._listener._listenCallBack);
    }

    for(_ListenersMap::iterator mIt = _listeners.begin();
        mIt != _listeners.end();
        mIt++)
    {
        _Listeners listeners = mIt->second;
        for(_Listeners::iterator lIt = listeners.begin();
            lIt != listeners.end();
            lIt++)
        {
            _Listener &listener = *lIt;
            FS_Release(listener._listenCallBack);
        }
    }
}
template <typename ObjectType>
inline FS_ListenerStub EventManager::AddListener(int id,
                                    ObjectType *obj,
                                    void (ObjectType::*listener)(FS_Event *),
                                    const FS_ListenerStub &bindedStub)
{
    if(!obj || !listener)
        return FS_INVALID_LISTENER_STUB;

    auto listenerDelegate = DelegatePlusFactory::Create(obj, listener);
    return AddListener(id, listenerDelegate, bindedStub);
}

inline int EventManager::RemoveListenerX(FS_ListenerStub &stub)
{
    if(RemoveListener(stub) != StatusDefs::Success)
    {
        stub = FS_INVALID_LISTENER_STUB;
        return StatusDefs::Error;
    }

    stub = FS_INVALID_LISTENER_STUB;
    return StatusDefs::Success;
}

inline bool EventManager::IsFiring() const
{
    return _firing > 0;
}

inline void EventManager::BeforeFireEvent()
{
    ++_firing;
}

FS_NAMESPACE_END

#endif
