#ifndef __Base_Common_Net_Protocol_Defs_ProtocolDefs__H__
#define __Base_Common_Net_Protocol_Defs_ProtocolDefs__H__
/**
* @file ProtocolDefs.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/status/status.h"

#pragma region socket定义

// 数据类型定义
#ifdef _WIN32
typedef unsigned long long MYSOCKET;
#else
typedef int MYSOCKET;

#endif

// 宏缓冲区
#ifndef SOCKET_CACHE_SIZE
#define SOCKET_CACHE_SIZE 102400
#endif // !SOCKET_CACHE_SIZE

#define MYINVALID_SOCKET static_cast<MYSOCKET>(~0)
#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif

#pragma endregion

#pragma region 包数据结构定义

// 协议号
class BASE_EXPORT ProtocolCmd
{
public:
    enum
    {
        CMD_Begin = 0,              // 协议命令开始
        LoginReq = 1,               // 登陆
        LoginNty = 2,               // 登陆返回数据包
        LoginRes = 3,               // 登陆响应
        LogoutReq = 4,              // 登出注销
        LogoutNty = 5,              // 登出返回数据
        LogoutRes = 6,              // 登出响应
        CreatePlayerReq = 7,        // 创建角色
        CreatePlayerRes = 8,        // 创建角色响应
        CreatePlayerNty = 9,        // 创建角色
    };
};

struct BASE_EXPORT PacketHeader
{
    PacketHeader();
    UInt16 _packetLength;           // 当前包长度
    UInt16 _cmd;                    // 命令
};

// 登陆
struct BASE_EXPORT LoginReq : public PacketHeader
{
    LoginReq();
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
};

// 登陆
// 响应
struct BASE_EXPORT LoginRes : public PacketHeader
{
    LoginRes();

    char _userName[MAX_NAME_LEN];
    Int32 _status;
};

// 登陆
// 登陆后必要的数据推送
struct BASE_EXPORT LoginNty : public PacketHeader
{
    LoginNty();
    
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
};

// 
struct BASE_EXPORT CreatePlayerNty : public PacketHeader
{
    CreatePlayerNty();

    Int32 _socket;
};

#pragma endregion

#endif // !__Base_Common_Net_Protocol_Defs_ProtocolDefs__H__
