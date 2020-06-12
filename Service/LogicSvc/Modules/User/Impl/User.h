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

// TODO:��Ҫ��user������չ��user������,ϵͳ�ڵ�user,��ʵ��user��
class User
{
    OBJ_POOL_CREATE_ANCESTOR(User);
public:
    User(IUserMgr *userMgr, UInt64 sessionId, UInt64 userId);
    ~User();

public:
//     Int32 Login(fs::LoginData *loginData);
     Int32 Logout();
     void CheckHeart();

public:
    // ȡ��ָ���û�ϵͳ(���ͷ���)
    template <typename UserSysType>
    UserSysType *GetSys();
    template <typename UserSysType>
    const UserSysType *GetSys() const;

    // ȡ��ָ���û�ϵͳ(�Ƿ��ͷ���)
    IUserSys *GetSys(const fs::FS_String &sysName);
    const IUserSys *GetSys(const fs::FS_String &sysName) const;

    // ȡ���û�ϵͳ�б�
    std::vector<IUserSys *> &GetSyss();
    const std::vector<IUserSys *> &GetSyss() const;

public:
    virtual UInt64 GetSessionId() const;
    virtual UInt64 GetUseId() const;
    // ΪNULL��ʾ����,����session IsValid Ϊfalse
    const fs::FS_Session *GetSession() const;

    virtual void Close(Int32 reason);
    // msgdata�����Դ������绺������С FS_BUFF_SIZE_DEF
    void SendData(fs::NetMsg_DataHeader *msgData);
    void SendData(const Byte8 *msgData, UInt64 msgLen);
    void OnDisconnect();
    void OnConnected();

    #pragma region User Level Events
public:
    // ���User�����¼�Listener
    template <typename ObjectType>
    fs::FS_ListenerStub AddListener(Int32 id,
                                  ObjectType *obj,
                                  void (ObjectType::*listener)(fs::FS_Event *),
                                  const fs::FS_ListenerStub &bindedStub = FS_INVALID_LISTENER_STUB);

    // ���User�����¼�Listener
    fs::FS_ListenerStub AddListener(Int32 id,
                                  fs::IDelegate<void, fs::FS_Event *> *listener,
                                  const fs::FS_ListenerStub &bindedStub = FS_INVALID_LISTENER_STUB);

    // �Ƴ�User�����¼�Listener
    Int32 RemoveListener(Int32 id);

    // �Ƴ�User�����¼�Listener
    Int32 RemoveListener(const fs::FS_ListenerStub &stub);
    Int32 RemoveListenerX(fs::FS_ListenerStub &stub);   // ͬʱ�����ⲿ��stub

    // Fire User�����¼�
    void FireEvent(fs::FS_Event *event);

    // ��ǰ�Ƿ����ڴ���User����¼���ǲ
    bool IsFiring() const;
    #pragma endregion

private:
    void _AddSys(IUserSys *sys);
    IUserSys *_CreateSys(const ILogicSysInfo *sysInfo);
    void _RemoveSyss();

    Int32 _Create(bool isShowLog = false);

    // void _FireLogin(UInt64 sessionId, UInt64 userId, Int32 recvMsgId);

private:
    friend class UserMgr;

    UInt64 _sessionId;
    UInt64 _userId;
//     // ����server�����յ�����ϢID�Ƿ����� ÿ�յ�һ���ͻ�����Ϣ������1�Ա���ͻ��˵�msgidУ�飬��ƥ���򱨴���˵�������ȣ�
//     Int32 _recvMsgId = 1;
//     // ���Խ��շ��߼���
//     // ����client�����յ�����ϢID�Ƿ����� ÿ����һ����Ϣ������1�Ա���ͻ��˵�sendmsgidУ�飬��ƥ����ͻ��˱���˵�������ȣ�
//     Int32 _sendMsgId = 1;

    IUserMgr *_userMgr;
    EventMgr *_eventMgr;
    fs::IFS_MsgDispatcher *_dispatcher;
    std::vector<IUserSys *> _userSysList;                       // �û�ϵͳ�б�
    std::map<fs::FS_String, IUserSys *> _userSysNameRefSyss;    // �û�ϵͳ��;         key: ϵͳ��, value: ϵͳ
    std::map<fs::FS_String, IUserSys *> _iuserSysNameRefSyss;   //; �û�ϵͳ�ӿڼ�;    key: ϵͳ��Ӧ�Ľӿ�����, value: ϵͳ
};

#include "Service/LogicSvc/Modules/User/Impl/UserImpl.h"

#endif
