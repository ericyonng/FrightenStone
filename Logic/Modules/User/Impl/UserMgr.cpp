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
#include "Logic/Modules/User/Impl/UserMgr.h"
#include "Logic/Modules/User/Impl/User.h"
#include "Logic/Modules/User/Impl/UserFacade.h"
#include "Logic/Modules/Common/GlobalObjs/GlobalObjsDef.h"
#include "Logic/Common/LogicCommon.h"

IUserMgr *g_UserMgr = NULL;

UserMgr::UserMgr()
    :_curMaxUserId(0)
{
    _userFacade = new UserFacade;
    g_UserMgr = this;
}

UserMgr::~UserMgr()
{
    _ClearUsers();
    Fs_SafeFree(_userFacade);
}

Int32 UserMgr::OnInitialize()
{
    _userFacade->BindLogic(_logic);

    Int32 st = _userFacade->OnInitialize();
    if(st != StatusDefs::Success)
    {
        g_Log->e<UserMgr>(_LOGFMT_("_userFacade->OnInitialize fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 UserMgr::BeforeStart()
{
    Int32 st = _userFacade->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<UserMgr>(_LOGFMT_("_userFacade->BeforeStart fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 UserMgr::Start()
{
    Int32 st = _userFacade->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<UserMgr>(_LOGFMT_("_userFacade->Start fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

void UserMgr::BeforeClose()
{
    _userFacade->BeforeClose();
}

void UserMgr::Close()
{
    _userFacade->Close();
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

Int32 UserMgr::CreateUser(UInt64 sessionId)
{
    g_Log->i<UserMgr>(_LOGFMT_("will create new user, sessionId[%llu]"), sessionId);

    auto newUser = _NewUser(sessionId);
    Int32 ret = newUser->_Create();
    if(ret != StatusDefs::Success)
    {
        FS_Release(newUser);

        g_Log->e<UserMgr>(_LOGFMT_("Create new user failed, call User::Create() return error, sessionId: %llu, ret:%d")
                          , sessionId, ret);
        return ret;
    }

    // Log: CreateUser成功
    g_Log->i<UserMgr>(_LOGFMT_("Create new user success, sessionId: %llu"), sessionId);

    // TODO:创建用户完成事件

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
    FS_Release(iterUser->second);
    _sessionIdRefUsers.erase(iterUser);
    _userIdRefUsers.erase(userId);
}

User *UserMgr::_NewUser(UInt64 sessionId)
{
    auto user = new User(this, sessionId, ++_curMaxUserId);
    user->BindDispatcher(_dispatcher);
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
