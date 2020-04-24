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
 * @file  : FS_EpollPollerImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/18
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollPoller_H__

#pragma once

#ifndef _WIN32

FS_NAMESPACE_BEGIN
inline FS_EpollPoller::FS_EpollPoller(IFS_EngineComp *engineComp, Int32 monitorType)
    : IFS_BasePoller(engineComp, monitorType)
    ,_mainType(PollerDefs::MainType_Epoll)
    ,_pool(NULL)
    ,_epoll(NULL)
    ,_innerEvThread2MsgPreHandleMq(NULL)
    ,_isInit{false}
    ,_cpuInfo(NULL)
    ,_maxThreadCnt(0)
    ,_isEventArrived{false}
    ,_recverMailBox(NULL)
    ,_senderMailBox(NULL)
    , _generatorId(0)
    ,_isWillClose{false}
{

}

inline FS_EpollPoller::~FS_EpollPoller()
{
    BeforeClose();
    Close();
}

inline FS_Epoll *FS_EpollPoller::GetEpollObj()
{
    return _epoll;
}

FS_NAMESPACE_END

#endif

#endif
