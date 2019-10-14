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
 * @file  : FS_ServerCore.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_ServerCore.h"
#include "base/common/net/Impl/IFS_ServerConfigMgr.h"
#include "base/common/net/Impl/FS_ServerConfigMgrFactory.h"
#include "base/common/net/Impl/FS_ConnectorFactory.h"
#include "base/common/net/Impl/FS_MsgTransferFactory.h"
#include "base/common/net/Impl/FS_MsgHandlerFactory.h"
#include "base/common/net/Impl/FS_SessionMgr.h"
#include "base/common/net/Impl/IFS_Connector.h"
#include "base/common/net/Impl/IFS_MsgTransfer.h"
#include "base/common/net/Impl/IFS_MsgHandler.h"
#include "base/common/net/Impl/IFS_Connector.h"

#include "base/common/status/status.h"
#include "base/common/log/Log.h"
#include "base/common/assist/utils/utils.h"
#include "base/common/crashhandle/CrashHandle.h"
#include "base/common/socket/socket.h"
#include "base/common/component/Impl/FS_CpuInfo.h"

fs::FS_ServerCore *g_ServerCore = NULL;

FS_NAMESPACE_BEGIN
FS_ServerCore::FS_ServerCore()
    :_serverConfigMgr(NULL)
    ,_cpuInfo(new FS_CpuInfo)
    ,_msgHandler(NULL)
    ,_sessiomMgr(NULL)
{
    g_ServerCore = this;
}

FS_ServerCore::~FS_ServerCore()
{
    Fs_SafeFree(_sessiomMgr);
    Fs_SafeFree(_msgHandler);
    STLUtil::DelVectorContainer(_msgTransfers);
    STLUtil::DelVectorContainer(_connectors);
    Fs_SafeFree(_cpuInfo);
    Fs_SafeFree(_serverConfigMgr);
    g_ServerCore = NULL;
}

#pragma region api
Int32 FS_ServerCore::Start()
{
    // 1.时区
    TimeUtil::SetTimeZone();

    // 2.类型识别
    SmartVarRtti::InitRttiTypeNames();

    // 3.初始化线程局部存储句柄
    Int32 ret = FS_TlsUtil::CreateUtilTlsHandle(); 
    if(ret != StatusDefs::Success)
    {
        FS_String str;
        str.AppendFormat("CreateUtilTlsHandle fail ret[%d]", ret);
        ASSERTBOX(str.c_str());
        return ret;
    }

    // 4.log初始化
    ret = g_Log->InitModule(NULL);
    if(ret != StatusDefs::Success)
    {
        FS_String str;
        str.AppendFormat("log init fail ret[%d]", ret);
        ASSERTBOX(str.c_str());
        return ret;
    }

    // 5. crash dump switch start
    ret = fs::CrashHandleUtil::InitCrashHandleParams();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("init crash handle params fail ret[%d]"), ret);
        return ret;
    }

    // cpu信息初始化
    if(!_cpuInfo->Initialize())
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("Initialize cpuinfo fail"));
        return StatusDefs::Failed;
    }

    // 6.Socket环境
    ret = SocketUtil::InitSocketEnv();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("InitSocketEnv fail ret[%d]"), ret);
        return ret;
    }

    // 7. 读取配置
    ret = _ReadConfig();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("_ReadConfig fail ret[%d]"), ret);
        return ret;
    }
    
    // 8.创建服务器模块
    ret = _CreateNetModules();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("_CreateNetModules fail ret[%d]"), ret);
        return ret;
    }

    // 9.注册接口
    _RegisterToModule();

    // 10.启动前...
    ret = _BeforeStart();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("_BeforeStart fail ret[%d]"), ret);
        return ret;
    }

    // 11.start 模块
    ret = _StartModules();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("_StartModules fail ret[%d]"), ret);
        return ret;
    }

    // 12.onstart
    ret = _OnStart();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("_OnStart fail ret[%d]"), ret);
        return ret;
    }

    // 13._AfterStart
    ret = _AfterStart();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("_AfterStart fail ret[%d]"), ret);
        return ret;
    }

    return StatusDefs::Success;
}

void FS_ServerCore::Wait()
{
    _waitForClose.Lock();
    _waitForClose.Wait();
    _waitForClose.Unlock();
}

void FS_ServerCore::Close()
{
    // 断开所有依赖
    _WillClose();

    // 各自自个模块移除资源
    _BeforeClose();

    // 移除服务器核心模块
    for(auto &connector : _connectors)
        connector->Close();
    for(auto &msgTransfer : _msgTransfers)
        msgTransfer->Close();
    _msgHandler->Close();
    _sessiomMgr->Close();

    // 最后处理
    _AfterClose();
}

#pragma endregion

/* 网络事件 */
#pragma region
void FS_ServerCore::_OnConnected(IFS_Session *session)
{
}
#pragma endregion

