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
 * @file  : UserMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/User/Impl/UserMgr.h"
#include "Service/LogicSvc/Modules/User/Impl/User.h"
#include "Service/LogicSvc/Modules/User/Impl/UserFacade.h"
#include "Service/LogicSvc/Modules/Common/GlobalObjs/GlobalObjsDef.h"
#include "Service/Common/LogicCommon.h"

IUserMgr *g_UserMgr = NULL;

UserMgr::UserMgr()
    :_curMaxUserId(0)
    ,_sessionConnectedStub(0)
    ,_afterSessionDisconnectedStub(0)
{
    g_UserMgr = this;
}

UserMgr::~UserMgr()
{
    _ClearUsers();
    _UnRegisterEvents();
}

Int32 UserMgr::OnInitialize()
{
    _RegisterEvents();
    _RegisterProtocols();
    return StatusDefs::Success;
}

Int32 UserMgr::BeforeStart()
{
    return StatusDefs::Success;
}

Int32 UserMgr::Start()
{
    return StatusDefs::Success;
}

void UserMgr::BeforeClose()
{

}

void UserMgr::Close()
{

}

void UserMgr::WillClose()
{
    _ClearUsers();
}

Int32 UserMgr::RegisterUserSys(IFS_LogicSysFactory *sysFactory)
{
    auto sysInfo = sysFactory->CreateSysInfo();

    auto &sysName = sysInfo->GetSysName();
    if(GetSysInfo(sysName))
    {
        g_Log->e<UserMgr>(_LOGFMT_("RegisterUserSys: User sys name[%s] repeat"), sysName.c_str());

        delete sysInfo;
        return StatusDefs::Repeat;
    }
    
    _sysInfoList.push_back(sysInfo);
    _sysNameRefSysInfoDict.insert(std::make_pair(sysName, sysInfo));
    _sysNameRefUserSysFactory.insert(std::make_pair(sysName, sysFactory));

    g_Log->i<UserMgr>(_LOGFMT_("Register game sys: [%s]")
                      , sysInfo->ToString().c_str());

    return StatusDefs::Success;
}

const ILogicSysInfo * UserMgr::GetSysInfo(const fs::FS_String &sysName) const
{
    auto it = _sysNameRefSysInfoDict.find(sysName);
    return it != _sysNameRefSysInfoDict.end() ? it->second : NULL;
}

const IFS_LogicSysFactory *UserMgr::GetSysFactory(const fs::FS_String &sysName) const
{
    auto it = _sysNameRefUserSysFactory.find(sysName);
    return it != _sysNameRefUserSysFactory.end() ? it->second : NULL;
}

const std::map<fs::FS_String, IFS_LogicSysFactory *> & UserMgr::GetSysFactoriesDict() const
{
    return _sysNameRefUserSysFactory;
}

const std::vector<ILogicSysInfo *> &UserMgr::GetSysInfosList() const
{
    return _sysInfoList;
}

Int32 UserMgr::CreateUser(UInt64 sessionId, bool isShowLog)
{
    if(isShowLog)
        g_Log->i<UserMgr>(_LOGFMT_("will create new user, sessionId[%llu]"), sessionId);

    auto newUser = _NewUser(sessionId);
    Int32 ret = newUser->_Create();
    if(ret != StatusDefs::Success)
    {
        Fs_SafeFree(newUser);

        g_Log->e<UserMgr>(_LOGFMT_("Create new user failed, call User::Create() return error, sessionId: %llu, ret:%d")
                          , sessionId, ret);
        return ret;
    }

    // Log: CreateUser�ɹ�
    if(isShowLog)
        g_Log->i<UserMgr>(_LOGFMT_("Create new user success, sessionId: %llu"), sessionId);

    // TODO:�����û�����¼�

    return StatusDefs::Success;
}

User *UserMgr::GetUserBySessionId(UInt64 sessionId)
{
    auto iterUser = _sessionIdRefUsers.find(sessionId);
    if(iterUser == _sessionIdRefUsers.end())
        return NULL;

    return iterUser->second;
}

User *UserMgr::GetUserByUserId(UInt64 userId)
{
    auto iterUser = _userIdRefUsers.find(userId);
    if(iterUser == _userIdRefUsers.end())
        return NULL;

    return iterUser->second;
}

void UserMgr::RemoveUser(UInt64 sessionId)
{
    auto iterUser = _sessionIdRefUsers.find(sessionId);
    if(iterUser == _sessionIdRefUsers.end())
        return;

    const UInt64 userId = iterUser->second->GetUseId();
    Fs_SafeFree(iterUser->second);
    _sessionIdRefUsers.erase(iterUser);
    _userIdRefUsers.erase(userId);
}

User *UserMgr::_NewUser(UInt64 sessionId)
{
    auto user = new User(this, sessionId, ++_curMaxUserId);
    _sessionIdRefUsers.insert(std::make_pair(sessionId, user));
    _userIdRefUsers.insert(std::make_pair(_curMaxUserId, user));
    return user;
}

