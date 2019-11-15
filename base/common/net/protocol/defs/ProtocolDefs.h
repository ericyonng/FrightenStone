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
 * @file  : ProtocolDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *      !!!!注意发送数据到客户端时候iocp网络层使用的是内存池，所以请在网络层注意下，业务层可任意，业务层不适用内存池
 * 
 */
#ifndef __Base_Common_Net_Protocol_Defs_ProtocolDefs_H__
#define __Base_Common_Net_Protocol_Defs_ProtocolDefs_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/status/status.h"
#include "base/common/memorypool/memorypool.h"

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
class BASE_EXPORT ProtocolCmd
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
        CMD_End,
    };

    static const char *GetStr(UInt16 cmd);
};

struct BASE_EXPORT NetMsg_DataHeader
{
    MEM_POOL_CREATE_DEF();

    NetMsg_DataHeader();
    
    // 提供序列化反序列化接口
    // 序列化反序列化（ObjType 必须有SerialzieTo(FS_Stream *), DeserializeFrom(FS_Stream *)接口）
    // 序列化反序列化（ObjType 必须有bool SerializeTo(FS_Stream *) const, bool DeserializeFrom(FS_Stream *)接口）
    // bool SerializeTo(FS_Stream *str);
    UInt16 _packetLength;           // 包长度
    UInt16 _cmd;                    // 命令
};

// struct BASE_EXPORT NetMsg_Buffer : public NetMsg_DataHeader
// {
//     OBJ_POOL_CREATE_DEF(NetMsg_Buffer);
//     NetMsg_Buffer();
// 
//     char _buffer[0];
// };

struct BASE_EXPORT LoginReq : public NetMsg_DataHeader
{
    MEM_POOL_CREATE_DEF();
    LoginReq();
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
    char _data[28];
    Int32 _msgId;
};

struct BASE_EXPORT LoginRes : public NetMsg_DataHeader
{
    MEM_POOL_CREATE_DEF();
    LoginRes();

    Int32 _result;
    char data[88];
    Int32 _msgId;
};

struct BASE_EXPORT LoginNty : public NetMsg_DataHeader
{
    MEM_POOL_CREATE_DEF();
    LoginNty();
    
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
};

struct BASE_EXPORT CreatePlayerNty : public NetMsg_DataHeader
{
    MEM_POOL_CREATE_DEF();
    CreatePlayerNty();
    Int32 _socket;
};

struct BASE_EXPORT CheckHeartReq : public NetMsg_DataHeader
{
    MEM_POOL_CREATE_DEF();

    CheckHeartReq();
};

struct BASE_EXPORT CheckHeartRes : public NetMsg_DataHeader
{
    MEM_POOL_CREATE_DEF();

    CheckHeartRes();
};

class BASE_EXPORT ProtocoalAssist
{
public:
    static void DelNetMsg(NetMsg_DataHeader *header);
};

FS_NAMESPACE_END

#pragma endregion

#endif // !__Base_Common_Net_Protocol_Defs_ProtocolDefs_H__
