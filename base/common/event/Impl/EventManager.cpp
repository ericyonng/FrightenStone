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
 * @file  : EventManager.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/event/Impl/EventManager.h"
#include "base/common/assist/utils/Impl/FS_GuidUtil.h"
#include "base/common/event/Impl/FS_Event.h"

FS_NAMESPACE_BEGIN

FS_ListenerStub EventManager::AddListener(int id, IDelegatePlus<void, FS_Event *> *listener, const FS_ListenerStub &bindedStub /*= FS_INVALID_LISTENER_STUB*/)
{
    if(id <= 0 || listener == NULL)
    {
        Fs_SafeFree(listener);
        return FS_INVALID_LISTENER_STUB;
    }

    FS_String stub;
    if(bindedStub != FS_INVALID_LISTENER_STUB)
    {
        if(_SearchStub(bindedStub))
        {
            Fs_SafeFree(listener);
            return FS_INVALID_LISTENER_STUB;
        }

        stub = bindedStub;
    }
    else
        stub = FS_GuidUtil::GenStr();

    _Op op;
    op._addOp = true;
    op._listener._evId = id;
    op._listener._listenCallBack = listener;
    op._listener._stub = stub;

    // 若监听时候正在fire状态则放入延迟队列（延迟到after fire）,且当前的监听会失效
    if(IsFiring())
    {
        _delayedOps.push_back(op);
        return FS_INVALID_LISTENER_STUB;
    }

    if(ProcessEventOperation(op) != StatusDefs::Success)
    {
        Fs_SafeFree(listener);
        return FS_INVALID_LISTENER_STUB;
    }

    return op._listener._stub.c_str();
}

int EventManager::RemoveListener(int id)
{
    if(id <= 0)
        return StatusDefs::Error;

    _Op op;
    op._addOp = false;
    op._listener._evId = id;

    // 正在fire则放入延迟队列，延迟执行
    if(IsFiring())
    {
        _delayedOps.push_back(op);
        return StatusDefs::Error;
    }

    return ProcessEventOperation(op);
}

int EventManager::RemoveListener(const FS_ListenerStub &stub)
{
    _Op op;
    op._addOp = false;
    op._listener._stub << stub;

    // 正在fire则放入延迟队列，延迟执行
    if(IsFiring())
    {
        _delayedOps.push_back(op);
        return StatusDefs::Error;
    }

    return ProcessEventOperation(op);
}

void EventManager::FireEvent(FS_Event *event)
{
    BeforeFireEvent();

    _ListenersMap::iterator mIt = _listeners.find(event->GetId());
    if(mIt != _listeners.end())
    {
        _Listeners &listeners = mIt->second;
        for(_Listeners::iterator lIt = listeners.begin();
            lIt != listeners.end();
            lIt++)
        {
            _Listener &listener = *lIt;
            if(listener._listenCallBack)
                (*listener._listenCallBack)(std::forward<FS_Event *>(event));
        }
    }

    if(event->IsDontDelAfterFire())
    {
        AfterFireEvent();
    }
    else
    {
        Fs_SafeFree(event);
        AfterFireEvent();
    }
}

bool EventManager::_SearchStub(const FS_ListenerStub &stub) const
{
    return (_stubListeners.find(stub) != _stubListeners.end()) ? true : false;
}

int EventManager::ProcessEventOperation(EventManager::_Op &op)
{
    _Listener &listener = op._listener;
    if(op._addOp)
    {// 加入事件操作
        _ListenersMap::iterator mIt = _listeners.find(listener._evId);
        if(mIt == _listeners.end())
            mIt = _listeners.insert(std::make_pair(listener._evId, std::vector<_Listener>())).first;

        _Listeners &listeners = mIt->second;
        listeners.push_back(listener);

        _stubListeners.insert(std::make_pair(listener._stub, listener));
    }
    else
    {// 移除事件
        if(listener._evId > 0)
        {
            _ListenersMap::iterator mIt = _listeners.find(listener._evId);
            if(mIt == _listeners.end())
                return StatusDefs::Error;

            // 移除
            _Listeners &listeners = mIt->second;
            for(_Listeners::iterator lIt = listeners.begin();
                lIt != listeners.end();
                lIt++)
            {
                _Listener &l = *lIt;

                _stubListeners.erase(l._stub);
                FS_Release(l._listenCallBack);
            }

            _listeners.erase(mIt);
        }
        else
        {
            _StubIndexedListeners::iterator stubIt = _stubListeners.find(listener._stub);
            if(stubIt == _stubListeners.end())
                return StatusDefs::Error;

            const int evId = stubIt->second._evId;
            _ListenersMap::iterator mIt = _listeners.find(evId);

            _Listeners &listeners = mIt->second;
            for(_Listeners::iterator lIt = listeners.begin();
                lIt != listeners.end();
                lIt++)
            {
                _Listener &l = *lIt;
                if(l._stub == listener._stub)
                {
                    FS_Release(l._listenCallBack);
                    listeners.erase(lIt);
                    break;
                }
            }

            if(listeners.empty())
                _listeners.erase(mIt);

            _stubListeners.erase(stubIt);
        }
    }

    return StatusDefs::Success;
}

void EventManager::AfterFireEvent()
{
    // after fire 执行延迟执行事件
    if(--_firing == 0)
    {
        for(size_t i = 0; i < _delayedOps.size(); ++i)
            ProcessEventOperation(_delayedOps[i]);

        _delayedOps.clear();
    }
}
FS_NAMESPACE_END


