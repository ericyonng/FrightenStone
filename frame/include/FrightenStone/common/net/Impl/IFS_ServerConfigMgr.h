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
 * @file  : IFS_ServerConfigMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_ServerConfigMgr_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_ServerConfigMgr_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/FS_String.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_IniFile;
class BASE_EXPORT IFS_ServerConfigMgr
{
public:
    IFS_ServerConfigMgr();
    virtual ~IFS_ServerConfigMgr();

public:
    Int32 Init();

    // 读配置
public:
    /* listener */
    FS_String GetListenIp() const;
    UInt16 GetListenPort() const;
    Int32 GetMaxConnectQuantityLimit() const;

    /* transfer */
    Int32 GetTransferCnt() const;
    Int32 GetHeartbeatDeadTimeInterval() const;
    Int32 GetPrepareBufferPoolCnt() const;
    UInt64 GetMaxMemPoolBytesPerTransfer() const;

    /* dispatcher */
    Int32 GetDispatcherCnt() const;

    /* 对象池 */
    UInt64 GetMaxAllowObjPoolBytesOccupied() const;

    /* 内存池最大占用大小 */
    UInt64 GetMaxAllowMemoryPoolBytesOccupied() const;

private:
    Int32 _InitDefCfgs();
    void _ReadCfgs();

private:
    FS_IniFile *_ini;

    FS_String _ip;
    UInt16 _port;
    Int32 _maxConnectQuantityLimit;
    
    Int32 _transferCnt;
    Int32 _heartbeatDeadTimeInterval;
    Int32 _prepareBufferPoolCnt;
    UInt64 _maxMemPoolBytesPerTransfer;

    Int32 _dispatcherCnt;

    UInt64 _maxAllowObjPoolBytesOccupied;
    UInt64 _maxAllowMemPoolBytesOccupied;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_ServerConfigMgrImpl.h"

#endif
