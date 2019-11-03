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
 * @file  : MessageQueueImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/1
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_MessageQueue_Impl_MessageQueue_H__

#pragma once

FS_NAMESPACE_BEGIN
inline void MessageQueue::PushLock()
{
    _msgGeneratorGuard.Lock();
}

inline bool MessageQueue::Push(std::list<FS_MessageBlock *> &msgs)
{
    if(UNLIKELY(!_isWorking))
        return false;

    for(auto iterMsgBlock = msgs.begin(); iterMsgBlock != msgs.end();)
    {
        _msgGeneratorQueue.push_back(*iterMsgBlock);
        iterMsgBlock = msgs.erase(iterMsgBlock);
    }
    _msgGeneratorChange = true;
    _msgGeneratorGuard.Sinal();
    return true;
}

inline void MessageQueue::PushUnlock()
{
    _msgGeneratorGuard.Unlock();
}

inline void MessageQueue::PopLock()
{
    _msgConsumerGuard.Lock();
}

inline Int32 MessageQueue::WaitForPoping(std::list<FS_MessageBlock *> &exportMsgsOut, ULong timeoutMilisec)
{
    if(UNLIKELY(!_isWorking))
        return StatusDefs::NotWorking;

    Int32 st = _msgConsumerGuard.Wait(timeoutMilisec);
    if(_msgComsumerQueueChange)
    {
        for(auto iterMsgBlock = _msgComsumerQueue.begin(); iterMsgBlock != _msgComsumerQueue.end();)
        {
            exportMsgsOut.push_back(*iterMsgBlock);
            iterMsgBlock = _msgComsumerQueue.erase(iterMsgBlock);
        }
        _msgComsumerQueueChange = false;
    }

    return st;
}

inline void MessageQueue::PopUnlock()
{
    _msgConsumerGuard.Unlock();
}

FS_NAMESPACE_END

#endif
