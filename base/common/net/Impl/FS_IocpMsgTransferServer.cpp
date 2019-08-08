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
 * @file  : FS_IocpMsgTransferServer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_IocpMsgTransferServer.h"
#include "base/common/net/Impl/FS_Iocp.h"
#include "base/common/net/Impl/FS_Client.h"
#include "base/common/net/Defs/IocpDefs.h"
#include "base/common/net/Defs/FS_NetDefs.h"
#include "base/common/net/Defs/FS_NetBuffer.h"
#include "base/common/net/Impl/FS_Iocp.h"
#include "base/common/net/Interface/INetEvent.h"
#include "base/common/log/Log.h"

FS_NAMESPACE_BEGIN

FS_IocpMsgTransferServer::FS_IocpMsgTransferServer()
    :_iocpClientMsgTransfer(new FS_Iocp)
    ,_ioEvent(new IO_EVENT)
{
    _iocpClientMsgTransfer->Create();
}

FS_IocpMsgTransferServer::~FS_IocpMsgTransferServer()
{
    Close();
    Fs_SafeFree(_iocpClientMsgTransfer);
    Fs_SafeFree(_ioEvent);
}

void FS_IocpMsgTransferServer::BeforeClose()
{
    g_Log->net("FS_IocpMsgTransferServer%d.BeforeClose begin quit iocp", _id);
    // 退出iocp
    _iocpClientMsgTransfer->PostQuit();
}

Int32 FS_IocpMsgTransferServer::_BeforeClientMsgTransfer()
{
    // 1.遍历post 客户端请求
    FS_Client *client = NULL;
    for(auto iter = _socketRefClients.begin(); iter != _socketRefClients.end(); )
    {
        client = iter->second;

        // 需要写数据的客户端,才postSend
        if(client->NeedWrite())
        {
            auto ioData = client->MakeSendIoData();
            if(ioData)
            {
                if(!_iocpClientMsgTransfer->PostSend(ioData))
                {
                    _OnClientLeave(client);
                    iter = _socketRefClients.erase(iter);
                    continue;
                }
            }

            ioData = client->MakeRecvIoData();
            if(ioData)
            {
                if(!_iocpClientMsgTransfer->PostRecv(ioData))
                {
                    _OnClientLeave(client);
                    iter = _socketRefClients.erase(iter);
                    continue;
                }
            }
        }
        else {
            auto ioData = client->MakeRecvIoData();
            if(ioData)
            {
                if(!_iocpClientMsgTransfer->PostRecv(ioData))
                {
                    _OnClientLeave(client);
                    iter = _socketRefClients.erase(iter);
                    continue;
                }
            }
        }

        iter++;
    }

    // 2.iocp等待消息完成直到timeout或error为止
    while(true)
    {
        int ret = _ListenIocpNetEvents();
        if(ret == StatusDefs::WaitEventTimeOut)
            return StatusDefs::Success;
        else if(ret != StatusDefs::Success)
            return ret;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpMsgTransferServer::_ListenIocpNetEvents()
{
    auto ret = _iocpClientMsgTransfer->WaitForCompletion(*_ioEvent, 1);
    if(ret != StatusDefs::Success)
    {
        g_Log->net("FS_IOCPServer%d.DoIocpNetEvents.wait nothing but ret[%d]", _id, ret);
        return ret;
    }

    // 处理iocp退出
    if(_ioEvent->_data._code == IocpDefs::IO_QUIT)
    {
        g_Log->sys(_LOGFMT_("iocp退出 code=%lld"), _ioEvent->_data._code);
        return StatusDefs::IOCP_Quit;
    }

    if(IocpDefs::IO_RECV == _ioEvent->_ioData->_ioType)
    {// 接收数据 完成 Completion

        if(_ioEvent->_bytesTrans <= 0)
        {// 客户端断开处理
            
            //CELLLog_Info("rmClient sockfd=%d, IO_TYPE::RECV bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
            _RmClient(*_ioEvent);
            return ret;
        }

        FS_Client *client = reinterpret_cast<FS_Client*>(_ioEvent->_data._ptr);
        if(client)
        {// 客户端接收数据

            client->OnRecvFromIocp(_ioEvent->_bytesTrans);
            _OnPrepareNetRecv(client);
        }
    }
    else if(IocpDefs::IO_SEND == _ioEvent->_ioData->_ioType)
    {// 发送数据 完成 Completion

        if(_ioEvent->_bytesTrans <= 0)
        {// 客户端断开处理
            _RmClient(*_ioEvent);
            return ret;
        }

        FS_Client *client = reinterpret_cast<FS_Client *>(_ioEvent->_data._ptr);
        if(client)
            client->OnSend2iocp(_ioEvent->_bytesTrans);
    }
    else 
    {
        g_Log->e<FS_IocpMsgTransferServer>(_LOGFMT_("undefine io type[%d]."), _ioEvent->_ioData->_ioType);
    }

    return ret;
}

void FS_IocpMsgTransferServer::_RmClient(FS_Client *client)
{
    auto iter = _socketRefClients.find(client->GetSocket());
    if(iter != _socketRefClients.end())
        _socketRefClients.erase(iter);

    // 掉线处理
    _OnClientLeave(client);
}

void FS_IocpMsgTransferServer::_RmClient(IO_EVENT &ioEvent)
{
    FS_Client *client = reinterpret_cast<FS_Client *>(_ioEvent->_data._ptr);
    if(client)
        _RmClient(client);
}

void FS_IocpMsgTransferServer::_OnClientJoin(FS_Client *client)
{
    // 玩家连入时监听玩家数据
    _iocpClientMsgTransfer->Reg(client->GetSocket(), client);
    auto ioData = client->MakeRecvIoData();
    if(ioData)
        _iocpClientMsgTransfer->PostRecv(ioData);
}

FS_NAMESPACE_END
