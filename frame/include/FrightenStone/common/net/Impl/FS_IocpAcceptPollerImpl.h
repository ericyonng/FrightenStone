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
 * @file  : FS_IocpAcceptPollerImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/17
 * @brief :
 */

#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpAcceptPoller_H__

#ifdef _WIN32
#pragma once

FS_NAMESPACE_BEGIN

inline FS_IocpAcceptPoller::FS_IocpAcceptPoller(IFS_EngineComp *engineComp)
    :IFS_AcceptorPoller(engineComp, PollerDefs::MonitorType_IocpAcceptor)
    , _pool(NULL)
    , _iocp(NULL)
    , _ioEv(NULL)
    , _quitIocpMonitor(NULL)
    , _isInit(false)
{
}

inline FS_IocpAcceptPoller::~FS_IocpAcceptPoller()
{
    BeforeClose();
    Close();
}

FS_NAMESPACE_END

#endif

#endif
