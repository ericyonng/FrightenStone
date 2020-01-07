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
 * @file  : IFS_NetEngineImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/31
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_NetEngine_H__

#pragma once

FS_NAMESPACE_BEGIN

inline IFS_NetEngine::IFS_NetEngine()
    :_totalCfgs(NULL)
    ,_isInit(false)
    ,_cpuInfo(NULL)
    ,_connector(NULL)
    ,_curMaxCompId(0)
    ,_concurrentMq(NULL)
    ,_curSessionConnecting(0)
    ,_sessionConnectedBefore(0)
    ,_sessionDisconnectedCnt(0)
    ,_recvMsgCountPerSecond(0)
    ,_recvMsgBytesPerSecond(0)
    ,_sendMsgBytesPerSecond(0)
    ,_heartbeatTimeOutDisconnected(0)
{
}

inline void IFS_NetEngine::Wait()
{
    _waitForClose.Lock();
    _waitForClose.DeadWait();
    _waitForClose.Unlock();
}

inline void IFS_NetEngine::Sinal()
{
    _waitForClose.Lock();
    _waitForClose.Sinal();
    _waitForClose.Unlock();
}

inline void IFS_NetEngine::_HandleCompEv_Disconnected(UInt64 sessionId)
{
    --_curSessionConnecting;
    ++_sessionDisconnectedCnt;

    // 关闭接受连接
    for(auto &acceptor : _acceptors)
        acceptor->OnDisconnected(sessionId);
}

inline void IFS_NetEngine::_HandleCompEv_HeartBeatTimeOut()
{
    ++_heartbeatTimeOutDisconnected;
}

inline void IFS_NetEngine::_HandleCompEv_RecvMsg(Int64 transferBytes)
{
    _recvMsgBytesPerSecond += transferBytes;
}

inline void IFS_NetEngine::_HandleCompEv_RecvMsgAmount()
{
    ++_recvMsgCountPerSecond;
}

inline void IFS_NetEngine::_HandleCompEv_SendMsg(Int64 transferBytes)
{
    _sendMsgBytesPerSecond += transferBytes;
}

inline UInt32 IFS_NetEngine::_GenerateCompId()
{
    return ++_curMaxCompId;
}

inline ConcurrentMessageQueueNoThread *IFS_NetEngine::_GetConcurrentMQ()
{
    return _concurrentMq;
}

inline void IFS_NetEngine::_AddNewComp(UInt32 compId, IFS_EngineComp *newComp)
{
    _compIdRefComps.insert(std::make_pair(compId, newComp));
}

inline IFS_EngineComp *IFS_NetEngine::_GetComp(UInt32 compId)
{
    auto iterComp = _compIdRefComps.find(compId);
    return iterComp->second;
}

FS_NAMESPACE_END

#endif
