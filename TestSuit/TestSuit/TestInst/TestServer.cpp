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
 * @file  : TestServer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "TestSuit/TestSuit/TestInst/TestServer.h"
#include "FrightenStone/exportbase.h"

FS_NAMESPACE_BEGIN
// 
// class User
// {
// public:
//     User(UInt64 sessionId, IFS_MsgDispatcher *dispatcher)
//         :_sessionId(sessionId)
//         ,_recvMsgId(1)
//         ,_sendMsgId(1)
//         ,_dispatcher(dispatcher)
//     {
//     }
//     ~User()
//     {
//     }
//     
//     void SendData(IFS_Session *session, NetMsg_DataHeader *msgData)
//     {
//         session->Send(msgData);
//     }
// 
//     UInt64 _sessionId;
//     // 用于server检测接收到的消息ID是否连续 每收到一个客户端消息会自增1以便与客户端的msgid校验，不匹配则报错处理（说明丢包等）
//     Int32 _recvMsgId = 1;
//     // 测试接收发逻辑用
//     // 用于client检测接收到的消息ID是否连续 每发送一个消息会自增1以便与客户端的sendmsgid校验，不匹配则客户端报错（说明丢包等）
//     Int32 _sendMsgId = 1;
//     IFS_MsgDispatcher *_dispatcher;
// };
// 
// class MyLogic : public IFS_BusinessLogic
// {
// public:
//     std::map<UInt64, User *> _users;
// 
// public:
//     MyLogic() {}
//     virtual ~MyLogic() 
//     {
//         STLUtil::DelMapContainer(_users);
//     }
//     virtual void Release()
//     {
//         delete this;
//     }
// 
// public:
//     User *GetUser(UInt64 sessionId)
//     {
//         auto iterUser = _users.find(sessionId);
//         if(iterUser == _users.end())
//             return NULL;
// 
//         return iterUser->second;
//     }
// 
//     void RemoveUser(UInt64 sessionId)
//     {
//         auto iterUser = _users.find(sessionId);
//         if(iterUser == _users.end())
//             return;
// 
//         Fs_SafeFree(iterUser->second);
//         _users.erase(iterUser);
//     }
// 
//     User *NewUser(UInt64 sessionId)
//     {
//         auto user = new User(sessionId, _dispatcher);
//         _users.insert(std::make_pair(sessionId, user));
//         return user;
//     }
// 
//     virtual Int32 Start()
//     {
//         return StatusDefs::Success;
//     }
// 
//     void BeforeClose()
//     {
//         STLUtil::DelMapContainer(_users);
//         g_Log->sys<MyLogic>(_LOGFMT_("MyLogic before closed remove all users"));
//     }
// 
//     virtual void Close()
//     {
//          g_Log->sys<MyLogic>(_LOGFMT_("MyLogic closed"));
//     }
// 
//     virtual void OnMsgDispatch(fs::IFS_Session *session, NetMsg_DataHeader *msgData)
//     {
//         auto sessionId = session->GetSessionId();
//         auto user = GetUser(sessionId);
//         if(!user)
//             user = NewUser(sessionId);
// 
//         // g_Log->any<MyLogic>("sessionid[%llu] handle a msg", sessionId);
//         switch(msgData->_cmd)
//         {
//             case fs::ProtocolCmd::LoginReq:
//             {
//                 fs::LoginReq *login = static_cast<fs::LoginReq *>(msgData);
// 
//                 //     g_Log->any<MyLogic>("sessionid[%llu] login username[%s], pwd[%s] msgId[%d] user recvmsgid"
//                 //                         , sessionId, login->_userName, login->_pwd, login->_msgId, user->_recvMsgId);
//                      // 检查消息ID
//                 if(login->_msgId != user->_recvMsgId)
//                 {//当前消息ID和本地收消息次数不匹配
//                     g_Log->e<MyLogic>(_LOGFMT_("OnMsgDispatch sessionId<%llu> msgID<%d> _nRecvMsgID<%d> diff<%d>")
//                                       , sessionId, login->_msgId
//                                       , user->_recvMsgId, login->_msgId - user->_recvMsgId);
//                 }
// 
//                 // 返回包
//                 ++user->_recvMsgId;
//                 fs::LoginRes ret;
//                 ret._msgId = user->_sendMsgId;
//                 user->SendData(session, &ret);
//                 ++user->_sendMsgId;
//                 return;
//             }//接收 消息---处理 发送   生产者 数据缓冲区  消费者 
//             break;
//             case fs::ProtocolCmd::LogoutReq:
//             {
//                 fs::FS_MsgReadStream r(msgData);
//                 // 读取消息长度
//                 r.ReadInt16();
//                 // 读取消息命令
//                 r.GetNetMsgCmd();
//                 auto n1 = r.ReadInt8();
//                 auto n2 = r.ReadInt16();
//                 auto n3 = r.ReadInt32();
//                 auto n4 = r.ReadFloat();
//                 auto n5 = r.ReadDouble();
//                 uint32_t n = 0;
//                 r.ReadWithoutOffsetPos(n);
//                 char name[32] = {};
//                 auto n6 = r.ReadArray(name, 32);
//                 char pw[32] = {};
//                 auto n7 = r.ReadArray(pw, 32);
//                 int ata[10] = {};
//                 auto n8 = r.ReadArray(ata, 10);
//                 ///
//                 fs::FS_MsgWriteStream s(128);
//                 s.SetNetMsgCmd(fs::ProtocolCmd::LogoutNty);
//                 s.WriteInt8(n1);
//                 s.WriteInt16(n2);
//                 s.WriteInt32(n3);
//                 s.WriteFloat(n4);
//                 s.WriteDouble(n5);
//                 s.WriteArray(name, n6);
//                 s.WriteArray(pw, n7);
//                 s.WriteArray(ata, n8);
//                 s.Finish();
// 
//                 // TODO:需要支持流发送
// //                 _dispatcher->SendData(sessionId, )
// //                 client->SendData(*s.GetDataAddr(), s.GetWrLength());
//                 //                 g_Log->i<EasyFSServer>(_LOGFMT_("socket<%d> logout")
//                 //                                        , static_cast<Int32>(client->GetSocket()));
//                 return;
//             }
//             break;
//             case fs::ProtocolCmd::CheckHeartReq:
//             {
//                 fs::CheckHeartRes ret;
//                 _dispatcher->SendData(sessionId, &ret);
//                 //g_Log->any("socket<%d> CheckHeartReq", static_cast<Int32>(client->GetSocket()));
//                 return;
//             }
//             default:
//             {
//                 g_Log->w<MyLogic>(_LOGFMT_("recv <sessionId=%llu> undefine msgType,dataLen：%hu")
//                                   , sessionId, msgData->_packetLength);
//             }
//             break;
//         }
// 
//         return;
//     }
//     virtual void OnMsgDispatch(UInt64 sessionId, NetMsg_DataHeader *msgData)
//     {
//        
//     }
//     virtual void OnSessionDisconnected(UInt64 sessionId)
//     {
//         RemoveUser(sessionId);
//          // g_Log->any<MyLogic>("sessionid[%llu] Disconnected", sessionId);
//     }
// 
// 
// };
// 
// class EasyFSServer : public fs::FS_MainIocpServer
// {
//     OBJ_POOL_CREATE_DEF(EasyFSServer);
// public:
//     EasyFSServer()
//     {
//         _isCheckMsgID = true;
//         _bSendBack = true;
//         _bSendFull = true;
//     }
// 
//     // FS_Server 4 多个线程触发 不安全
//     // 如果只开启1个cellServer就是安全的
//     virtual void OnNetJoin(fs::FS_Client * client)
//     {
//         fs::FS_TcpServer::OnNetJoin(client);
//         Int32 joinedCnt = _clientJoinedCnt;
//        // g_Log->net<EasyFSServer>("OnNetJoin joinedcnt[%d]", joinedCnt);
//         // g_Log->any("client<%d> joined curJoinedCnt[%d]", (Int32)(client->GetSocket()), joinedCnt);
// //         g_Log->i<EasyFSServer>(_LOGFMT_("socket<%d> joined, client joined cnt<%d>")
// //                                , static_cast<Int32>(client->GetSocket()), joinedCnt);
//     }
//     //cellServer 4 多个线程触发 不安全
//     //如果只开启1个cellServer就是安全的
//     virtual void OnNetLeave(fs::FS_Client *client)
//     {
//         fs::FS_TcpServer::OnNetLeave(client);
//         // Int32 joinedCnt = _clientJoinedCnt;
//         // g_Log->any("client<%d> leave curJoinedCnt[%d]", (Int32)(client->GetSocket()), joinedCnt);
// 
// //         g_Log->i<EasyFSServer>(_LOGFMT_("socket<%d> leave, client joined cnt<%d>")
// //                                , static_cast<Int32>(client->GetSocket()), joinedCnt);
//     }
//     //cellServer 4 多个线程触发 不安全
//     //如果只开启1个cellServer就是安全的
//     virtual Int32 OnNetMsg(fs::FS_Server *server, fs::FS_Client *client, fs::NetMsg_DataHeader *header)
//     {
//         fs::FS_TcpServer::OnNetMsg(server, client, header);
//         // Int32 recvMsgCnt = _recvMsgCount;
//         // g_Log->any("curRecv[%d]", recvMsgCnt);
//         switch(header->_cmd)
//         {
//             case fs::ProtocolCmd::LoginReq:
//             {
//                 fs::LoginReq *login = static_cast<fs::LoginReq *>(header);
// 
//                 // 检查消息ID
//                 if(_isCheckMsgID)
//                 {
//                     if(login->_msgId != client->_recvMsgId)
//                     {//当前消息ID和本地收消息次数不匹配
//                         g_Log->e<EasyFSServer>(_LOGFMT_("OnNetMsg socket<%d> msgID<%d> _nRecvMsgID<%d> diff<%d>")
//                                                , client->GetSocket(), login->_msgId
//                                                , client->_recvMsgId, login->_msgId - client->_recvMsgId);
//                     }
// 
//                     ++client->_recvMsgId;
//                 }
// 
// //                 g_Log->net<EasyFSServer>("<Recv>socket<%d> loginReq, userName[%s] pwd[%s] msgId[%d] "
// //                                         , static_cast<Int32>(client->GetSocket()), login->_userName, login->_pwd, login->_msgId);
//                 // 登录逻辑
// 
//                 // ......
//                 // 回应消息
//                 if(_bSendBack)
//                 {
//                     fs::LoginRes ret;
//                     ret._msgId = client->_sendMsgId;
// //                     g_Log->net<EasyFSServer>("<Send>socket<%d> LoginRes, _result[%d] msgId[%d] "
// //                                              , static_cast<Int32>(client->GetSocket()), ret._result, ret._msgId);
//                     client->SendData(&ret);
//                     ++client->_sendMsgId;
// 
// //                     if(SOCKET_ERROR == client->SendData(&ret))
// //                     {
// //                         // 发送缓冲区满了，消息没发出去,目前直接抛弃了
// //                         // 客户端消息太多，需要考虑应对策略
// //                         // 正常连接，业务客户端不会有这么多消息
// //                         // 模拟并发测试时是否发送频率过高
// //                         if(_bSendFull)
// //                             g_Log->w<EasyFSServer>(_LOGFMT_("<Socket=%d> Send Full"), client->GetSocket());
// //                     }
// //                     else {
// //                     }
// 
//                     // g_Log->net<EasyFSServer>("<Socket=%d> send login res", client->GetSocket());
//                 }
// 
//                 return StatusDefs::Success;
//                 //CELLLog_Info("recv <Socket=%d> msgType：CMD_LOGIN, dataLen：%d,userName=%s PassWord=%s", cSock, login->dataLength, login->userName, login->PassWord);
//             }//接收 消息---处理 发送   生产者 数据缓冲区  消费者 
//             break;
//             case fs::ProtocolCmd::LogoutReq:
//             {
//                 fs::FS_MsgReadStream r(header);
//                 // 读取消息长度
//                 r.ReadInt16();
//                 // 读取消息命令
//                 r.GetNetMsgCmd();
//                 auto n1 = r.ReadInt8();
//                 auto n2 = r.ReadInt16();
//                 auto n3 = r.ReadInt32();
//                 auto n4 = r.ReadFloat();
//                 auto n5 = r.ReadDouble();
//                 uint32_t n = 0;
//                 r.ReadWithoutOffsetPos(n);
//                 char name[32] = {};
//                 auto n6 = r.ReadArray(name, 32);
//                 char pw[32] = {};
//                 auto n7 = r.ReadArray(pw, 32);
//                 int ata[10] = {};
//                 auto n8 = r.ReadArray(ata, 10);
//                 ///
//                 fs::FS_MsgWriteStream s(128);
//                 s.SetNetMsgCmd(fs::ProtocolCmd::LogoutNty);
//                 s.WriteInt8(n1);
//                 s.WriteInt16(n2);
//                 s.WriteInt32(n3);
//                 s.WriteFloat(n4);
//                 s.WriteDouble(n5);
//                 s.WriteArray(name, n6);
//                 s.WriteArray(pw, n7);
//                 s.WriteArray(ata, n8);
//                 s.Finish();
//                 client->SendData(*s.GetDataAddr(), s.GetWrLength());
// //                 g_Log->i<EasyFSServer>(_LOGFMT_("socket<%d> logout")
// //                                        , static_cast<Int32>(client->GetSocket()));
//                 return StatusDefs::Success;
//             }
//             break;
//             case fs::ProtocolCmd::CheckHeartReq:
//             {
//                 fs::CheckHeartRes ret;
//                 client->SendData(&ret);
//                 //g_Log->any("socket<%d> CheckHeartReq", static_cast<Int32>(client->GetSocket()));
//                 return StatusDefs::Success;
//             }
//             default:
//             {
//                 g_Log->w<EasyFSServer>(_LOGFMT_("recv <socket=%d> undefine msgType,dataLen：%u")
//                                        , static_cast<Int32>(client->GetSocket()), header->_packetLength);
//             }
//             break;
//         }
// 
//         return StatusDefs::Unknown;
//     }
// 
// private:
//     //自定义标志 收到消息后将返回应答消息
//     bool _bSendBack;
//     //自定义标志 是否提示：发送缓冲区已写满
//     bool _bSendFull;
//     //是否检查接收到的消息ID是否连续
//     bool _isCheckMsgID;
// };

