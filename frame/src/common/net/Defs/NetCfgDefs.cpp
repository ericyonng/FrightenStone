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
 * @file  : NetCfgDefs.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/17
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"

#include "FrightenStone/common/assist/utils/Impl/StringUtil.h"
#include "FrightenStone/common/component/Impl/Time.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(CommonCfgs, 16);
OBJ_POOL_CREATE_DEF_IMPL(ConnectorCfgs, 16);
OBJ_POOL_CREATE_DEF_IMPL(AcceptorCfgs, 16);
OBJ_POOL_CREATE_DEF_IMPL(TransferCfgs, 16);
OBJ_POOL_CREATE_DEF_IMPL(DispatcherCfgs, 16);
OBJ_POOL_CREATE_DEF_IMPL(ObjPoolCfgs, 16);
OBJ_POOL_CREATE_DEF_IMPL(MempoolCfgs, 16);
OBJ_POOL_CREATE_DEF_IMPL(NetEngineTotalCfgs, 16);

CommonCfgs::CommonCfgs()
    :_maxSessionQuantityLimit(StringUtil::StringToInt32(SVR_CFG_COMMONCFG_SESSION_QUANTITY_LIMIT))
    , _acceptorQuantityLimit(0)
    , _transferQuantity(StringUtil::StringToUInt32(SVR_CFG_COMMONCFG_TRANSFER_QUANTITY))
    , _dispatcherQuantity(StringUtil::StringToUInt32(SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY))
    ,_isOpenMemoryMonitor(StringUtil::StringToInt32(SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR))
    ,_memoryMonitorPrintIntervalSeconds(StringUtil::StringToUInt32(SVR_CFG_COMMONCFG_MEMORY_MONITOR_INTERVAL_SEC))
    , _serviceNodePort(static_cast<UInt16>(StringUtil::StringToUInt32(SVR_CFG_COMMONCFG_SERVICE_NODE_PORT)))
    , _realUserPort(static_cast<UInt16>(StringUtil::StringToUInt32(SVR_CFG_COMMONCFG_REAL_USER_PORT)))
{
}

ConnectorCfgs::ConnectorCfgs()
    :_connectTimeOutMs(StringUtil::StringToInt64(SVR_CFG_CONNECTOR_CONNECT_TIME_OUT))
{
}

TransferCfgs::TransferCfgs()
{
}

DispatcherCfgs::DispatcherCfgs()
    :_heartbeatDeadTimeMsInterval(StringUtil::StringToInt64(SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL))
    , _prepareBufferPoolCnt(StringUtil::StringToInt32(SVR_CFG_PREPARE_POOL_BUFFER_CNT))
    , _maxAlloctorBytesPerDispatcher(StringUtil::StringToUInt64(SVR_CFG_MAX_MEMPOOL_MB_PER_DISPATCHER)*1024*1024)
    ,_dispatcherResolutionInterval(0, StringUtil::StringToInt64(SVR_CFG_DISPATCHER_RESOLUTION_INTERVAL), 0)
{
}

ObjPoolCfgs::ObjPoolCfgs()
    :_maxAllowObjPoolBytesOccupied(StringUtil::StringToUInt64(SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED)*1024*1024)
{
}

MempoolCfgs::MempoolCfgs()
    :_maxAllowMemPoolBytesOccupied(StringUtil::StringToUInt64(SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED)*1024*1024)
{
}

FS_NAMESPACE_END
