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
 * @file  : ProtocolDefs.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *      !!!!注意发送数据到客户端时候iocp网络层使用的是内存池，所以请在网络层注意下，业务层可任意，业务层不适用内存池
 * 
 */
#ifndef __Protocols_Impl_Protocols_Defs_ProtocolDefs_H__
#define __Protocols_Impl_Protocols_Defs_ProtocolDefs_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/objpool/objpool.h"
#include "protocols/ProtocolInterface/protocol.h"

#pragma region 
#ifndef SOCKET_CACHE_SIZE
#define SOCKET_CACHE_SIZE 102400
#endif // !SOCKET_CACHE_SIZE

#define MYINVALID_SOCKET static_cast<MYSOCKET>(~0)
#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif
#pragma endregion

#pragma region 

FS_NAMESPACE_BEGIN

// 协议号
class ProtocolCmd
{
public:
    enum CmdEnums:UInt16
    {
        CMD_Begin = 0,              // 
        LoginReq = 10,               // 
        LoginNty = 2,               // 
        LoginRes = 11,               // 
        LogoutReq = 12,              // 
        LogoutNty = 13,              // 
        LogoutRes = 6,              // 
        CreatePlayerReq = 7,        // 
        CreatePlayerRes = 8,        // 
        CreatePlayerNty = 9,        // 
        CheckHeartReq = 15,         // 客户端心跳包
        CheckHeartRes = 16,         // 心跳包反馈，按理不用反馈给客户端减少服务端压力

        CheckNetReq = 17,           // 网络包测试
        CheckNetRes = 18,           // 网络包测试
        CMD_End,
    };

    static const char *GetStr(UInt16 cmd);
};

// struct BASE_EXPORT NetMsg_Buffer : public NetMsg_DataHeader
// {
//     OBJ_POOL_CREATE_ANCESTOR(NetMsg_Buffer);
//     NetMsg_Buffer();
// 
//     char _buffer[0];
// };

/*
*
* 网络消息请遵循:
*               1.读取网络消息务必DeserializeFrom,
*               2.发送网络消息需要SerializeTo,
*               3.避免因为内存对齐问题导致数据错乱
* 网络消息编码 :
                1.先bindbuffer,初始化缓冲区
                2.调用start,给包头长度位占位
                3.再调用SetCmd写入cmd消息协议号
                4.写入消息数据
                5.调用finish
                6.若有多个消息可以start->setcmd->write->finish循环

* 网络消息解码 :
                1.先调用decode预解码
                2.给消息协议号_cmd与长度赋值_packetLength
                3.调用Read按照消息体中数据顺序读取数据

* 注意         :
                1.serialize NetMsgCoder *coder的外部需要做绑定,start->setcmd,以及finish的工作
                2.DeserializeFrom  NetMsgDecoder *decoder 的外部需要预先Decode
                3.每个消息包的len在构造时需要对每个基本类型数据进行计算尺寸,不可直接sizeof包类型
                ,因为虚表以及内存对齐会导致len长度与包序列化的字节流长度不一致
*/

/////////////////////////////////////////////////////////
///
struct LoginData
{
    OBJ_POOL_CREATE_ANCESTOR(LoginData);
    LoginData();
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
    char _data[28];
    Int32 _msgId;
};

struct LoginReq : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(LoginReq, NetMsg_DataHeader);
    LoginReq();
    LoginData _loginData;

    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);
};

struct LoginRes : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(LoginRes, NetMsg_DataHeader);
    LoginRes();

    Int32 _result;
    char data[88];
    Int32 _msgId;

    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);
};

struct LoginNty : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(LoginNty, NetMsg_DataHeader);
    LoginNty();
    
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];

    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);
};

struct CreatePlayerNty : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(CreatePlayerNty, NetMsg_DataHeader);
    CreatePlayerNty();
    Int32 _socket;

    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);
};

struct CheckHeartReq : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(CheckHeartReq, NetMsg_DataHeader);
    CheckHeartReq();
    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);
};

struct CheckHeartRes : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(CheckHeartRes, NetMsg_DataHeader);
    CheckHeartRes();
    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);
};

// class BASE_EXPORT ProtocoalAssist
// {
// public:
//     static void DelNetMsg(NetMsg_DataHeader *header);
// };

struct CheckNetReq : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(CheckNetReq, NetMsg_DataHeader);

    CheckNetReq();

    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);

    Int64 _requireMsgId;
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
    char _data[24];
};

struct CheckNetRes : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE_DERIVE(CheckNetRes, NetMsg_DataHeader);

    CheckNetRes();

    virtual bool SerializeToByteBuffer(NetMsgCoder *coder);
    virtual void SerializeToStringBuffer(NetMsgCoder *coder);
    virtual bool DeserializeFrom(NetMsgDecoder *decoder);

    Int64 _recvMsgId;
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
    char _data[24];
};

FS_NAMESPACE_END

#pragma endregion

#endif // !__Frame_Include_FrightenStone_Common_Net_Protocol_Defs_ProtocolDefs_H__
