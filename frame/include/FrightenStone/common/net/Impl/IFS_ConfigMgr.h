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
 * @file  : IFS_ConfigMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/09
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_ConfigMgrImpl_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_ConfigMgrImpl_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class FS_IniFile;
class BASE_EXPORT IFS_ConfigMgr
{
    OBJ_POOL_CREATE_DEF(IFS_ConfigMgr);
public:
    static FS_String _bindAnyIp;    // 绑定任意ip:"Any"

public:
    IFS_ConfigMgr();
    virtual ~IFS_ConfigMgr();

public:
    Int32 Init(const Byte8 *iniPath);

    // 读配置
public:
    /* listener */
    void GetListenAddr(std::vector<std::pair<FS_String, UInt16>> &addrInfo);
    Int32 GetMaxSessionQuantityLimit() const;

    /* transfer */
    Int32 GetTransferCnt() const;

    /* dispatcher */
    Int64 GetHeartbeatDeadTimeIntervalMs() const;
    Int32 GetDispatcherCnt() const;
    Int32 GetPrepareBufferCnt() const;
    UInt64 GetMaxAllowAlloctorBytesPerDispatcher() const;

    /* 对象池 */
    UInt64 GetMaxAllowObjPoolBytesOccupied() const;

    /* 内存池最大占用大小 */
    UInt64 GetMaxAllowMemPoolBytesOccupied() const;
    UInt32 GetAcceptorQuantity() const;
    Int64 GetConnectorConnectTimeOutMs() const;
    Int64 GetDispatcherResolutionIntervalMs() const;

    bool GetIsOpenMemoryMonitor() const;

private:
    Int32 _InitDefCfgs();
    void _ReadCfgs();
    void _WriteIniHeaderAnnotation();
    void _ChangeLineBetweenSegs();

private:
    FS_IniFile *_ini;

    std::vector<std::pair<FS_String, UInt16>> _ipPortVec;
    Int32 _maxSessionQuantityLimit;
    
    Int32 _transferCnt;
    Int64 _heartbeatDeadTimeIntervalMs;
    Int32 _prepareBufferCnt;
    UInt64 _maxAlloctorBytesPerDispatcher;

    Int32 _dispatcherCnt;

    UInt64 _maxAllowObjPoolBytesOccupied;
    UInt64 _maxAllowMemPoolBytesOccupied;

    UInt32 _acceptorQuantity;
    Int64 _connectorConnectTimeOutMs;

    Int64 _dispatcherResolutionIntervalMs;

    bool  _isOpenMemoryMonitor;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_ConfigMgrImpl.h"

#endif
