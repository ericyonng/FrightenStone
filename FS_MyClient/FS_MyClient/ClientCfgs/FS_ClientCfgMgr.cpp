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
#include "FS_MyClient/FS_MyClient/ClientCfgs/FS_ClientCfgMgr.h"
#include <FrightenStone/common/component/Impl/File/FS_IniFile.h>
#include "FS_MyClient/FS_MyClient/ClientCfgs/Defs/FS_ClientDefaultCfgDef.h"

FS_ClientCfgMgr *g_ClientCfgMgr = NULL;

FS_ClientCfgMgr::FS_ClientCfgMgr()
{
    g_ClientCfgMgr = this;
}

FS_ClientCfgMgr::~FS_ClientCfgMgr()
{
}

Int32 FS_ClientCfgMgr::_InitCustomCfgs(fs::FS_IniFile *iniOperator)
{
    // 写入默认配置
    iniOperator->WriteStr(CLIENT_CFG_SEG, TARGET_IP_TO_CONNECT_KEY, TARGET_IP_TO_CONNECT);
    iniOperator->WriteStr(CLIENT_CFG_SEG, TARGET_PORT_TO_CONNECT_KEY, TARGET_PORT_TO_CONNECT);
    iniOperator->WriteStr(CLIENT_CFG_SEG, CLIENT_QUANTITY_KEY, CLIENT_QUANTITY);
    iniOperator->WriteStr(CLIENT_CFG_SEG, CLIENT_MSG_NUM_PER_PERIOD_KEY, CLIENT_MSG_NUM_PER_PERIOD);
    iniOperator->WriteStr(CLIENT_CFG_SEG, CLIENT_SEND_PERIOD_KEY, CLIENT_SEND_PERIOD);
    iniOperator->WriteStr(CLIENT_CFG_SEG, CLIENT_CHECK_MSG_ID_KEY, CLIENT_CHECK_MSG_ID);
    iniOperator->WriteStr(CLIENT_CFG_SEG, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE);

    // 检查是否写入正确
    fs::FS_String result;
    iniOperator->ReadStr(CLIENT_CFG_SEG, TARGET_IP_TO_CONNECT_KEY, "", result);
    if(result != TARGET_IP_TO_CONNECT)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail target ip not match result[%s] def[%s]")
                                  , result.c_str(), TARGET_IP_TO_CONNECT);
        return StatusDefs::Failed;
    }

    iniOperator->ReadStr(CLIENT_CFG_SEG, TARGET_PORT_TO_CONNECT_KEY, "", result);
    if(result != TARGET_PORT_TO_CONNECT)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail target port not match result[%s] default[%s]")
                                  , result.c_str(), TARGET_PORT_TO_CONNECT);
        return StatusDefs::Failed;
    }

    iniOperator->ReadStr(CLIENT_CFG_SEG, CLIENT_QUANTITY_KEY, "", result);
    if(result != CLIENT_QUANTITY)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail client quantity not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_QUANTITY);
        return StatusDefs::Failed;
    }

    iniOperator->ReadStr(CLIENT_CFG_SEG, CLIENT_MSG_NUM_PER_PERIOD_KEY, "", result);
    if(result != CLIENT_MSG_NUM_PER_PERIOD)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_MSG_NUM_PER_PERIOD not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_MSG_NUM_PER_PERIOD);
        return StatusDefs::Failed;
    }

    iniOperator->ReadStr(CLIENT_CFG_SEG, CLIENT_SEND_PERIOD_KEY, "", result);
    if(result != CLIENT_SEND_PERIOD)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_SEND_PERIOD not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_SEND_PERIOD);
        return StatusDefs::Failed;
    }

    iniOperator->ReadStr(CLIENT_CFG_SEG, CLIENT_CHECK_MSG_ID_KEY, "", result);
    if(result != CLIENT_CHECK_MSG_ID)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_CHECK_MSG_ID not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_CHECK_MSG_ID);
        return StatusDefs::Failed;
    }

    iniOperator->ReadStr(CLIENT_CFG_SEG, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY, "", result);
    if(result != CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE)
    {
        g_Log->e<FS_ClientCfgMgr>(_LOGFMT_("_InitDefCfgs fail CLIENT_SEND_WHEN_SVR_RES_ARRIVE not match result[%s] default[%s]")
                                  , result.c_str(), CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE);
        return StatusDefs::Failed;
    }

    return StatusDefs::Success;
}

void FS_ClientCfgMgr::_ReadCustomCfgs(fs::FS_IniFile *iniOperator)
{
    iniOperator->ReadStr(CLIENT_CFG_SEG, TARGET_IP_TO_CONNECT_KEY, "", _ip);
    _port = static_cast<UInt16>(iniOperator->ReadUInt(CLIENT_CFG_SEG, TARGET_PORT_TO_CONNECT_KEY, 0));
    _clientQuantity = static_cast<Int32>(iniOperator->ReadInt(CLIENT_CFG_SEG, CLIENT_QUANTITY_KEY, 0));
    _msgNumPerPeriod = static_cast<Int32>(iniOperator->ReadInt(CLIENT_CFG_SEG, CLIENT_MSG_NUM_PER_PERIOD_KEY, 0));
    _sendPeriodMs = static_cast<Int32>(iniOperator->ReadInt(CLIENT_CFG_SEG, CLIENT_SEND_PERIOD_KEY, 0));
    _checkMsgId = iniOperator->ReadInt(CLIENT_CFG_SEG, CLIENT_CHECK_MSG_ID_KEY, 0) != 0;
    _isSendAfterSvrResArrive = iniOperator->ReadInt(CLIENT_CFG_SEG, CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY, 0) != 0;
}
