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
 * @file  : FS_ClientCfgMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_ClientCfgMgr.h"
#include <FrightenStone/common/component/Impl/File/FS_IniFile.h>
#include <FrightenStone/common/net/Defs/FS_ClientDefaultCfgDef.h>
#include <FrightenStone/common/status/status.h>
#include <FrightenStone/common/log/Log.h>

fs::FS_ClientCfgMgr *g_ClientCfgMgr = NULL;

FS_NAMESPACE_BEGIN

FS_ClientCfgMgr::FS_ClientCfgMgr()
    :_ini(NULL)
{
    g_ClientCfgMgr = this;
}

FS_ClientCfgMgr::~FS_ClientCfgMgr()
{
    Fs_SafeFree(_ini);
}

Int32 FS_ClientCfgMgr::Init(const char *clientCfgPath /*= "./ClientCfg.ini"*/)
{
    _ini = new FS_IniFile;
    _ini->Init(FS_CLIENT_CFG_PATH);

    BUFFER256 cfgs;
    char *ptr = cfgs;
    if(!_ini->HasCfgs(CLIENT_CFG_SEG))
        return _InitDefCfgs();

    _ReadAllCfgs();
    return StatusDefs::Success;
}

FS_String FS_ClientCfgMgr::GetStr(const char *segment, const char *key, const char *defaultStr) const
{
    FS_String str;
    _ini->ReadStr(segment, key, defaultStr, str);
    return str;
}

Int64 FS_ClientCfgMgr::GetInt(const char *segment, const char *key, Int64 defaultInt) const
{
    return _ini->ReadInt(segment, key, defaultInt);
}

void FS_ClientCfgMgr::Lock()
{
    _ini->Lock();
}

void FS_ClientCfgMgr::Unlock()
{
    _ini->Unlock();
}

Int32 FS_ClientCfgMgr::_InitDefCfgs()
{
    // 写入默认配置
    _ini->WriteStr(CLIENT_CFG_SEG, TARGET_IP_TO_CONNECT_KEY, TARGET_IP_TO_CONNECT);
    _ini->WriteStr(CLIENT_CFG_SEG, TARGET_PORT_TO_CONNECT_KEY, TARGET_PORT_TO_CONNECT);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_QUANTITY_KEY, CLIENT_QUANTITY);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_THREAD_QUANTITY_KEY, CLIENT_THREAD_QUANTITY);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_MSG_NUM_PER_PERIOD_KEY, CLIENT_MSG_NUM_PER_PERIOD);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_SEND_PERIOD_KEY, CLIENT_SEND_PERIOD);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_SEND_BUFFER_SIZE_KEY, CLIENT_SEND_BUFFER_SIZE);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_RECV_BUFFER_SIZE_KEY, CLIENT_RECV_BUFFER_SIZE);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_CHECK_MSG_ID_KEY, CLIENT_CHECK_MSG_ID);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_MEMPOOL_BUFFER_CNT_INIT_KEY, CLIENT_MEMPOOL_BUFFER_CNT_INIT);
    _ini->WriteStr(CLIENT_CFG_SEG, CLIENT_MEMPOOL_ALLOC_MAX_BYTES_KEY, CLIENT_MEMPOOL_ALLOC_MAX_BYTES);

    // 检查是否写入正确
    FS_String result;
    _ini->ReadStr(CLIENT_CFG_SEG, TARGET_IP_TO_CONNECT_KEY, "", result);
    if(result != TARGET_IP_TO_CONNECT)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail target ip not match result[%s] def[%s]")
                                  , result.c_str(), TARGET_IP_TO_CONNECT);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, TARGET_PORT_TO_CONNECT_KEY, "", result);
    if(result != TARGET_PORT_TO_CONNECT)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail target port not match result[%s] default[%s]")
                                  , result.c_str(), TARGET_PORT_TO_CONNECT);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_QUANTITY_KEY, "", result);
    if(result != CLIENT_QUANTITY)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail client quantity not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_QUANTITY);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_THREAD_QUANTITY_KEY, "", result);
    if(result != CLIENT_THREAD_QUANTITY)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail client thread quantity not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_THREAD_QUANTITY);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_MSG_NUM_PER_PERIOD_KEY, "", result);
    if(result != CLIENT_MSG_NUM_PER_PERIOD)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_MSG_NUM_PER_PERIOD not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_MSG_NUM_PER_PERIOD);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_SEND_PERIOD_KEY, "", result);
    if(result != CLIENT_SEND_PERIOD)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_SEND_PERIOD not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_SEND_PERIOD);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_SEND_BUFFER_SIZE_KEY, "", result);
    if(result != CLIENT_SEND_BUFFER_SIZE)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_SEND_BUFFER_SIZE not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_SEND_BUFFER_SIZE);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_RECV_BUFFER_SIZE_KEY, "", result);
    if(result != CLIENT_RECV_BUFFER_SIZE)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_RECV_BUFFER_SIZE not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_RECV_BUFFER_SIZE);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_CHECK_MSG_ID_KEY, "", result);
    if(result != CLIENT_CHECK_MSG_ID)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_CHECK_MSG_ID not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_CHECK_MSG_ID);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY, "", result);
    if(result != CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_SEND_WHEN_SVR_RES_ARRIVE not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_MEMPOOL_BUFFER_CNT_INIT_KEY, "", result);
    if(result != CLIENT_MEMPOOL_BUFFER_CNT_INIT)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_MEMPOOL_BUFFER_CNT_INIT not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_MEMPOOL_BUFFER_CNT_INIT);
        return StatusDefs::Failed;
    }

    _ini->ReadStr(CLIENT_CFG_SEG, CLIENT_MEMPOOL_ALLOC_MAX_BYTES_KEY, "", result);
    if(result != CLIENT_MEMPOOL_ALLOC_MAX_BYTES)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_MEMPOOL_ALLOC_MAX_BYTES not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_MEMPOOL_ALLOC_MAX_BYTES);
        return StatusDefs::Failed;
    }

    return StatusDefs::Success;
}

