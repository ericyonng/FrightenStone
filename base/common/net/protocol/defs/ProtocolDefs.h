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
 *
 * 
 */
#ifndef __Base_Common_Net_Protocol_Defs_ProtocolDefs_H__
#define __Base_Common_Net_Protocol_Defs_ProtocolDefs_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/status/status.h"
#include "base/common/objpool/objpool.h"

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
    enum
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
};

struct BASE_EXPORT NetMsg_DataHeader
{
    OBJ_POOL_CREATE(NetMsg_DataHeader, _objPoolHelper);

    NetMsg_DataHeader();
    UInt16 _packetLength;           // 包长度
    UInt16 _cmd;                    // 命令
};

struct BASE_EXPORT LoginReq : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE(LoginReq, _objPoolHelper);
    LoginReq();
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
    char _data[28];
    Int32 _msgId;
};

struct BASE_EXPORT LoginRes : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE(LoginRes, _objPoolHelper);
    LoginRes();

    Int32 result;
    char data[88];
    Int32 _msgId;
};

struct BASE_EXPORT LoginNty : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE(LoginNty, _objPoolHelper);
    LoginNty();
    
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
};

struct BASE_EXPORT CreatePlayerNty : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE(CreatePlayerNty, _objPoolHelper);
    CreatePlayerNty();
    Int32 _socket;
};

struct BASE_EXPORT CheckHeartReq : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE(CheckHeartReq, _objPoolHelper);

    CheckHeartReq();
};

struct BASE_EXPORT CheckHeartRes : public NetMsg_DataHeader
{
    OBJ_POOL_CREATE(CheckHeartRes, _objPoolHelper);

    CheckHeartRes();
};

FS_NAMESPACE_END

#pragma endregion

#endif // !__Base_Common_Net_Protocol_Defs_ProtocolDefs_H__
