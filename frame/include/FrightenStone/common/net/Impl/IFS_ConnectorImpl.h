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
 * @file  : IFS_ConnectorImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#pragma once

#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Connector_H__
FS_NAMESPACE_BEGIN
inline IFS_Connector::IFS_Connector(IFS_NetEngine *netEngine
    , UInt32 compId
    , Locker &locker
    , Int32 &curSessionCnt
    , Int32 &maxSessionQuantityLimit
    , UInt64 &curMaxSessionId)
    :IFS_EngineComp(netEngine, compId, EngineCompType::Connector)
    ,_pool(NULL)
    , _locker(locker)
    , _curSessionCnt(curSessionCnt)
    , _maxSessionQuantityLimit(maxSessionQuantityLimit)
    , _curMaxSessionId(curMaxSessionId)
    , _cfgs(NULL)
    , _isInit{ false }
{
}

inline void IFS_Connector::_MakeAddrInfo(const FS_String &ip, UInt64 port, FS_String &addrInfo) const
{
    addrInfo.AppendFormat("%s:%hu", ip.c_str(), port);
}

FS_NAMESPACE_END
#endif

