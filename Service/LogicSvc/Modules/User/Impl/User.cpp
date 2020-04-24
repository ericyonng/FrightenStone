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
 * @file  : User.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/User/Impl/User.h"
#include "Service/LogicSvc/Modules/User/Interface/IUserMgr.h"
#include "Service/LogicSvc/Modules/User/Impl/IUserSys.h"

User::User(IUserMgr *userMgr, UInt64 sessionId, UInt64 userId)
    :_sessionId(sessionId)
    , _userId(userId)
    , _recvMsgId(1)
    , _sendMsgId(1)
    ,_userMgr(userMgr)
    ,_eventMgr(NULL)
    ,_dispatcher(NULL)
{
    _eventMgr = new EventMgr;
    _dispatcher = userMgr->GetDispatcher();
}

User::~User()
{
    _RemoveSyss();
    Fs_SafeFree(_eventMgr);
}

Int32 User::Login(fs::LoginData *loginData)
{
    if(loginData->_msgId != _recvMsgId)
    {// 当前消息ID和本地收消息次数不匹配
        g_Log->i<User>(_LOGFMT_("OnMsgDispatch sessionId<%llu> msgID<%d> _nRecvMsgID<%d> diff<%d>")
                      , _sessionId, loginData->_msgId
                      , _recvMsgId, loginData->_msgId - _recvMsgId);
    }

    // 打印
//     g_Log->custom("user[%s]-pwd[%s]-msgid[%d] login suc", 
//                   loginData->_userName
//                   , loginData->_pwd
//                   , loginData->_msgId);

    //g_Log->i<User>(_LOGFMT_("username[%s], pwd[%s] suc login"), loginData->_userName, loginData->_pwd);
    return StatusDefs::Success;
}

Int32 User::Logout()
{
    g_Log->i<User>(_LOGFMT_("userId[%llu] sessionId[%llu] logout"), _userId, _sessionId);
    return StatusDefs::Success;
}

void User::CheckHeart()
{

}

IUserSys *User::GetSys(const fs::FS_String &sysName)
{
    // 尝试从接口字典中取得
    auto iterIUser = _iuserSysNameRefSyss.find(sysName);
    if(iterIUser != _iuserSysNameRefSyss.end())
        return iterIUser->second;

    // 尝试中实现字典中取得
    auto it = _userSysNameRefSyss.find(sysName);
    return it != _userSysNameRefSyss.end() ? it->second : NULL;
}

const IUserSys * User::GetSys(const fs::FS_String &sysName) const
{
    // 尝试从接口字典中取得
    auto iterIUser = _iuserSysNameRefSyss.find(sysName);
    if(iterIUser != _iuserSysNameRefSyss.end())
        return iterIUser->second;

    // 尝试中实现字典中取得
    auto it = _userSysNameRefSyss.find(sysName);
    return it != _userSysNameRefSyss.end() ? it->second : NULL;
}

void User::Close()
{
    // TODO:抛一个断开session的消息到transfer 通过dispatcher
    _dispatcher->CloseSession(_sessionId);
}

// NetMsg_DataHeader内部会拷贝到缓冲区
void User::SendData(fs::NetMsg_DataHeader *msgData)
{
    _dispatcher->SendData(_sessionId, msgData);

    // 为了校验是否存在丢包,校验消息的连续性
    ++_sendMsgId;
}

void User::SendData(const Byte8 *msgData, UInt64 msgLen)
{
    _dispatcher->SendData(_sessionId, msgData, msgLen);

    // 为了校验是否存在丢包,校验消息的连续性
    ++_sendMsgId;
}

void User::OnDisconnect()
{
    g_Log->net<User>(" session id[%llu] user disconnect", _sessionId);
    _RemoveSyss();

    g_Log->i<User>(_LOGFMT_(" sessionId[%llu] disconnected")
                   , _sessionId);
}

void User::OnConnected()
{
    // 每个系统调用OnConnected
    for(auto &userSys : _userSysList)
        userSys->OnConnect();

    // 打印地址信息
    auto session = _dispatcher->GetSession(_sessionId);
    auto addr = session->GetAddr();
    g_Log->i<User>(_LOGFMT_(" new user connected sessionId[%llu] addr info[%s]")
                   , _sessionId, addr->ToString().c_str());
}

void User::_AddSys(IUserSys *sys)
{
    _userSysList.push_back(sys);
    _userSysNameRefSyss.insert(std::make_pair(sys->GetSysName(), sys));
    _iuserSysNameRefSyss.insert(std::make_pair(fs::FS_String("I") + sys->GetSysName(), sys));
}

IUserSys * User::_CreateSys(const ILogicSysInfo *sysInfo)
{
    auto sysFactory = _userMgr->GetSysFactory(sysInfo->GetSysName());

    auto sys = static_cast<IUserSys *>(sysFactory->CreateSys());
    sys->SetSysInfo(sysInfo);
    sys->BindDispatcher(_dispatcher);
    sys->_user = this;
    sys->OnInitialize();

    return sys;
}

void User::_RemoveSyss()
{
    fs::STLUtil::DelVectorContainer(_userSysList);
    _userSysNameRefSyss.clear();
    _iuserSysNameRefSyss.clear();
}

Int32 User::_Create(bool isShowLog)
{
    // 创建开始时间
    fs::Time beginTime, endTime;
    beginTime.FlushTime();

    auto session = _dispatcher->GetSession(_sessionId);
    if(isShowLog)
        g_Log->i<User>(_LOGFMT_("Create user, userId:%llu, sessionId:%llu, addrInfo:%s")
                   ,_userId, _sessionId, session->GetAddr()->ToString().c_str());

    // 创建所有syss，并调用Create完成所有sys创建，并调用Create
    auto &sysInfos = _userMgr->GetSysInfosList();
    for(auto infoIt = sysInfos.begin(); infoIt != sysInfos.end(); infoIt++)
    {
        const auto *sysInfo = *infoIt;
        if(isShowLog)
            g_Log->i<User>(_LOGFMT_("Create userid[%llu] sessionId[%llu]. system: %s...")
                       , _userId, _sessionId, sysInfo->GetSysName().c_str());

        auto *sys = _CreateSys(sysInfo);
        Int32 createRet = sys->_Create(this);
        if(createRet != StatusDefs::Success)
        {
            g_Log->e<User>(_LOGFMT_("Failed to create userid[%llu] sessionid[%llu] system: %s, status: %d")
                           , _userId, _sessionId, sysInfo->GetSysName().c_str(), createRet);

            FS_Release(sys);
            return createRet;
        }

        // TODO:load数据....

        _AddSys(sys);
    }

    // 调用所有system的AfterCreate
    for(auto sysIt = _userSysList.begin(); sysIt != _userSysList.end(); ++sysIt)
    {
        if(isShowLog)
            g_Log->i<User>(_LOGFMT_("After create userid[%llu] sessionId[%llu] system: %s...")
                       , _userId, _sessionId, (*sysIt)->GetSysInfo()->GetSysName().c_str());
        (*sysIt)->_AfterCreate();
    }

    // 创建结束时间
    endTime.FlushTime();

    if(isShowLog)
    {
        auto usedTime = endTime - beginTime;
        g_Log->i<User>(_LOGFMT_("sessionId[%llu] userId[%llu] created, usedTime[%lld] microseconds")
                       , _sessionId, _userId, usedTime.GetTotalMicroSeconds());
    }

    return StatusDefs::Success;
}
