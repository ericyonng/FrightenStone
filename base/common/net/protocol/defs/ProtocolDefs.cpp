/**
* @file ProtocolDefs.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#include "stdafx.h"
#include "base/common/net/protocol/defs/ProtocolDefs.h"

PacketHeader::PacketHeader()
{
    _packetLength = 0;
    _cmd = ProtocolCmd::CMD_Begin;
}

LoginReq::LoginReq()
{
    _packetLength = sizeof(LoginReq);
    _cmd = ProtocolCmd::LoginReq;
    memset(_userName, 0, sizeof(_userName));
    memset(_pwd, 0, sizeof(_pwd));
}

LoginRes::LoginRes()
{
    // 包头
    _packetLength = sizeof(LoginRes);
    _cmd = ProtocolCmd::LoginRes;

    // 数据成员
    memset(_userName, 0, sizeof(_userName));
    _status = StatusDefs::Success;
}

LoginNty::LoginNty()
{
    // 包头
    _packetLength = sizeof(LoginNty);
    _cmd = ProtocolCmd::LoginNty;

    // 数据成员
    memset(_userName, 0, sizeof(_userName));
    memset(_pwd, 0, sizeof(_pwd));
}

CreatePlayerNty::CreatePlayerNty()
{
    _packetLength = sizeof(CreatePlayerNty);
    _cmd = ProtocolCmd::CreatePlayerNty;
    _socket = 0;
}
