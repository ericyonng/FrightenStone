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
 * @file  : User.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_User_Impl_User_H__
#define __Service_LogicSvc_Modules_User_Impl_User_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "Service/Common/LogicCommon.h"
#include "Service/LogicSvc/Modules/Common/ModuleCommon.h"
#include "protocols/protocol.h"

class IUserMgr;
class EventMgr;
class IUserSys;

// TODO:还要对user进行扩展：user工厂类,系统节点user,真实人user等
class User
{
    OBJ_POOL_CREATE_DEF(User);
public:
    User(IUserMgr *userMgr, UInt64 sessionId, UInt64 userId);
    ~User();

public:
    Int32 Login(fs::LoginData *loginData);
    Int32 Logout();
    void CheckHeart();

public:
    // 取得指定用户系统(泛型方法)
    template <typename UserSysType>
    UserSysType *GetSys();
    template <typename UserSysType>
    const UserSysType *GetSys() const;

    // 取得指定用户系统(非泛型方法)
    IUserSys *GetSys(const fs::FS_String &sysName);
    const IUserSys *GetSys(const fs::FS_String &sysName) const;

    // 取得用户系统列表
    std::vector<IUserSys *> &GetSyss();
    const std::vector<IUserSys *> &GetSyss() const;

public:
    virtual UInt64 GetSessionId() const;
    virtual UInt64 GetUseId() const;
    // 为NULL表示断线,或者session IsValid 为false
    const fs::FS_Session *GetSession() const;

    virtual void Close(Int32 reason);
    // msgdata不可以大于网络缓冲区大小 FS_BUFF_SIZE_DEF
    void SendData(fs::NetMsg_DataHeader *msgData);
    void SendData(const Byte8 *msgData, UInt64 msgLen);
    void OnDisconnect();
    void OnConnected();

    #pragma region User级别事件接口
public:
    // 添加User级别事件Listener
    template <typename ObjectType>
    fs::FS_ListenerStub AddListener(Int32 id,
                                  ObjectType *obj,
                                  void (ObjectType::*listener)(fs::FS_Event *),
                                  const fs::FS_ListenerStub &bindedStub = FS_INVALID_LISTENER_STUB);

    // 添加User级别事件Listener
    fs::FS_ListenerStub AddListener(Int32 id,
                                  fs::IDelegate<void, fs::FS_Event *> *listener,
                                  const fs::FS_ListenerStub &bindedStub = FS_INVALID_LISTENER_STUB);

    // 移除User级别事件Listener
    Int32 RemoveListener(Int32 id);

    // 移除User级别事件Listener
    Int32 RemoveListener(const fs::FS_ListenerStub &stub);
    Int32 RemoveListenerX(fs::FS_ListenerStub &stub);   // 同时重置外部的stub

    // Fire User级别事件
    void FireEvent(fs::FS_Event *event);

    // 当前是否正在处理User组件事件派遣
    bool IsFiring() const;
    #pragma endregion

private:
    void _AddSys(IUserSys *sys);
    IUserSys *_CreateSys(const ILogicSysInfo *sysInfo);
    void _RemoveSyss();

    Int32 _Create(bool isShowLog = false);

    void _FireLogin(UInt64 sessionId, UInt64 userId, Int32 recvMsgId);

private:
    friend class UserMgr;

    UInt64 _sessionId;
    UInt64 _userId;
//     // 用于server检测接收到的消息ID是否连续 每收到一个客户端消息会自增1以便与客户端的msgid校验，不匹配则报错处理（说明丢包等）
//     Int32 _recvMsgId = 1;
//     // 测试接收发逻辑用
//     // 用于client检测接收到的消息ID是否连续 每发送一个消息会自增1以便与客户端的sendmsgid校验，不匹配则客户端报错（说明丢包等）
//     Int32 _sendMsgId = 1;

    IUserMgr *_userMgr;
    EventMgr *_eventMgr;
    fs::IFS_MsgDispatcher *_dispatcher;
    std::vector<IUserSys *> _userSysList;                       // 用户系统列表
    std::map<fs::FS_String, IUserSys *> _userSysNameRefSyss;    // 用户系统集;         key: 系统名, value: 系统
    std::map<fs::FS_String, IUserSys *> _iuserSysNameRefSyss;   //; 用户系统接口集;    key: 系统对应的接口类名, value: 系统
};

#include "Service/LogicSvc/Modules/User/Impl/UserImpl.h"

#endif
