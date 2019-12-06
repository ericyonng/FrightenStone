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
 * @file  : FS_ClientCfgMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/5
 * @brief :
 * 
 *
 * 
 */

#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_ClientCfgMgr_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_ClientCfgMgr_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include <FrightenStone/common/basedefs/BaseDefs.h>
#include <FrightenStone/common/component/Impl/FS_String.h>

FS_NAMESPACE_BEGIN

class FS_IniFile;

class BASE_EXPORT FS_ClientCfgMgr
{
public:
    FS_ClientCfgMgr();
    ~FS_ClientCfgMgr();

public:
    Int32 Init(const char *clientCfgPath = "./ClientCfg.ini");
    FS_String GetStr(const char *segment, const char *key, const char *defaultStr = "") const;
    Int64 GetInt(const char *segment, const char *key, Int64 defaultInt = -1) const;

    /* 具体配置get接口 */
public:
    FS_String GetTargetSvrIp() const;
    UInt16 GetTargetSvrPort() const;
    Int32 GetClientQuantity() const;
    Int32 GetThreadQuantity() const;
    Int32 GetMsgNumPerPeriod() const;
    Int32 GetSendPeriod() const;
    UInt64 GetSendBufferSize() const;
    UInt64 GetRecvBufferSize() const;
    bool NeedCheckMsgId() const;
    bool IsSendAfterSvrResArrive() const;
    Int32 GetMemPoolBufferCntInit() const;
    UInt64 GetMemPoolAllocMaxBytes() const;
    Int64 GetHeartbeatDeadTimeInterval() const;

public:
    void Lock();
    void Unlock();

private:
    Int32 _InitDefCfgs();
    void _ReadAllCfgs();

private:
    FS_IniFile *_ini;

    /* 配置缓存 */
    FS_String _ip;
    UInt16 _port = 0;
    Int32 _clientQuantity = 0;
    Int32 _threadQuantity = 0;
    Int32 _msgNumPerPeriod = 0;
    Int32 _sendPeriod = 0;
    UInt64 _sendBufferSize = 0;
    UInt64 _recvBufferSize = 0;
    bool _checkMsgId = 0;
    bool _isSendAfterSvrResArrive = 0;
    Int32 _memPoolBufferCntInit = 0;
    UInt64 _memPoolAllocMaxBytes = 0;
    Int64 _heartbeatInterval = 0;
};

FS_NAMESPACE_END

#include <FrightenStone/common/net/Impl/FS_ClientCfgMgrImpl.h>

extern BASE_EXPORT fs::FS_ClientCfgMgr *g_ClientCfgMgr;                       // 客户端配置

#endif