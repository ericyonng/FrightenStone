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
 * @file  : IFS_BusinessLogic.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/20
 * @brief :
 * 
 *
 * 
 */

#ifndef __Base_Common_Net_Impl_IFS_BusinessLogic_H__
#define __Base_Common_Net_Impl_IFS_BusinessLogic_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/net/Impl/IFS_Session.h"
#include "base/common/net/Impl/FS_ServerCore.h"
#include "base/common/log/Log.h"
#include "base/common/assist/utils/Impl/STLUtil.h"
#include "base/common/net/protocol/protocol.h"
#include "base/common/net/Impl/FS_MsgReadStream.h"
#include "base/common/net/Impl/FS_MsgWriteStream.h"

FS_NAMESPACE_BEGIN

struct BASE_EXPORT NetMsg_DataHeader;
class BASE_EXPORT IFS_MsgDispatcher;

class BASE_EXPORT IFS_BusinessLogic
{
public:
    IFS_BusinessLogic();
    virtual ~IFS_BusinessLogic();
    virtual void Release() = 0;

public:
    virtual Int32 BeforeStart() { return StatusDefs::Success; }
    virtual Int32 Start() = 0;
    virtual Int32 AfterStart() { return StatusDefs::Success; }
    virtual void WillClose() {} // 断开与模块之间的依赖
    virtual void BeforeClose() {}
    virtual void Close() = 0;
    virtual void AfterClose() {}

public:
    virtual void OnMsgDispatch(UInt64 sessionId, NetMsg_DataHeader *msgData) = 0;
    virtual void OnMsgDispatch(IFS_Session *session, NetMsg_DataHeader *msgData) = 0;
    virtual void OnSessionDisconnected(UInt64 sessionId) = 0;

    void SetDispatcher(IFS_MsgDispatcher *dispatcher);
    void AttachMsgQueue(ConcurrentMessageQueue *messageQueue);

protected:
    IFS_MsgDispatcher *_dispatcher;
};

class User
{
public:
    User(UInt64 sessionId, IFS_MsgDispatcher *dispatcher)
        :_sessionId(sessionId)
        , _recvMsgId(1)
        , _sendMsgId(1)
        , _dispatcher(dispatcher)
    {
    }
    ~User()
    {
    }

    void SendData(IFS_Session *session, NetMsg_DataHeader *msgData)
    {
        session->Send(msgData);
    }

    UInt64 _sessionId;
    // 用于server检测接收到的消息ID是否连续 每收到一个客户端消息会自增1以便与客户端的msgid校验，不匹配则报错处理（说明丢包等）
    Int32 _recvMsgId = 1;
    // 测试接收发逻辑用
    // 用于client检测接收到的消息ID是否连续 每发送一个消息会自增1以便与客户端的sendmsgid校验，不匹配则客户端报错（说明丢包等）
    Int32 _sendMsgId = 1;
    IFS_MsgDispatcher *_dispatcher;
};

class BusinessExample : public IFS_BusinessLogic
{
public:
    std::map<UInt64, User *> _users;

public:
    BusinessExample() {}
    virtual ~BusinessExample()
    {
        STLUtil::DelMapContainer(_users);
    }
    virtual void Release()
    {
        delete this;
    }

public:
    User *GetUser(UInt64 sessionId)
    {
        auto iterUser = _users.find(sessionId);
        if(iterUser == _users.end())
            return NULL;

        return iterUser->second;
    }

    void RemoveUser(UInt64 sessionId)
    {
        auto iterUser = _users.find(sessionId);
        if(iterUser == _users.end())
            return;

        Fs_SafeFree(iterUser->second);
        _users.erase(iterUser);
    }

    User *NewUser(UInt64 sessionId)
    {
        auto user = new User(sessionId, _dispatcher);
        _users.insert(std::make_pair(sessionId, user));
        return user;
    }

    virtual Int32 Start()
    {
        return StatusDefs::Success;
    }

    void BeforeClose()
    {
        STLUtil::DelMapContainer(_users);
    }

    virtual void Close()
    {
    }