void UserMgr::_ClearUsers()
{
    for(auto &iterUser : _sessionIdRefUsers)
    {
        auto user = iterUser.second;
        user->OnDisconnect();
        user->Close();
    }

    _sysInfoList.clear();
    fs::STLUtil::DelMapContainer(_sysNameRefSysInfoDict);
    fs::STLUtil::DelMapContainer(_sysNameRefUserSysFactory);
}

void UserMgr::_RegisterProtocols()
{
    _logic->SubscribeProtocol(fs::ProtocolCmd::LoginReq, fs::DelegatePlusFactory::Create(this, &UserMgr::_OnUserLoginReq));
    _logic->SubscribeProtocol(fs::ProtocolCmd::LogoutReq, fs::DelegatePlusFactory::Create(this, &UserMgr::_OnUserLogoutReq));
    _logic->SubscribeProtocol(fs::ProtocolCmd::CheckHeartReq, fs::DelegatePlusFactory::Create(this, &UserMgr::_OnUserCheckHeartReq));
}

void UserMgr::_OnUserLoginReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
{
    auto user = GetUserBySessionId(sessionId);
    if(!user)
        return;

    fs::LoginReq login;
    if(!login.DeserializeFrom(msgDecoder))
    {
        g_Log->e<UserFacade>(_LOGFMT_("sessionId[%llu] login req DeserializeFrom msgDecoder error len[%u]")
                             , sessionId, msgDecoder->GetMsgLen());
        return;
    }

    auto st = user->Login(&login._loginData);
    user->SucRecvMsg();
    fs::LoginRes ret;
    ret._msgId = user->GetSendMsgId();
    user->SendData(&ret);
}

void UserMgr::_OnUserLogoutReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
{
    // TODO:
    auto user = GetUserBySessionId(sessionId);
    if(!user)
        return;

    user->Logout();

    //     fs::FS_MsgReadStream r(msgData);
    //     // ��ȡ��Ϣ����
    //     r.ReadInt16();
    //     // ��ȡ��Ϣ����
    //     r.GetNetMsgCmd();
    //     auto n1 = r.ReadInt8();
    //     auto n2 = r.ReadInt16();
    //     auto n3 = r.ReadInt32();
    //     auto n4 = r.ReadFloat();
    //     auto n5 = r.ReadDouble();
    //     uint32_t n = 0;
    //     r.ReadWithoutOffsetPos(n);
    //     char name[32] = {};
    //     auto n6 = r.ReadArray(name, 32);
    //     char pw[32] = {};
    //     auto n7 = r.ReadArray(pw, 32);
    //     int ata[10] = {};
    //     auto n8 = r.ReadArray(ata, 10);
    //     ///
    //     fs::FS_MsgWriteStream s(128);
    //     s.SetNetMsgCmd(fs::ProtocolCmd::LogoutNty);
    //     s.WriteInt8(n1);
    //     s.WriteInt16(n2);
    //     s.WriteInt32(n3);
    //     s.WriteFloat(n4);
    //     s.WriteDouble(n5);
    //     s.WriteArray(name, n6);
    //     s.WriteArray(pw, n7);
    //     s.WriteArray(ata, n8);
    //     s.Finish();
}

void UserMgr::_OnUserCheckHeartReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
{
    // TODO:
    auto user = g_UserMgr->GetUserBySessionId(sessionId);
    if(!user)
        return;

    user->CheckHeart();
}

void UserMgr::_RegisterEvents()
{
    if(_sessionConnectedStub)
        return;

    _sessionConnectedStub = g_GlobalEventMgr->AddListener(EventIds::Logic_OnSessionConnected, this, &UserMgr::_OnSessionConnected);
    _afterSessionDisconnectedStub = g_GlobalEventMgr->AddListener(EventIds::Logic_OnAfterSessionDisconnected, this, &UserMgr::_OnAfterSessionDisconnected);
   // _connectToRemoteOpFinishStub = g_GlobalEventMgr->AddListener(EventIds::Logic_OnAllConnectToRemoteOpFinish, this, &UserMgr::_OnConnectToRemoteOpFinish);
}

void UserMgr::_UnRegisterEvents()
{
    if(!_sessionConnectedStub)
        return;

    g_GlobalEventMgr->RemoveListenerX(_sessionConnectedStub);
    g_GlobalEventMgr->RemoveListenerX(_afterSessionDisconnectedStub);
}

void UserMgr::_OnSessionConnected(fs::FS_Event *ev)
{
    const UInt64 sessionId = ev->GetParam(FS_EventParam::SessionId).AsUInt64();

    auto ret = CreateUser(sessionId);
    if(ret != StatusDefs::Success)
    {
        g_Log->e<UserMgr>(_LOGFMT_("create new user fail ret[%d] sessionId[%llu]"), ret, sessionId);
        return;
    }

    auto newUser = GetUserBySessionId(sessionId);
    newUser->OnConnected();
}

void UserMgr::_OnAfterSessionDisconnected(fs::FS_Event *ev)
{
    const UInt64 sessionId = ev->GetParam(FS_EventParam::SessionId).AsUInt64();
    auto user = GetUserBySessionId(sessionId);
    if(user)
        user->OnDisconnect();

    RemoveUser(sessionId);
}