#pragma region inner api
Int32 FS_ServerCore::_ReadConfig()
{
    _serverConfigMgr = FS_ServerConfigMgrFactory::Create();
    auto ret = _serverConfigMgr->Init();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("server config init fail ret[%d]"), ret);
        return ret;
    }

    return StatusDefs::Success;
}

Int32 FS_ServerCore::_CreateNetModules()
{
    const Int32 cpuCnt = _cpuInfo->GetCpuCoreCnt();

    _connectors.resize(cpuCnt);
    for(Int32 i = 0; i < cpuCnt; ++i)
        _connectors.push_back(FS_ConnectorFactory::Create());

    _msgTransfers.resize(cpuCnt);
    for(Int32 i = 0; i < cpuCnt; ++i)
        _msgTransfers.push_back(FS_MsgTransferFactory::Create());

    _msgHandler = FS_MsgHandlerFactory::Create();
    _sessiomMgr = new FS_SessionMgr();
    return StatusDefs::Success;
}

Int32 FS_ServerCore::_StartModules()
{
    Int32 ret = StatusDefs::Success;
    for(auto &connector : _connectors)
    {
        ret = connector->Start();
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_ServerCore>(_LOGFMT_("connector start fail ret[%d]"), ret);
            return ret;
        }
    }

    for(auto &msgTransfer : _msgTransfers)
    {
        ret = msgTransfer->Start();
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_ServerCore>(_LOGFMT_("msgTransfer start fail ret[%d]"), ret);
            return ret;
        }
    }

    ret = _msgHandler->Start();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("msgHandler start fail ret[%d]"), ret);
        return ret;
    }

    ret = _sessiomMgr->Start();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("sessiomMgr start fail ret[%d]"), ret);
        return ret;
    }

    return StatusDefs::Success;
}

Int32 FS_ServerCore::_BeforeStart()
{
    Int32 ret = StatusDefs::Success;
    for(auto &connector : _connectors)
    {
        ret = connector->BeforeStart();
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_ServerCore>(_LOGFMT_("connector BeforeStart fail ret[%d]"), ret);
            return ret;
        }
    }

    for(auto &msgTransfer : _msgTransfers)
    {
        ret = msgTransfer->BeforeStart();
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_ServerCore>(_LOGFMT_("msgTransfer BeforeStart fail ret[%d]"), ret);
            return ret;
        }
    }

    ret = _msgHandler->BeforeStart();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("msgHandler BeforeStart fail ret[%d]"), ret);
        return ret;
    }

    ret = _sessiomMgr->BeforeStart();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("sessiomMgr BeforeStart fail ret[%d]"), ret);
        return ret;
    }

    return StatusDefs::Success;
}

Int32 FS_ServerCore::_OnStart()
{
    return StatusDefs::Success;
}

Int32 FS_ServerCore::_AfterStart()
{
    Int32 ret = StatusDefs::Success;
    for(auto &connector : _connectors)
    {
        ret = connector->AfterStart();
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_ServerCore>(_LOGFMT_("connector AfterStart fail ret[%d]"), ret);
            return ret;
        }
    }

    for(auto &msgTransfer : _msgTransfers)
    {
        ret = msgTransfer->AfterStart();
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_ServerCore>(_LOGFMT_("msgTransfer AfterStart fail ret[%d]"), ret);
            return ret;
        }
    }

    ret = _msgHandler->AfterStart();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("msgHandler AfterStart fail ret[%d]"), ret);
        return ret;
    }

    ret = _sessiomMgr->AfterStart();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ServerCore>(_LOGFMT_("sessiomMgr AfterStart fail ret[%d]"), ret);
        return ret;
    }

    return StatusDefs::Success;
}

void FS_ServerCore::_WillClose()
{
    for(auto &connector : _connectors)
        connector->WillClose();

    for(auto &msgTransfer : _msgTransfers)
        msgTransfer->WillClose();

    _msgHandler->WillClose();
    _sessiomMgr->WillClose();
}

void FS_ServerCore::_BeforeClose()
{
    for(auto &connector : _connectors)
        connector->BeforeClose();

    for(auto &msgTransfer : _msgTransfers)
        msgTransfer->BeforeClose();

    _msgHandler->BeforeClose();
    _sessiomMgr->BeforeClose();
}

void FS_ServerCore::_AfterClose()
{
    for(auto &connector : _connectors)
        connector->AfterClose();

    for(auto &msgTransfer : _msgTransfers)
        msgTransfer->AfterClose();

    _msgHandler->AfterClose();
    _sessiomMgr->AfterClose();
}

void FS_ServerCore::_RegisterToModule()
{
    for(auto &connector : _connectors)
    {
        auto onConnectedRes = DelegatePlusFactory::Create(this, &FS_ServerCore::_OnConnected);
        connector->RegisterConnected(onConnectedRes);
    }

    // TODO:
//     for(auto &msgTransfer : _msgTransfers)
//     {
//         msgTransfer
//     }
//         msgTransfer->AfterClose();
// 
//     _msgHandler->AfterClose();
//     _sessiomMgr->AfterClose();
}

#pragma endregion

FS_NAMESPACE_END
