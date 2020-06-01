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
 * @file  : IFS_AcceptorImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/4/17
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Acceptor_H__
#pragma once

FS_NAMESPACE_BEGIN
inline IFS_Acceptor::IFS_Acceptor(UInt32 compId, IFS_NetEngine *netEngine
                                  , Locker &locker
                                  , Int32 &curSessionCnt
                                  , Int32 &maxSessionQuantityLimit
                                  , UInt64 &curMaxSessionId
                                  , const AcceptorCfgs &cfg)
    :IFS_EngineComp(netEngine, compId, EngineCompType::Acceptor)
    ,_sock(INVALID_SOCKET)
    ,_locker(locker)
    ,_curSessionCnt(curSessionCnt)
    ,_maxSessionQuantityLimit(maxSessionQuantityLimit)
    ,_curMaxSessionId(curMaxSessionId)
    ,_cfgs(NULL)
{
    _cfgs = new AcceptorCfgs;
    *_cfgs = cfg;
}

inline UInt16 IFS_Acceptor::GetListenPort() const
{
    return _cfgs->_port;
}

FS_NAMESPACE_END

#endif
