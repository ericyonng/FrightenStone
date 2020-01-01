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
 * @file  : IFS_ServerConfigMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_ServerConfigMgr.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/net/Impl/FS_NetEngine.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_CpuInfo.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/component/Impl/File/FS_IniFile.h"


FS_NAMESPACE_BEGIN

IFS_ServerConfigMgr::IFS_ServerConfigMgr()
    :_ini(NULL)
    ,_port(0)
    ,_maxSessionQuantityLimit(0)
    ,_transferCnt(0)
    ,_heartbeatDeadTimeIntervalMs(0)
    ,_prepareBufferCnt(0)
    ,_maxAlloctorBytesPerTransfer(0)
    ,_dispatcherCnt(0)
    ,_maxAllowObjPoolBytesOccupied(0)
    ,_maxAllowMemPoolBytesOccupied(0)
    ,_acceptorQuantity(0)
    ,_connectorConnectTimeOutMs(0)
    ,_dispatcherResolutionIntervalMs(0)
    ,_isOpenMemoryMonitor(false)
{

}

IFS_ServerConfigMgr::~IFS_ServerConfigMgr()
{
    Fs_SafeFree(_ini);
}

Int32 IFS_ServerConfigMgr::Init()
{
    _ini = new FS_IniFile;
    _ini->Init(SVR_CONFIG_PATH_NAME);

    if(!_ini->HasCfgs(SVR_CFG_COMMONCFG_SEG))
    {
        // 初始化默认配置
        _InitDefCfgs();
        // 段与段之间需要换行
        _ChangeLineBetweenSegs();
    }

    _ReadCfgs();

    return StatusDefs::Success;
}

