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
 * @file  : NormalHandler.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/5/19
 * @brief : 
 *          拆包:
                1.buffer有效长度是否大于自定义协议包长大小,
 *              2.若大于包长大小，取包长数据,比较有效长度是否大于包长度,若大于则至少有一个完整数据包
 *              3.可以开始解码,解得包协议号等
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/ProtocolMsgHandler/NormalHandler.h"
#include "protocols/protocol.h"
#include "Service/Application.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Interface/IProtocolMsgHandlerMgr.h"

NormalHandler::NormalHandler(const ProtocolTypeCfg *cfg, IProtocolMsgHandlerMgr *msgHandlerMgr)
    :_cfg(cfg)
    , _msgDecoder(new fs::NetMsgDecoder)
    , _msgHandlerMgr(msgHandlerMgr)
    , _dispatcher(NULL)
{
    _dispatcher = _msgHandlerMgr->GetDispatcher();
}

NormalHandler::~NormalHandler()
{
    FS_Release(_msgDecoder);
}

void NormalHandler::OnSessionMsgHandle(fs::FS_Session *session)
{// 返回若不是success，则会直接杀端
    // 会话缓冲区
    auto recvBuffer = session->GetRecvBuffer();
    const UInt64 sessionId = session->GetSessionId();
    auto user = g_UserMgr->GetUserBySessionId(sessionId);

    // 缓冲区有效数据长度
    auto &len = recvBuffer->GetDataLen();
    const Int64 bufferSize = recvBuffer->GetTotalSize();
    // 缓冲区指针
    const auto buffer = recvBuffer->GetData();
    // 转换成包头数据格式
    const fs::NetMsgHeaderFmtType::PacketLenDataType *packetLen =
        reinterpret_cast<const fs::NetMsgHeaderFmtType::PacketLenDataType *>(buffer);

    auto addr = session->GetAddr();
//     g_Log->netpackage<NormalHandler>(_LOGFMT_("before msg handle sessionId[%llu] addrinfo[%s] curbufferlen[%lld], packetlen[%u] recvBuffer raw:\n%s")
//         , sessionId, addr->ToString().c_str(), len, *packetLen, recvBuffer->ToString().c_str());

    // 1.缓冲有效数据长度大于包头长度说明包头数据到达
    // 2.与包长度比较，若有效数据长度比包长度大说明至少存在一个整包
    // 3.一个网络包比缓冲区还大是不允许出现的，若出现则直接kill掉连接
    // 4.packetLen不可为0，避免死循环
    //fs::Time s, e;
    for (; *packetLen &&
        (len >= fs::NetMsgHeaderFmtType::_msgHeaderSize) &&
        (len >= *packetLen);)
    {
        // 1.解码
        if (!_msgDecoder->Decode(buffer))
        {
            g_Log->w<NormalHandler>(_LOGFMT_("Decode error sessionId[%llu]")
                , sessionId);

            g_Log->netpackage<NormalHandler>(_LOGFMT_("Decode error sessionId[%llu] addrinfo[%s] curbufferlen[%lld], packetlen[%u] recvBuffer raw:\n%s")
                , sessionId, addr->ToString().c_str(), len, *packetLen, recvBuffer->ToString().c_str());

            user->Close(UserCloseReasonType::DecodeFail);
            break;
        }

//         g_Log->net<NormalHandler>("cmd[%u] msg iscoming len[%u]"
//             , _msgDecoder->GetCmd(), _msgDecoder->GetMsgLen());

//         g_Log->netpackage<NormalHandler>(_LOGFMT_("sessionId[%llu] msgdecode info: %s")
//             , sessionId, _msgDecoder->ToString().c_str());

        // 协议处理回调
        //s.FlushTime();
        if (!g_ProtocolStackMgr->InvokeProtocolHandler(sessionId, _msgDecoder))
        {
            g_Log->netpackage<NormalHandler>(_LOGFMT_("InvokeProtocolHandler error sessionId[%llu] msgdecode info: %s")
                , sessionId, _msgDecoder->ToString().c_str());

            g_Log->w<NormalHandler>(_LOGFMT_("InvokeProtocolHandler error sessionId[%llu]")
                , sessionId);
            user->Close(UserCloseReasonType::NoProtocolHandler);
        }

        //e.FlushTime();
        //Int64 useTime = (s - e).GetTotalMilliSeconds();

        // 弹出消息
        recvBuffer->PopFront(_msgDecoder->GetMsgLen());

//         g_Log->netpackage<NormalHandler>(_LOGFMT_("sessionId[%llu]  after msg handled recvbuffer info: %s")
//             , sessionId, recvBuffer->ToString().c_str());

        // 统计一个包
        g_ThisApp->HandleCompEv_RecvMsgAmount();

        // 处理过程中session被关闭则退出
        if (!session->IsValid())
            break;
    }

    // 网络包过大直接杀掉
    if (len >= fs::NetMsgHeaderFmtType::_msgHeaderSize && *packetLen > bufferSize)
    {
        g_Log->netpackage<NormalHandler>(_LOGFMT_("net package len larger than recv buffer size sessionId[%llu] addrinfo[%s] curbufferlen[%lld], packetlen[%u] recvBuffer raw:\n%s")
            , sessionId, addr->ToString().c_str(), len, *packetLen, recvBuffer->ToString().c_str());

        g_Log->w<NormalHandler>(_LOGFMT_("sessionId[%llu] addr[%s] cur buffer data len[%lld] net package len larger than recv buffer size packetlen[%u], buffersize[%lld]")
            , sessionId, session->GetAddr()->ToString().c_str(), len, *packetLen, bufferSize);
        user->Close(UserCloseReasonType::NetPackageTooLarge);
    }
}
