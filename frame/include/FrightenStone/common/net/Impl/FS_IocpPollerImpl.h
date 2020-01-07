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
 * @file  : FS_IocpPollerImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/29
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpPoller_H__
#pragma once
#ifdef _WIN32

FS_NAMESPACE_BEGIN

inline FS_IocpPoller::FS_IocpPoller(IFS_EngineComp *engineComp
                                    , Int32 monitorType)
    :_engineComp(engineComp)
    ,_mainType(PollerDefs::MainType_Iocp)
    ,_monitorType(monitorType)
    ,_acceptorSock(INVALID_SOCKET)
    ,_pool(NULL)
    ,_iocp(NULL)
    ,_ioEv(NULL)
    ,_quitIocpMonitor(NULL)
    ,_mq(NULL)
    ,_isInit(false)
    , _locker(NULL)
    , _curSessionCnt(NULL)
    ,_maxSessionQuantityLimit(0)
    ,_curMaxSessionId(NULL)
{
}

inline FS_IocpPoller::~FS_IocpPoller()
{
    BeforeClose();
    Close();
}

inline void FS_IocpPoller::AttachMessageQueue(ConcurrentMessageQueueNoThread *mq)
{
    _mq = mq;
}

inline void FS_IocpPoller::AttachAcceptorParam(SOCKET sock
                                               , Int32 maxSessionQuantityLimit
                                               , Locker *locker
                                               , Int32 *curSessionCnt
                                               , UInt64 *curMaxSessionId)
{
    _acceptorSock = sock;
    _maxSessionQuantityLimit = maxSessionQuantityLimit;
    _locker = locker;
    _curSessionCnt = curSessionCnt;
    _curMaxSessionId = curMaxSessionId;
}

FS_NAMESPACE_END

#endif

#endif