// OBJ_POOL_CREATE_DEF_IMPL(EasyFSServer, 10)


class User
{
public:
    User(UInt64 sessionId, IFS_MsgDispatcher *dispatcher)
        :_sessionId(sessionId)
        , _recvMsgId(1)
        , _sendMsgId(1)
        , _dispatcher(dispatcher)
    {
    }
    ~User()
    {
    }

    // NetMsg_DataHeader 必须是堆区创建的
    void SendData(UInt64 sessionId, UInt64 generatorId, NetMsg_DataHeader *msgData)
    {
        _dispatcher->SendData(sessionId, generatorId, msgData);
    }

    void OnDisconnect()
    {
        g_Log->net<User>(" session id[%llu] user disconnect", _sessionId);
    }
    UInt64 _sessionId;
    // 用于server检测接收到的消息ID是否连续 每收到一个客户端消息会自增1以便与客户端的msgid校验，不匹配则报错处理（说明丢包等）
    Int32 _recvMsgId = 1;
    // 测试接收发逻辑用
    // 用于client检测接收到的消息ID是否连续 每发送一个消息会自增1以便与客户端的sendmsgid校验，不匹配则客户端报错（说明丢包等）
    Int32 _sendMsgId = 1;
    IFS_MsgDispatcher *_dispatcher;
};

