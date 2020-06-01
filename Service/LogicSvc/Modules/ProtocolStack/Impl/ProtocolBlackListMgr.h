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
 * @file  : ProtocolBlackListMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/31
 * @brief : 黑名单管理
 */
#ifndef __Service_LogicSvc_Modules_ProtocolStack_Impl_ProtocolBlackListMgr_H__
#define __Service_LogicSvc_Modules_ProtocolStack_Impl_ProtocolBlackListMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/ProtocolStack/Interface/IProtocolBlackListMgr.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/BlackListEleComp.h"

class ProtocolBlackListMgr : public IProtocolBlackListMgr
{
public:
    ProtocolBlackListMgr();
    ~ProtocolBlackListMgr();

public:
    virtual Int32 OnInitialize();
    
public:
    // 是否在黑名单
    virtual bool IsInBlackList(const fs::FS_Addr *addrInfo);
    // 移除黑名单
    virtual void RemoveFromBlackList(const fs::FS_Addr *addrInfo);
    // 拉入黑名单/永久黑名单
    virtual void PushIntoForeverBlackList(const fs::FS_Addr *addrInfo);

private:
    /* 事件 */
    void _RegisterEvents();
    void _UnRegisterEvents();
    void _OnSessionConnecting(fs::FS_Event *ev);
    void _OnUserWillClose(fs::FS_Event *ev);

    /* 黑名单定时器相关 */
    void _OnExpireTimeOut(fs::FS_Timer *timer, const fs::Time &lastWheelTime, const fs::Time &curWheelTime);
    void _RestartTimer();

    /* 操作数据 */
    BlackListElement *_GetEle(const fs::FS_String &ip);
    BlackListElement *_NewEle(const fs::FS_String &ip, Int64 expireTime, UInt64 breakRuleCount);
    void _UpdateExpireTime(BlackListElement *ele, Int64 expireTime);
    void _RemoveEle(const fs::FS_String &ip);

private:
    fs::FS_ListenerStub _sessionConnectingEvStub;
    fs::FS_ListenerStub _userWillCloseEvStub;

    // 黑名单失效定时器 以ip为单位
    fs::FS_Timer *_blackListExpireTimer;
    std::set<BlackListElement *, BlackListEleComp> _blackListExpireQueue;   // 有过期时间的,永久黑名单则不加入
    std::map<fs::FS_String, BlackListElement *> _ipBlackList;
};

#endif
