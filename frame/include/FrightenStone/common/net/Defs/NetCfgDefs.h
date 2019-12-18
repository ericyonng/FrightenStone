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
 * @file  : NetCfgDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/16
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_NetCfgDefs_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_NetCfgDefs_H__

#pragma once

#include <FrightenStone/exportbase.h>
#include <FrightenStone/common/basedefs/BaseDefs.h>
#include <FrightenStone/common/component/Impl/TimeSlice.h>

FS_NAMESPACE_BEGIN

struct BASE_EXPORT CommonCfgs
{
    CommonCfgs();
    Int32 _maxSessionQuantityLimit;                     // 最大连接数
    UInt32 _acceptorQuantityLimit;                      // 监听端口数量
    UInt32 _transferQuantity;                           // 数据传输器数量
    UInt32 _dispatcherQuantity;                         // 业务层消息分配器数量
};

struct BASE_EXPORT ConnectorCfgs
{
    ConnectorCfgs();
    Int64 _connectTimeOutMs;                             // 连接超时时间
};

struct BASE_EXPORT AcceptorCfgs
{
    FS_String _ip;
    UInt16 _port;
};

struct BASE_EXPORT TransferCfgs
{
    TransferCfgs();
    Int64 _heartbeatDeadTimeMsInterval;                  // 每个session的心跳间隔时间 单位ms
    Int32 _prepareBufferPoolCnt;                         // 预创建的buffer数量
    UInt64 _maxAlloctorBytesPerTransfer;                 // 每个收发器buffer 分配器最大占用
};

struct BASE_EXPORT DispatcherCfgs
{
    DispatcherCfgs();
    TimeSlice _dispatcherResolutionInterval;                // 消息分配器的时间轮盘精度 microseconds 默认1ms
};

struct BASE_EXPORT ObjPoolCfgs
{
    ObjPoolCfgs();
    UInt64 _maxAllowObjPoolBytesOccupied;               // 对象池最大占用内存空间
};

struct BASE_EXPORT MempoolCfgs
{
    MempoolCfgs();
    UInt64 _maxAllowMemPoolBytesOccupied;               // 内存池最大占用内存空间
};

struct BASE_EXPORT NetEngineTotalCfgs
{
    CommonCfgs _commonCfgs;
    ConnectorCfgs _connectorCfgs;
    AcceptorCfgs _acceptorCfgs;
    TransferCfgs _transferCfgs;
    DispatcherCfgs _dispatcherCfgs;
    ObjPoolCfgs _objPoolCfgs;
    MempoolCfgs _mempoolCfgs;
};

FS_NAMESPACE_END

#endif