class MyLogic : public IFS_BusinessLogic
{
public:
    std::map<UInt64, User *> _users;

public:
    MyLogic() {}
    virtual ~MyLogic()
    {
        STLUtil::DelMapContainer(_users);
    }
    virtual void Release()
    {
        delete this;
    }

public:
    User * GetUser(UInt64 sessionId)
    {
        auto iterUser = _users.find(sessionId);
        if(iterUser == _users.end())
            return NULL;

        return iterUser->second;
    }

    void OnDisconnect(UInt64 sessionId)
    {
        auto iterUser = _users.find(sessionId);
        if(iterUser == _users.end())
            return;

        iterUser->second->OnDisconnect();
    }

    void RemoveUser(UInt64 sessionId)
    {
        auto iterUser = _users.find(sessionId);
        if(iterUser == _users.end())
            return;

        Fs_SafeFree(iterUser->second);
        _users.erase(iterUser);
    }

    User *NewUser(UInt64 sessionId)
    {
        auto user = new User(sessionId, _dispatcher);
        _users.insert(std::make_pair(sessionId, user));
        return user;
    }

    virtual Int32 Start()
    {
        return StatusDefs::Success;
    }

    void BeforeClose()
    {
        STLUtil::DelMapContainer(_users);
    }

    virtual void Close()
    {
    }

