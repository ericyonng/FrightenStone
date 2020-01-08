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
 * @file  : UserMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

class User;
class UserFacade;

class UserMgr : public fs::IBaseLogicSysMgr
{
public:
    UserMgr();
    ~UserMgr();

public:
    virtual Int32 OnInitialize();
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();
    void WillClose();

public:
    User *GetUserBySessionId(UInt64 sessionId);
    User *GetUserByUserId(UInt64 userId);
    void RemoveUser(UInt64 sessionId);
    User *NewUser(UInt64 sessionId);

private:
    UInt64 _curMaxUserId;
    std::map<UInt64, User *> _sessionIdRefUsers;
    std::map<UInt64, User *> _userIdRefUsers;
    fs::IFS_MsgDispatcher *_dispatcher;             // 消息分发
    UserFacade *_userFacade;
};

extern UserMgr *g_UserMgr;