Int32 IFS_ServerConfigMgr::_InitDefCfgs()
{
    #pragma region listener
    // ip
    _ini->WriteStr(SVR_CFG_ACCEPTOR_SEG, SVR_CFG_ACCEPTOR_IP_KEY, SVR_CFG_ACCEPTOR_IP);
    // port
    _ini->WriteStr(SVR_CFG_ACCEPTOR_SEG, SVR_CFG_ACCEPTOR_PORT_KEY, SVR_CFG_ACCEPTOR_PORT);
    // 最大连接数
    _ini->WriteStr(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_SESSION_QUANTITY_LIMIT_KEY, SVR_CFG_COMMONCFG_SESSION_QUANTITY_LIMIT);
    #pragma endregion

    #pragma region transfer
    // 数据传输线程数
    _ini->WriteStr(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_TRANSFER_QUANTITY_KEY, SVR_CFG_COMMONCFG_TRANSFER_QUANTITY);
    _ini->WriteStr(SVR_CFG_TRANSFER_SEG, SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL_KEY, SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL);
    _ini->WriteStr(SVR_CFG_TRANSFER_SEG, SVR_CFG_PREPARE_POOL_BUFFER_CNT_KEY, SVR_CFG_PREPARE_POOL_BUFFER_CNT);
    _ini->WriteStr(SVR_CFG_TRANSFER_SEG, SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER_KEY, SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER);
    #pragma endregion

    #pragma region dispatcher
    _ini->WriteStr(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY_KEY, SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY);
    _ini->WriteStr(SVR_CFG_DISPATCHER_SEG, SVR_CFG_DISPATCHER_RESOLUTION_INTERVAL_KEY, SVR_CFG_DISPATCHER_RESOLUTION_INTERVAL);
    #pragma endregion

    #pragma region acceptor
    _ini->WriteStr(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY_KEY, SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY);
    #pragma endregion

    #pragma region connector
    _ini->WriteStr(SVR_CFG_CONNECTOR_SEG, SVR_CFG_CONNECTOR_CONNECT_TIME_OUT_KEY, SVR_CFG_CONNECTOR_CONNECT_TIME_OUT);
    #pragma endregion

    #pragma region objpool
    _ini->WriteStr(SVR_CFG_OBJPOOL_SEG, SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED_KEY, SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED);
    #pragma endregion

    #pragma region memorypool
    _ini->WriteStr(SVR_CFG_MEMORY_POOL_SEG, SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED_KEY, SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED);
    #pragma endregion

    #pragma region memory monitor
    _ini->WriteStr(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR_KEY, SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR);
    #pragma endregion

    // 检查是否写入正确
    FS_String buffer;
    _ini->ReadStr(SVR_CFG_ACCEPTOR_SEG, SVR_CFG_ACCEPTOR_IP_KEY, "", buffer);
    if(strcmp(buffer.c_str(), "127.0.0.1") != 0)
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail ip not match"));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    UInt16 port = static_cast<UInt16>(_ini->ReadInt(SVR_CFG_ACCEPTOR_SEG, SVR_CFG_ACCEPTOR_PORT_KEY, 0));
    if(port != atoi(SVR_CFG_ACCEPTOR_PORT))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail port not match"));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto maxConnectQuantity = _ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_SESSION_QUANTITY_LIMIT_KEY, 0);
    if(maxConnectQuantity != atoi(SVR_CFG_COMMONCFG_SESSION_QUANTITY_LIMIT))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_COMMONCFG_SESSION_QUANTITY_LIMIT not match"));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto cnt = _ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_TRANSFER_QUANTITY_KEY, 0);
    if(cnt != atoi(SVR_CFG_COMMONCFG_TRANSFER_QUANTITY))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail transfer cnt not match"));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto heartbeatTime = _ini->ReadInt(SVR_CFG_TRANSFER_SEG, SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL_KEY, 0);
    if(heartbeatTime != atoll(SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL[%lld] not match SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL[%u] default")
                                      , heartbeatTime, static_cast<UInt32>(atoi(SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto preBufferCnt = _ini->ReadInt(SVR_CFG_TRANSFER_SEG, SVR_CFG_PREPARE_POOL_BUFFER_CNT_KEY, 0);
    if(preBufferCnt != atoi(SVR_CFG_PREPARE_POOL_BUFFER_CNT))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_PREPARE_POOL_BUFFER_CNT[%lld] not match SVR_CFG_PREPARE_POOL_BUFFER_CNT[%u] default")
                                      , preBufferCnt, static_cast<UInt32>(atoi(SVR_CFG_PREPARE_POOL_BUFFER_CNT)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto memPoolMBPerTransfer = _ini->ReadInt(SVR_CFG_TRANSFER_SEG, SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER_KEY, 0);
    if(memPoolMBPerTransfer != atoi(SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER[%lld] not match SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER[%u] default")
                                      , memPoolMBPerTransfer, static_cast<UInt32>(atoi(SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto dispatcherCnt = _ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY_KEY, 0);
    if(dispatcherCnt != atoi(SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY[%lld] not match SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY[%u] default")
                                      , dispatcherCnt, static_cast<UInt32>(atoi(SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto maxAllowMB = _ini->ReadInt(SVR_CFG_OBJPOOL_SEG, SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED_KEY, 0);
    if(maxAllowMB != atoi(SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED[%lld] not match SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED[%u] default")
                                      , maxAllowMB, static_cast<UInt32>(atoi(SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto memPoolMaxAllowMB = _ini->ReadInt(SVR_CFG_MEMORY_POOL_SEG, SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED_KEY, 0);
    if(memPoolMaxAllowMB != atoi(SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED[%lld] not match SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED[%u] default")
                                      , memPoolMaxAllowMB, static_cast<UInt32>(atoi(SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto acceptorQuatity = _ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY_KEY, 0);
    if(acceptorQuatity != atoi(SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY[%lld] not match SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY[%u] default")
                                      , acceptorQuatity, static_cast<UInt32>(atoi(SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    auto isOpenMonitor = _ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR_KEY, 0);
    if(isOpenMonitor != atoi(SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR))
    {
        g_Log->e<IFS_ServerConfigMgr>(_LOGFMT_("_InitDefCfgs fail SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR[%lld] not match SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR[%u] default")
                                      , isOpenMonitor, static_cast<UInt32>(atoi(SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR)));
        return StatusDefs::IocpAcceptor_InitDefIniFail;
    }

    return StatusDefs::Success;
}

void IFS_ServerConfigMgr::_ReadCfgs()
{
    _ini->ReadStr(SVR_CFG_ACCEPTOR_SEG, SVR_CFG_ACCEPTOR_IP_KEY, "",_ip);
    _port = static_cast<UInt16>(_ini->ReadInt(SVR_CFG_ACCEPTOR_SEG, SVR_CFG_ACCEPTOR_PORT_KEY, 0));
    _maxSessionQuantityLimit = static_cast<Int32>(_ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_SESSION_QUANTITY_LIMIT_KEY, 0));

    _transferCnt = static_cast<Int32>(_ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_TRANSFER_QUANTITY_KEY, 0));
    _heartbeatDeadTimeIntervalMs = static_cast<Int64>(_ini->ReadInt(SVR_CFG_TRANSFER_SEG, SVR_CFG_HEARTBEAT_DEAD_TIME_INTERVAL_KEY, 0));
    _prepareBufferCnt = static_cast<Int32>(_ini->ReadInt(SVR_CFG_TRANSFER_SEG, SVR_CFG_PREPARE_POOL_BUFFER_CNT_KEY, 0));
    _maxAlloctorBytesPerTransfer = static_cast<UInt64>((_ini->ReadInt(SVR_CFG_TRANSFER_SEG, SVR_CFG_MAX_MEMPOOL_MB_PER_TRANSFER_KEY, 0))) * 1024 * 1024;

    _dispatcherCnt = static_cast<Int32>(_ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_DISPATCHER_QUANTITY_KEY, 0));

    _maxAllowObjPoolBytesOccupied = static_cast<UInt64>(_ini->ReadInt(SVR_CFG_OBJPOOL_SEG, SVR_CFG_MAX_ALLOW_OBJPOOL_MB_OCCUPIED_KEY, 0)) * 1024 * 1024;
    _maxAllowMemPoolBytesOccupied = static_cast<UInt64>(_ini->ReadInt(SVR_CFG_MEMORY_POOL_SEG, SVR_CFG_MAX_ALLOW_MEMPOOL_MB_OCCUPIED_KEY, 0)) * 1024 * 1024;

    _acceptorQuantity = static_cast<UInt32>(_ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_ACCEPTOR_QUANTITY_KEY, 0));
    _connectorConnectTimeOutMs = static_cast<Int64>(_ini->ReadInt(SVR_CFG_CONNECTOR_SEG, SVR_CFG_CONNECTOR_CONNECT_TIME_OUT_KEY, 0));
    
    _dispatcherResolutionIntervalMs = static_cast<Int64>(_ini->ReadInt(SVR_CFG_DISPATCHER_SEG, SVR_CFG_DISPATCHER_RESOLUTION_INTERVAL_KEY, 0));
    _isOpenMemoryMonitor = static_cast<bool>(_ini->ReadInt(SVR_CFG_COMMONCFG_SEG, SVR_CFG_COMMONCFG_IS_OPEN_MEMORY_MONITOR_KEY, 0));
}

void fs::IFS_ServerConfigMgr::_ChangeLineBetweenSegs()
{
    _ini->ChangeLineBetweenSegs();
}

FS_NAMESPACE_END


