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
 * @file  : FS_AcceptorFactory.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/12/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include <FrightenStone/common/net/Impl/FS_AcceptorFactory.h>
#include <FrightenStone/common/net/Impl/FS_IocpAcceptor.h>
#include <FrightenStone/common/net/Impl/FS_EpollAcceptor.h>

FS_NAMESPACE_BEGIN

IFS_Acceptor *FS_AcceptorFactory::Create(UInt32 compId,
                                         Locker &sessionLocker
                                         , Int32 &curSessionCnt
                                         , Int32 &maxSessionQuantityLimit
                                         , UInt64 &curMaxSessionId
                                         , const AcceptorCfgs &cfg
                                         , IFS_NetEngine *netEngine)
{
#ifdef _WIN32
    return new FS_IocpAcceptor(compId
                               , sessionLocker
                               , curSessionCnt
                               , maxSessionQuantityLimit
                               , curMaxSessionId
                               , cfg
                               , netEngine);
#else
    // TODO:Linux
    return new FS_EpollAcceptor(compId
                               , sessionLocker
                               , curSessionCnt
                               , maxSessionQuantityLimit
                               , curMaxSessionId
                               , cfg
                               , netEngine);
#endif
}

FS_NAMESPACE_END
