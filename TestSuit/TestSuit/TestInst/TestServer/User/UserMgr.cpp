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
#include "TestInst/TestServer/User/UserMgr.h"
#include "TestInst/TestServer/User/User.h"
#include "TestInst/TestServer/User/UserFacade.h"

UserMgr *g_UserMgr = NULL;

UserMgr::UserMgr()
    :_curMaxUserId(0)
    ,_dispatcher(NULL)
{
    _userFacade = new UserFacade;
    g_UserMgr = this;
}

UserMgr::~UserMgr()
{
    Fs_SafeFree(_userFacade);
}

Int32 UserMgr::OnInitialize()
{
    _dispatcher = _logic->GetDispatcher();
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
    for(auto &iterUser : _sessionIdRefUsers)
    {
        auto user = iterUser.second;
        user->OnDisconnect();
        user->Close();
    }
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

User *UserMgr::NewUser(UInt64 sessionId)
{
    auto user = new User(sessionId, ++_curMaxUserId, _logic);
    _sessionIdRefUsers.insert(std::make_pair(sessionId, user));
    _userIdRefUsers.insert(std::make_pair(_curMaxUserId, user));
    return user;
}