    virtual void OnSessionDisconnected(UInt64 sessionId)
    {
        auto user = GetUser(sessionId);
        if(user)
            user->OnDisconnect();

        RemoveUser(sessionId);
        // g_Log->any<MyLogic>("sessionid[%llu] Disconnected", sessionId);
    }

    virtual void OnSessionConnected(UInt64 sessionId)
    {
        NewUser(sessionId);
    }

    virtual void OnMsgDispatch(UInt64 sessionId, UInt64 generatorId, NetMsg_DataHeader *msgData)
    {
        auto user = GetUser(sessionId);
        if(!user)
            return;

        // g_Log->any<MyLogic>("sessionid[%llu] handle a msg", sessionId);
        switch(msgData->_cmd)
        {
            case fs::ProtocolCmd::LoginReq:
            {
                fs::LoginReq *login = static_cast<fs::LoginReq *>(msgData);

//                 g_Log->custom("user[%s] pwd[%s] login. at dispatcher[%d]"
//                               , login->_userName, login->_pwd, _dispatcher->GetId());
                //     g_Log->any<MyLogic>("sessionid[%llu] login username[%s], pwd[%s] msgId[%d] user recvmsgid"
                //                         , sessionId, login->_userName, login->_pwd, login->_msgId, user->_recvMsgId);
                     // 检查消息ID
                if(login->_msgId != user->_recvMsgId)
                {//当前消息ID和本地收消息次数不匹配
                    g_Log->custom("OnMsgDispatch sessionId<%llu> msgID<%d> _nRecvMsgID<%d> diff<%d>"
                                  , sessionId, login->_msgId
                                  , user->_recvMsgId, login->_msgId - user->_recvMsgId);
                }

                // 返回包
                ++user->_recvMsgId;

                for(Int32 i = 0; i < 1; ++i)
                {
                    fs::LoginRes ret;
                    ret._msgId = user->_sendMsgId;
                    user->SendData(sessionId, generatorId, &ret);
                    ++user->_sendMsgId;
                }

                return;
            }//接收 消息---处理 发送   生产者 数据缓冲区  消费者 
            break;
            case fs::ProtocolCmd::LogoutReq:
            {
                fs::FS_MsgReadStream r(msgData);
                // 读取消息长度
                r.ReadInt16();
                // 读取消息命令
                r.GetNetMsgCmd();
                auto n1 = r.ReadInt8();
                auto n2 = r.ReadInt16();
                auto n3 = r.ReadInt32();
                auto n4 = r.ReadFloat();
                auto n5 = r.ReadDouble();
                uint32_t n = 0;
                r.ReadWithoutOffsetPos(n);
                char name[32] = {};
                auto n6 = r.ReadArray(name, 32);
                char pw[32] = {};
                auto n7 = r.ReadArray(pw, 32);
                int ata[10] = {};
                auto n8 = r.ReadArray(ata, 10);
                ///
                fs::FS_MsgWriteStream s(128);
                s.SetNetMsgCmd(fs::ProtocolCmd::LogoutNty);
                s.WriteInt8(n1);
                s.WriteInt16(n2);
                s.WriteInt32(n3);
                s.WriteFloat(n4);
                s.WriteDouble(n5);
                s.WriteArray(name, n6);
                s.WriteArray(pw, n7);
                s.WriteArray(ata, n8);
                s.Finish();

                // TODO:需要支持流发送
//                 _dispatcher->SendData(sessionId, )
//                 client->SendData(*s.GetDataAddr(), s.GetWrLength());
                //                 g_Log->i<EasyFSServer>(_LOGFMT_("socket<%d> logout")
                //                                        , static_cast<Int32>(client->GetSocket()));
                return;
            }
            break;
            case fs::ProtocolCmd::CheckHeartReq:
            {
                fs::CheckHeartRes ret;
                //_dispatcher->SendData(sessionId, &ret);
                //g_Log->any("socket<%d> CheckHeartReq", static_cast<Int32>(client->GetSocket()));
                return;
            }
            default:
            {
                g_Log->custom("recv <sessionId=%llu> undefine msgType,dataLen：%hu"
                              , sessionId, msgData->_packetLength);
            }
            break;
        }

        return;
    }
};