    virtual void OnMsgDispatch(fs::IFS_Session *session, NetMsg_DataHeader *msgData)
    {
        auto sessionId = session->GetSessionId();
        auto user = GetUser(sessionId);
        if(!user)
            user = NewUser(sessionId);

        // g_Log->any<MyLogic>("sessionid[%llu] handle a msg", sessionId);
        switch(msgData->_cmd)
        {
            case fs::ProtocolCmd::LoginReq:
            {
                fs::LoginReq *login = static_cast<fs::LoginReq *>(msgData);

                //     g_Log->any<MyLogic>("sessionid[%llu] login username[%s], pwd[%s] msgId[%d] user recvmsgid"
                //                         , sessionId, login->_userName, login->_pwd, login->_msgId, user->_recvMsgId);
                     // 检查消息ID
                if(login->_msgId != user->_recvMsgId)
                {//当前消息ID和本地收消息次数不匹配
                    g_Log->custom("OnMsgDispatch sessionId<%llu> msgID<%d> _nRecvMsgID<%d> diff<%d>"
                                      , sessionId, login->_msgId
                                      , user->_recvMsgId, login->_msgId - user->_recvMsgId);
                }

                // 返回包
                ++user->_recvMsgId;
                fs::LoginRes ret;
                ret._msgId = user->_sendMsgId;
                user->SendData(session, &ret);
                ++user->_sendMsgId;
                return;
            }//接收 消息---处理 发送   生产者 数据缓冲区  消费者 
            break;
            case fs::ProtocolCmd::LogoutReq:
            {
                fs::FS_MsgReadStream r(msgData);
                // 读取消息长度
                r.ReadInt16();
                // 读取消息命令
                r.GetNetMsgCmd();
                auto n1 = r.ReadInt8();
                auto n2 = r.ReadInt16();
                auto n3 = r.ReadInt32();
                auto n4 = r.ReadFloat();
                auto n5 = r.ReadDouble();
                uint32_t n = 0;
                r.ReadWithoutOffsetPos(n);
                char name[32] = {};
                auto n6 = r.ReadArray(name, 32);
                char pw[32] = {};
                auto n7 = r.ReadArray(pw, 32);
                int ata[10] = {};
                auto n8 = r.ReadArray(ata, 10);
                ///
                fs::FS_MsgWriteStream s(128);
                s.SetNetMsgCmd(fs::ProtocolCmd::LogoutNty);
                s.WriteInt8(n1);
                s.WriteInt16(n2);
                s.WriteInt32(n3);
                s.WriteFloat(n4);
                s.WriteDouble(n5);
                s.WriteArray(name, n6);
                s.WriteArray(pw, n7);
                s.WriteArray(ata, n8);
                s.Finish();

                // TODO:需要支持流发送
//                 _dispatcher->SendData(sessionId, )
//                 client->SendData(*s.GetDataAddr(), s.GetWrLength());
                //                 g_Log->i<EasyFSServer>(_LOGFMT_("socket<%d> logout")
                //                                        , static_cast<Int32>(client->GetSocket()));
                return;
            }
            break;
            case fs::ProtocolCmd::CheckHeartReq:
            {
                fs::CheckHeartRes ret;
                //_dispatcher->SendData(sessionId, &ret);
                //g_Log->any("socket<%d> CheckHeartReq", static_cast<Int32>(client->GetSocket()));
                return;
            }
            default:
            {
                g_Log->custom("recv <sessionId=%llu> undefine msgType,dataLen：%hu"
                                  , sessionId, msgData->_packetLength);
            }
            break;
        }

        return;
    }
    virtual void OnMsgDispatch(UInt64 sessionId, NetMsg_DataHeader *msgData)
    {

    }
    virtual void OnSessionDisconnected(UInt64 sessionId)
    {
        RemoveUser(sessionId);
        // g_Log->any<MyLogic>("sessionid[%llu] Disconnected", sessionId);
    }
};

FS_NAMESPACE_END

#include "base/common/net/Impl/IFS_BusinessLogicImpl.h"

#endif
