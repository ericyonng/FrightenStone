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
 * @file  : UserFacade.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/User/Impl/UserFacade.h"
#include "Service/LogicSvc/Modules/User/Impl/UserMgr.h"
#include "Service/LogicSvc/Modules/User/Impl/User.h"
#include "Service/LogicSvc/Modules/User/Interface/IUserMgr.h"
#include "Service/LogicSvc/Modules/Common/GlobalObjs/GlobalObjsDef.h"
#include "Service/LogicSvc/Modules/ProtocolStack/ProtocolStack.h"

Int32 UserFacade::OnInitialize()
{
    _RegisterProtocols();
    return StatusDefs::Success;
}

Int32 UserFacade::BeforeStart()
{
    return StatusDefs::Success;
}

Int32 UserFacade::Start()
{
    return StatusDefs::Success;
}

void UserFacade::BeforeClose()
{

}

void UserFacade::Close()
{

}

void UserFacade::_RegisterProtocols()
{
//     g_ProtocolStackMgr->SubscribeProtocol(fs::ProtocolCmd::LoginReq, fs::DelegatePlusFactory::Create(this, &UserFacade::_OnUserLoginReq));
//     g_ProtocolStackMgr->SubscribeProtocol(fs::ProtocolCmd::LogoutReq, fs::DelegatePlusFactory::Create(this, &UserFacade::_OnUserLogoutReq));
//     g_ProtocolStackMgr->SubscribeProtocol(fs::ProtocolCmd::CheckHeartReq, fs::DelegatePlusFactory::Create(this, &UserFacade::_OnUserCheckHeartReq));
}
// 
// void UserFacade::_OnUserLoginReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
// {
// //     auto user = g_UserMgr->GetUserBySessionId(sessionId);
// //     if(!user)
// //         return;
// // 
// //     fs::LoginReq login;
// //     if(!login.DeserializeFrom(msgDecoder))
// //     {
// //         g_Log->e<UserFacade>(_LOGFMT_("sessionId[%llu] login req DeserializeFrom msgDecoder error len[%u]")
// //                              , sessionId, msgDecoder->GetMsgLen());
// //         return;
// //     }
// // 
// //     auto st = user->Login(&login._loginData);
// //     fs::LoginRes ret;
// //     user->SendData(&ret);
// }
// 
// void UserFacade::_OnUserLogoutReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
// {
//     // TODO:
// //     auto user = g_UserMgr->GetUserBySessionId(sessionId);
// //     if(!user)
// //         return;
// // 
// //     user->Logout();
// 
//     //     fs::FS_MsgReadStream r(msgData);
//     //     // 读取消息长度
//     //     r.ReadInt16();
//     //     // 读取消息命令
//     //     r.GetNetMsgCmd();
//     //     auto n1 = r.ReadInt8();
//     //     auto n2 = r.ReadInt16();
//     //     auto n3 = r.ReadInt32();
//     //     auto n4 = r.ReadFloat();
//     //     auto n5 = r.ReadDouble();
//     //     uint32_t n = 0;
//     //     r.ReadWithoutOffsetPos(n);
//     //     char name[32] = {};
//     //     auto n6 = r.ReadArray(name, 32);
//     //     char pw[32] = {};
//     //     auto n7 = r.ReadArray(pw, 32);
//     //     int ata[10] = {};
//     //     auto n8 = r.ReadArray(ata, 10);
//     //     ///
//     //     fs::FS_MsgWriteStream s(128);
//     //     s.SetNetMsgCmd(fs::ProtocolCmd::LogoutNty);
//     //     s.WriteInt8(n1);
//     //     s.WriteInt16(n2);
//     //     s.WriteInt32(n3);
//     //     s.WriteFloat(n4);
//     //     s.WriteDouble(n5);
//     //     s.WriteArray(name, n6);
//     //     s.WriteArray(pw, n7);
//     //     s.WriteArray(ata, n8);
//     //     s.Finish();
// }
// 
// void UserFacade::_OnUserCheckHeartReq(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
// {
//     // TODO:
//     auto user = g_UserMgr->GetUserBySessionId(sessionId);
//     if(!user)
//         return;
// 
//     user->CheckHeart();
// }