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

#pragma region 

#ifdef _WIN32
typedef unsigned long long MYSOCKET;
#else
typedef int MYSOCKET;

#endif

#ifndef SOCKET_CACHE_SIZE
#define SOCKET_CACHE_SIZE 102400
#endif // !SOCKET_CACHE_SIZE

#define MYINVALID_SOCKET static_cast<MYSOCKET>(~0)
#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif

#pragma endregion

#pragma region 

// –≠“È∫≈
class BASE_EXPORT ProtocolCmd
{
public:
    enum
    {
        CMD_Begin = 0,              // 
        LoginReq = 1,               // 
        LoginNty = 2,               // 
        LoginRes = 3,               // 
        LogoutReq = 4,              // 
        LogoutNty = 5,              // 
        LogoutRes = 6,              // 
        CreatePlayerReq = 7,        // 
        CreatePlayerRes = 8,        // 
        CreatePlayerNty = 9,        // 
    };
};

struct BASE_EXPORT PacketHeader
{
    PacketHeader();
    UInt16 _packetLength;           // 
    UInt16 _cmd;                    // 
};

struct BASE_EXPORT LoginReq : public PacketHeader
{
    LoginReq();
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
};

struct BASE_EXPORT LoginRes : public PacketHeader
{
    LoginRes();

    char _userName[MAX_NAME_LEN];
    Int32 _status;
};

struct BASE_EXPORT LoginNty : public PacketHeader
{
    LoginNty();
    
    char _userName[MAX_NAME_LEN];
    char _pwd[MAX_PWD_LEN];
};

struct BASE_EXPORT CreatePlayerNty : public PacketHeader
{
    CreatePlayerNty();

    Int32 _socket;
};

#pragma endregion

#endif // !__Base_Common_Net_Protocol_Defs_ProtocolDefs__H__
