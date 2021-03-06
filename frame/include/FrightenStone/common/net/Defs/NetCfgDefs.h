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
 * @file  : NetCfgDefs.h
 * @author: Eric Yonng<120453674@qq.com>
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
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

struct BASE_EXPORT CommonCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(CommonCfgs);

    CommonCfgs();
    Int32 _maxSessionQuantityLimit;                     // 最大连接数
    UInt32 _acceptorQuantityLimit;                      // 监听端口数量
    UInt32 _transferQuantity;                           // 数据传输器数量
    UInt32 _dispatcherQuantity;                         // 业务层消息分配器数量
    bool _isOpenMemoryMonitor;                          // 是否开启内存监控
    UInt32 _memoryMonitorPrintIntervalSeconds;          // 内存监控打印日志的秒数
    UInt16 _serviceNodePort;                            // 提供服务的端口
    UInt16 _realUserPort;                               // 真实用户连接的端口
};

struct BASE_EXPORT ConnectorCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(ConnectorCfgs);
    ConnectorCfgs();
    Int64 _connectTimeOutMs;                             // 连接超时时间
};

struct BASE_EXPORT AcceptorCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(AcceptorCfgs);
    FS_String _ip;
    UInt16 _port;
};

struct BASE_EXPORT TransferCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(TransferCfgs);
    TransferCfgs();
};

struct BASE_EXPORT DispatcherCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(DispatcherCfgs);
    DispatcherCfgs();
    Int64 _heartbeatDeadTimeMsInterval;                  // 每个session的心跳间隔时间 单位ms
    Int32 _prepareBufferPoolCnt;                         // 预创建的buffer数量
    UInt64 _maxAlloctorBytesPerDispatcher;               // 每个业务层消息缓冲 分配器最大占用
    TimeSlice _dispatcherResolutionInterval;             // 消息分配器的时间轮盘精度 microseconds 默认1ms
    Int32 _logicServiceId;                               // 服务号 TODO:本结点的服务内容
};

struct BASE_EXPORT ObjPoolCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(ObjPoolCfgs);
    ObjPoolCfgs();
    UInt64 _maxAllowObjPoolBytesOccupied;               // 对象池最大占用内存空间
};

struct BASE_EXPORT MempoolCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(MempoolCfgs);
    MempoolCfgs();
    UInt64 _maxAllowMemPoolBytesOccupied;               // 内存池最大占用内存空间
};

struct BASE_EXPORT NetEngineTotalCfgs
{
    OBJ_POOL_CREATE_ANCESTOR(NetEngineTotalCfgs);
    CommonCfgs _commonCfgs;
    ConnectorCfgs _connectorCfgs;
    AcceptorCfgs *_acceptorCfgs;        // 监视器是一组配置,数量配置在CommonCfgs中
    TransferCfgs _transferCfgs;
    DispatcherCfgs _dispatcherCfgs;
    ObjPoolCfgs _objPoolCfgs;
    MempoolCfgs _mempoolCfgs;
};

FS_NAMESPACE_END

#endif

