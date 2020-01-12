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
 * @file  : FS_IocpTcpClient.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/05
 * @brief :
 * 
 *
 * 
 */
#include <stdafx.h>
#include <FrightenStone/common/net/Impl/FS_IocpTcpClient.h>

#ifdef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_IocpTcpClient, __DEF_OBJ_POOL_OBJ_NUM__);
FS_IocpTcpClient::FS_IocpTcpClient()
{
}

FS_IocpTcpClient::~FS_IocpTcpClient()
{
}

void FS_IocpTcpClient::OnConnect()
{
    // 重新更新地址信息，以便地址正常
    _session->ResetAddr();
    _session->OnConnect();
    auto addr = _session->GetAddr();
    g_Log->net<FS_IocpTcpClient>("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]"
                                 , _session->GetSessionId()
                                 , _session->GetSocket()
                                 , addr->GetAddr().c_str()
                                 , addr->GetPort());
}

bool FS_IocpTcpClient::OnRun(int microseconds)
{
    if(IsRun())
    {
        // 需要写数据的客户端,才postSend
//         auto iocpSession = _session->CastTo<FS_IocpSession>();
//         if(iocpSession->HasMsgToSend())
//         {
//             auto ioData = iocpSession->MakeSendIoData();
//             if(ioData)
//             {
//                 auto st = _iocp.PostSend(iocpSession->GetSocket(), ioData);
//                 if(st != StatusDefs::Success)
//                 {
//                     iocpSession->ResetPostSendMask();
//                     if(st != StatusDefs::IOCP_ClientForciblyClosed)
//                     {
//                         g_Log->e<FS_IocpTcpClient>(_LOGFMT_("sessionId[%llu] socket[%llu] post send fail st[%d]")
//                                                    , iocpSession->GetSessionId(), iocpSession->GetSocket(), st);
//                     }
// 
//                     Close();
//                     return false;
//                 }
//             }
// 
//             // --
//             ioData = iocpSession->MakeRecvIoData();
//             if(ioData)
//             {
//                 auto st = _iocp.PostRecv(iocpSession->GetSocket(), ioData);
//                 if(st != StatusDefs::Success)
//                 {
//                     iocpSession->ResetPostRecvMask();
//                     if(st != StatusDefs::IOCP_ClientForciblyClosed)
//                     {
//                         g_Log->e<FS_IocpTcpClient>(_LOGFMT_("sessionId[%llu] socket[%llu] post recv fail st[%d]")
//                                                    , iocpSession->GetSessionId(), iocpSession->GetSocket(), st);
//                     }
// 
//                     Close();
//                     return false;
//                 }
//             }
//         }
//         else {
//             // --
//             auto ioData = iocpSession->MakeRecvIoData();
//             if(ioData)
//             {
//                 Int32 st = _iocp.PostRecv(iocpSession->GetSocket(), ioData);
//                 if(st != StatusDefs::Success)
//                 {
//                     iocpSession->ResetPostRecvMask();
//                     if(st != StatusDefs::IOCP_ClientForciblyClosed)
//                     {
//                         g_Log->e<FS_IocpTcpClient>(_LOGFMT_("sessionId[%llu] socket[%llu] post recv fail st[%d]")
//                                                    , iocpSession->GetSessionId(), iocpSession->GetSocket(), st);
//                     }
// 
//                     Close();
//                     return false;
//                 }
//             }
//         }
// 
//         //---
//         Int32 ret = StatusDefs::Success;
//         while(true)
//         {
//             ret = DoIocpNetEvents(microseconds);
//             if(ret == StatusDefs::IOCP_WaitTimeOut)
//             {// 超时才处理消息
//                 DoMsg();
//                 return true;
//             }
//             else if(ret != StatusDefs::Success)
//             {
//                 return false;
//             }
//         }

        //---
        return true;
    }

    return false;
}

Int32 FS_IocpTcpClient::DoIocpNetEvents(int microseconds)
{
    auto ret = _iocp.WaitForCompletion(_ev, microseconds);
    if(ret != StatusDefs::Success)
        return ret;

    // 处理iocp退出
//     if(_ev._data._code == IocpDefs::IO_QUIT)
//     {
//         g_Log->sys<FS_IocpTcpClient>(_LOGFMT_("FS_IocpMsgTransfer iocp退出 threadId<%llu> code=%lld")
//                                        , SystemUtil::GetCurrentThreadId(), _ev._data._code);
//         return StatusDefs::IocpMsgTransfer_IocpQuit;
//     }
// 
//     // 1.判断会话是否存在
//     const UInt64 sessionId = _ev._data._sessionId;
//     if(!_session)
//     {
//         g_Log->e<FS_IocpTcpClient>(_LOGFMT_("session not exists sessionId[%llu]"), sessionId);
//         Close();
//         return StatusDefs::Error;
//     }
//     if(_session->IsClose())
//     {
//         g_Log->e<FS_IocpTcpClient>(_LOGFMT_("session is close sessionId[%llu]"), sessionId);
//         Close();
//         return StatusDefs::Error;
//     }
//     if(_session->GetSessionId() != sessionId)
//     {
//         g_Log->e<FS_IocpTcpClient>(_LOGFMT_("session is wrong sessionId[%llu] local sessionId[%llu]")
//                                    , sessionId, _session->GetSessionId());
//         Close();
//         return StatusDefs::Error;
//     }
// 
//     auto iocpSession = _session->CastTo<FS_IocpSession>();
// 
//     // 2.处理接收与发送
//     if(IocpDefs::IO_RECV == _ev._ioData->_ioType)
//     {
//         if(_ev._bytesTrans <= 0)
//         {// 客户端断开处理
//             g_Log->net<FS_IocpTcpClient>("sessionId[%llu] sock[%llu] IO_TYPE::RECV bytesTrans[%lld] will disconnect"
//                                            , _session->GetSessionId()
//                                            , _session->GetSocket(),
//                                            _ev._bytesTrans);
// 
//             iocpSession->ResetAllIoMask();
//             Close();
//             return StatusDefs::Success;
//         }
// 
//         iocpSession->OnRecvSuc(_ev._bytesTrans, _ev._ioData);
//     }
//     else if(IocpDefs::IO_SEND == _ev._ioData->_ioType)
//     {
//         if(_ev._bytesTrans <= 0)
//         {// 客户端断开处理
//             g_Log->net<FS_IocpTcpClient>("sessionId[%llu] sock[%llu] IO_TYPE::IO_SEND bytesTrans[%lld] disconnected"
//                                            , iocpSession->GetSessionId()
//                                            , iocpSession->GetSocket(),
//                                            _ev._bytesTrans);
//             iocpSession->ResetAllIoMask();
//             Close();
//             return StatusDefs::Success;
//         }
// 
//         iocpSession->OnSendSuc(_ev._bytesTrans, _ev._ioData);
//     }
//     else
//     {
//         iocpSession->EnableDisconnect();
//         iocpSession->Close();
//         g_Log->e<FS_IocpTcpClient>(_LOGFMT_("undefine io type[%d]."), _ev._ioData->_ioType);
//         return StatusDefs::Error;
//     }

    return StatusDefs::Success;
}

FS_NAMESPACE_END

#endif
