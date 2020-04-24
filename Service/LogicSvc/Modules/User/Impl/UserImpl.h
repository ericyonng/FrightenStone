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
 * @file  : UserImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifdef __Service_LogicSvc_Modules_User_Impl_User_H__

#pragma once
inline void User::SucRecvMsg()
{
    ++_recvMsgId;
}

template <typename UserSysType>
inline UserSysType *User::GetSys()
{
    auto sysName = fs::RTTIUtil::GetByType<UserSysType>();
    if(UNLIKELY(sysName == ""))
        return NULL;

    return static_cast<UserSysType *>(GetSys(sysName));
}

template <typename UserSysType>
inline const UserSysType *User::GetSys() const
{
    auto sysName = fs::RTTIUtil::GetByType<UserSysType>();
    if(UNLIKELY(sysName == ""))
        return NULL;

    return static_cast<UserSysType *>(GetSys(sysName));
}

inline std::vector<IUserSys *> &User::GetSyss()
{
    return _userSysList;
}

inline const std::vector<IUserSys *> &User::GetSyss() const
{
    return _userSysList;
}

inline UInt64 User::GetSessionId() const
{
    return _sessionId;
}

inline UInt64 User::GetUseId() const
{
    return _userId;
}

inline Int32 User::GetRecvMsgId() const
{
    return _recvMsgId;
}

inline Int32 User::GetSendMsgId() const
{
    return _sendMsgId;
}

#pragma region User级别事件接口
template <typename ObjectType>
inline fs::FS_ListenerStub User::AddListener(Int32 id,
                                        ObjectType *obj,
                                        void (ObjectType::*listener)(fs::FS_Event *),
                                        const fs::FS_ListenerStub &bindedStub)
{
    return _eventMgr->AddListener(id, obj, listener, bindedStub);
}

inline fs::FS_ListenerStub User::AddListener(Int32 id,
                                fs::IDelegate<void, fs::FS_Event *> *listener,
                                const fs::FS_ListenerStub &bindedStub)
{
    return _eventMgr->AddListener(id, listener, bindedStub);
}

inline Int32 User::RemoveListener(Int32 id)
{
    return _eventMgr->RemoveListener(id);
}

inline Int32 User::RemoveListener(const fs::FS_ListenerStub &stub)
{
    return _eventMgr->RemoveListener(stub);
}

inline Int32 User::RemoveListenerX(fs::FS_ListenerStub &stub)
{
    return _eventMgr->RemoveListenerX(stub);
}

inline void User::FireEvent(fs::FS_Event *event)
{
    return _eventMgr->FireEvent(event);
}

inline bool User::IsFiring() const
{
    return _eventMgr->IsFiring();
}

#pragma endregion

#endif