FS_NAMESPACE_END


static union {
    char c[4];
    unsigned long l;
} endian_test = {{'l', '?', '?', 'b'}};

void TestServer::Run()
{
    endian_test;
    if((char)(endian_test.l) == 'b')
    {
        printf("big endian\n");
    }
    else
    {
        printf("little endian\n");
    }

    fs::FS_ServerCore *serverCore = new fs::FS_ServerCore();
    auto st = serverCore->Init();

    if(st == StatusDefs::Success)
    {
        // 并发业务逻辑
        auto dispatcherCnt = g_SvrCfg->GetDispatcherCnt();
        std::vector<fs::IFS_BusinessLogic *> logics;
        logics.resize(dispatcherCnt);
        for(Int32 i = 0; i < dispatcherCnt; ++i)
            logics[i] = new fs::MyLogic;
        auto st = serverCore->Start(logics);
        if(st == StatusDefs::Success)
        {
            getchar();
        }
        else
        {
            g_Log->e<TestServer>(_LOGFMT_("Start server fail st[%d] server will close now. please check! "));
        }
    }
    else
    {
        std::cout << "server core init fail." << std::endl;
    }

    //serverCore->Wait();
    serverCore->Close();
    Fs_SafeFree(serverCore);
    g_Log->FinishModule();
    std::cout << "free server core" << std::endl;
    getchar();
}
