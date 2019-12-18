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
 * @file  : IFS_ServerConfigMgrImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/17
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_ServerConfigMgr_H__

#pragma once

FS_NAMESPACE_BEGIN

inline FS_String IFS_ServerConfigMgr::GetListenIp() const
{
    return _ip;
}

inline UInt16 IFS_ServerConfigMgr::GetListenPort() const
{
    return _port;
}

inline Int32 IFS_ServerConfigMgr::GetMaxSessionQuantityLimit() const
{
    return _maxSessionQuantityLimit;
}

inline Int32 IFS_ServerConfigMgr::GetTransferCnt() const
{
    return _transferCnt;
}

inline Int64 IFS_ServerConfigMgr::GetHeartbeatDeadTimeIntervalMs() const
{
    return _heartbeatDeadTimeIntervalMs;
}

inline Int32 IFS_ServerConfigMgr::GetPrepareBufferCnt() const
{
    return _prepareBufferCnt;
}

inline UInt64 IFS_ServerConfigMgr::GetMaxAllowAlloctorBytesPerTransfer() const
{
    return _maxAlloctorBytesPerTransfer;
}

inline Int32 IFS_ServerConfigMgr::GetDispatcherCnt() const
{
    return _dispatcherCnt;
}

inline UInt64 IFS_ServerConfigMgr::GetMaxAllowObjPoolBytesOccupied() const
{
    return _maxAllowObjPoolBytesOccupied;
}

inline UInt64 IFS_ServerConfigMgr::GetMaxAllowMemPoolBytesOccupied() const
{
    return _maxAllowMemPoolBytesOccupied;
}

inline UInt32 IFS_ServerConfigMgr::GetAcceptorQuantity() const
{
    return _acceptorQuantity;
}

inline Int64 IFS_ServerConfigMgr::GetConnectorConnectTimeOutMs() const
{
    return _connectorConnectTimeOutMs;
}

inline Int64 IFS_ServerConfigMgr::GetDispatcherResolutionIntervalMs() const
{
    return _dispatcherResolutionIntervalMs;
}

FS_NAMESPACE_END

#endif