void FS_ClientCfgMgr::_ReadAllCfgs()
{
    FS_String result;
    _ini->ReadStr(CLIENT_CFG_SEG, TARGET_IP_TO_CONNECT_KEY, "", _ip);
    _port = static_cast<UInt16>(_ini->ReadUInt(CLIENT_CFG_SEG, TARGET_PORT_TO_CONNECT_KEY, 0));
    _clientQuantity = static_cast<Int32>(_ini->ReadInt(CLIENT_CFG_SEG, CLIENT_QUANTITY_KEY, 0));
    _threadQuantity = static_cast<Int32>(_ini->ReadInt(CLIENT_CFG_SEG, CLIENT_THREAD_QUANTITY_KEY, 0));
    _msgNumPerPeriod = static_cast<Int32>(_ini->ReadInt(CLIENT_CFG_SEG, CLIENT_MSG_NUM_PER_PERIOD_KEY, 0));
    _sendPeriod = static_cast<Int32>(_ini->ReadInt(CLIENT_CFG_SEG, CLIENT_SEND_PERIOD_KEY, 0));
    _sendBufferSize = _ini->ReadUInt(CLIENT_CFG_SEG, CLIENT_SEND_BUFFER_SIZE_KEY, 0);
    _recvBufferSize = _ini->ReadUInt(CLIENT_CFG_SEG, CLIENT_RECV_BUFFER_SIZE_KEY, 0);
    _checkMsgId = _ini->ReadInt(CLIENT_CFG_SEG, CLIENT_CHECK_MSG_ID_KEY, 0) != 0;
    _isSendAfterSvrResArrive = _ini->ReadInt(CLIENT_CFG_SEG, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY, 0) != 0;
    _memPoolBufferCntInit = static_cast<Int32>(_ini->ReadInt(CLIENT_CFG_SEG, CLIENT_MEMPOOL_BUFFER_CNT_INIT_KEY, 0));
    _memPoolAllocMaxBytes = _ini->ReadUInt(CLIENT_CFG_SEG, CLIENT_MEMPOOL_ALLOC_MAX_BYTES_KEY, 0);
}

FS_NAMESPACE_END
