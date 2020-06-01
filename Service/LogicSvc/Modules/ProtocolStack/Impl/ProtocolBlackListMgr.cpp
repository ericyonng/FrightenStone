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
 * @file  : ProtocolBlackListMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/31
 * @brief : 黑名单管理
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Impl/ProtocolBlackListMgr.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/BlackListElement.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/IsSpite.h"

ProtocolBlackListMgr::ProtocolBlackListMgr()
    :_sessionConnectingEvStub(0)
    , _userWillCloseEvStub(0)
    , _blackListExpireTimer(NULL)
{

}

ProtocolBlackListMgr::~ProtocolBlackListMgr()
{
    Fs_SafeFree(_blackListExpireTimer);
    fs::STLUtil::DelMapContainer(_ipBlackList);
    _UnRegisterEvents();
}

Int32 ProtocolBlackListMgr::OnInitialize()
{
    _blackListExpireTimer = new fs::FS_Timer(_dispatcher->GetTimeWheel());
    _blackListExpireTimer->SetTimeOutHandler(this, &ProtocolBlackListMgr::_OnExpireTimeOut);
    _RegisterEvents();
    return StatusDefs::Success;
}

bool ProtocolBlackListMgr::IsInBlackList(const fs::FS_Addr *addrInfo)
{
    auto ele = _GetEle(addrInfo->GetAddr());
    if (!ele)
        return false;

    // 是否在定时黑名单中,若在的话表示被黑名单锁定,不在则表示已移出黑民单
    if (ele->_expireTime)
        return _blackListExpireQueue.find(ele) != _blackListExpireQueue.end();

    return true;
}

void ProtocolBlackListMgr::RemoveFromBlackList(const fs::FS_Addr *addrInfo)
{
    _RemoveEle(addrInfo->GetAddr());
}

void ProtocolBlackListMgr::PushIntoForeverBlackList(const fs::FS_Addr *addrInfo)
{
    auto ele = _GetEle(addrInfo->GetAddr());
    if (!ele)
        ele = _NewEle(addrInfo->GetAddr(), 0, 0);

    _UpdateExpireTime(ele, 0);
}

void ProtocolBlackListMgr::_RegisterEvents()
{
    if (_sessionConnectingEvStub)
        return;

    _sessionConnectingEvStub = g_GlobalEventMgr->AddListener(EventIds::Logic_OnSessionConnecting
        , this, &ProtocolBlackListMgr::_OnSessionConnecting);
    _userWillCloseEvStub = g_GlobalEventMgr->AddListener(EventIds::User_WillClose
        , this, &ProtocolBlackListMgr::_OnUserWillClose);
}

void ProtocolBlackListMgr::_UnRegisterEvents()
{
    if (!_sessionConnectingEvStub)
        return;

    g_GlobalEventMgr->RemoveListenerX(_sessionConnectingEvStub);
    g_GlobalEventMgr->RemoveListenerX(_userWillCloseEvStub);
}

void ProtocolBlackListMgr::_OnSessionConnecting(fs::FS_Event *ev)
{
    const UInt64 sessionId = EventPtrGetParam(ev, SessionId).AsUInt64();
    auto session = _dispatcher->GetSession(sessionId);
    if (!session->IsValid())
        return;

    // 是否在黑名单
    auto addr = session->GetAddr();
    if (!IsInBlackList(addr))
        return;

    // 黑名单中的user即时断开
    auto users = g_UserMgr->GetUsersByIp(addr->GetAddr());
    for (auto &user : *users)
        user->Close(UserCloseReasonType::InBlackList);
}

void ProtocolBlackListMgr::_OnUserWillClose(fs::FS_Event *ev)
{
    const UInt64 sessionId = EventPtrGetParam(ev, SessionId).AsUInt64();
    const Int32 reason = EventPtrGetParam(ev, Reason).AsInt32();
    const UInt64 userId = EventPtrGetParam(ev, UserId).AsUInt64();

    if (!IsSpite::Check(reason))
        return;

    // 获取黑名单数据
    auto session = _dispatcher->GetSession(sessionId);
    auto addr = session->GetAddr();
    auto ele = _GetEle(addr->GetAddr());
    if (!ele)
        ele = _NewEle(addr->GetAddr(), 0, 0);
    else // 存在则先从定时队列中移除
        _blackListExpireQueue.erase(ele);

    // 累计次数
    ele->_breakRuleCount += 1;

    // 在配置指定的违规次数范围内,如配置指定次数为3,表示在3次范围内过期时间采用该配置
    auto allCfgs = g_BlackListCfgMgr->GetAllCfgs();
    const Int32 cfgQuantity = static_cast<Int32>(allCfgs->size());
    const BlackListCfg *cfg = NULL;
    for (Int32 i = 0; i < cfgQuantity; ++i)
    {
        cfg = allCfgs->at(i);
        if (ele->_breakRuleCount < cfg->_breakCount)
            break;
    }

    if (!cfg)
    {// 找不到配置则设置为永久黑名单
        _UpdateExpireTime(ele, 0);
    }
    else
    {// 时效黑名单
        Int64 interval = cfg->_expireTimeSlice*fs::Time::_millisecondPerSecond;
        _UpdateExpireTime(ele, fs::Time::NowMilliTimestamp() + interval);
    }

    // 开启定时器
    if (!_blackListExpireTimer->IsScheduling())
        _RestartTimer();

    g_Log->net<ProtocolBlackListMgr>("addr[%s] sessionId[%llu], userId[%llu], spite attack! reason[%d:%s]"
        " black list expiretime[%lld /millisec], breakRuleTimes[%d], expireTimeSlice[%lld]"
        , addr->ToString().c_str(), sessionId, userId, reason, UserCloseReasonType::GetStr(reason)
        , ele->_expireTime, ele->_breakRuleCount, cfg ? cfg->_expireTimeSlice : 0);
}

void ProtocolBlackListMgr::_OnExpireTimeOut(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curWheelTime)
{
    _blackListExpireTimer->Cancel();

    // 从时效黑民单队列中移除
    const auto nowTime = fs::Time::Now().GetMilliTimestamp();
    for (auto iterEle = _blackListExpireQueue.begin(); 
        iterEle != _blackListExpireQueue.end();)
    {
        auto ele = *iterEle;
        if (ele->_expireTime > nowTime)
            break;

        iterEle = _blackListExpireQueue.erase(iterEle);
    }

    _RestartTimer();
}

void ProtocolBlackListMgr::_RestartTimer()
{
    if (_blackListExpireQueue.empty())
        return;

    auto ele = *_blackListExpireQueue.begin();
    const Int64 nowTime = fs::Time::NowMilliTimestamp();
    Int64 interval = ele->_expireTime > nowTime ? (ele->_expireTime - nowTime) : 0;
    _blackListExpireTimer->Schedule(interval);
}

BlackListElement * ProtocolBlackListMgr::_GetEle(const fs::FS_String &ip)
{
    auto iterEle = _ipBlackList.find(ip);
    if (iterEle == _ipBlackList.end())
        return NULL;

    return iterEle->second;
}

BlackListElement *ProtocolBlackListMgr::_NewEle(const fs::FS_String &ip, Int64 expireTime, UInt64 breakRuleCount)
{
    BlackListElement *ele = new BlackListElement;
    ele->_ip = ip;
    ele->_expireTime = expireTime;
    ele->_breakRuleCount = breakRuleCount;
    _ipBlackList.insert(std::make_pair(ip, ele));

    if (expireTime > 0)
        _blackListExpireQueue.insert(ele);
    return ele;
}

void ProtocolBlackListMgr::_UpdateExpireTime(BlackListElement *ele, Int64 expireTime)
{
    _blackListExpireQueue.erase(ele);
    ele->_expireTime = expireTime;

    if (expireTime > 0)
        _blackListExpireQueue.insert(ele);
}

void ProtocolBlackListMgr::_RemoveEle(const fs::FS_String &ip)
{
    auto iterEle = _ipBlackList.find(ip);
    if (iterEle == _ipBlackList.end())
        return;

    _blackListExpireQueue.erase(iterEle->second);
    Fs_SafeFree(iterEle->second);
    _ipBlackList.erase(iterEle);
}
