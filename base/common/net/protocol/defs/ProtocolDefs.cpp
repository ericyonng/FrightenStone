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
 * @file  : ProtocolDefs.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/protocol/defs/ProtocolDefs.h"

#include "base/common/log/Log.h"

FS_NAMESPACE_BEGIN
const char * ProtocolCmd::GetStr(UInt16 cmd)
{
    switch(cmd)
    {
        case ProtocolCmd::LoginReq:
        {
            return "LoginReq";
        }
        break;
        case ProtocolCmd::LoginNty:
        {
            return "LoginNty";
        }
        break;
        case ProtocolCmd::LoginRes:
        {
            return "LoginRes";
        }
        break;
        case ProtocolCmd::LogoutReq:
        {
            return "LogoutReq";
        }
        break;
        case ProtocolCmd::LogoutNty:
        {
            return "LogoutNty";
        }
        break;
        case ProtocolCmd::LogoutRes:
        {
            return "LogoutRes";
        }
        break;
        case ProtocolCmd::CreatePlayerReq:
        {
            return "CreatePlayerReq";
        }
        break;
        case ProtocolCmd::CreatePlayerRes:
        {
            return "CreatePlayerRes";
        }
        break;
        case ProtocolCmd::CreatePlayerNty:
        {
            return "CreatePlayerNty";
        }
        break;
        case ProtocolCmd::CheckHeartReq:
        {
            return "CheckHeartReq";
        }
        break;
        case ProtocolCmd::CheckHeartRes:
        {
            return "CheckHeartRes";
        }
        break;
        default:
        {
            g_Log->w<ProtocolCmd>(_LOGFMT_("unknown net msg cmd[%hu]"), cmd);
        }
        break;
    }

    return "";
}

OBJ_POOL_CREATE_DEF_IMPL(NetMsg_DataHeader, __DEF_OBJ_POOL_OBJ_NUM__)
NetMsg_DataHeader::NetMsg_DataHeader()
{
    _packetLength = 0;
    _cmd = ProtocolCmd::CMD_Begin;
}

// OBJ_POOL_CREATE_DEF_IMPL(NetMsg_Buffer, __DEF_OBJ_POOL_OBJ_NUM__)
// NetMsg_DataHeader::NetMsg_DataHeader()
// {
// }

OBJ_POOL_CREATE_DEF_IMPL(LoginReq, __DEF_OBJ_POOL_OBJ_NUM__)
LoginReq::LoginReq()
{
    _packetLength = sizeof(LoginReq);
    _cmd = ProtocolCmd::LoginReq;
    memset(_userName, 0, sizeof(_userName));
    memset(_pwd, 0, sizeof(_pwd));
}

OBJ_POOL_CREATE_DEF_IMPL(LoginRes, __DEF_OBJ_POOL_OBJ_NUM__)
LoginRes::LoginRes()
{
    _packetLength = sizeof(LoginRes);
    _cmd = ProtocolCmd::LoginRes;
    _result = 0;
}

OBJ_POOL_CREATE_DEF_IMPL(LoginNty, __DEF_OBJ_POOL_OBJ_NUM__)
LoginNty::LoginNty()
{
    _packetLength = sizeof(LoginNty);
    _cmd = ProtocolCmd::LoginNty;

    memset(_userName, 0, sizeof(_userName));
    memset(_pwd, 0, sizeof(_pwd));
}

OBJ_POOL_CREATE_DEF_IMPL(CreatePlayerNty, __DEF_OBJ_POOL_OBJ_NUM__)
CreatePlayerNty::CreatePlayerNty()
{
    _packetLength = sizeof(CreatePlayerNty);
    _cmd = ProtocolCmd::CreatePlayerNty;
    _socket = 0;
}

OBJ_POOL_CREATE_DEF_IMPL(CheckHeartReq, __DEF_OBJ_POOL_OBJ_NUM__)
CheckHeartReq::CheckHeartReq()
{
    _packetLength = sizeof(CheckHeartReq);
    _cmd = ProtocolCmd::CheckHeartReq;
}

OBJ_POOL_CREATE_DEF_IMPL(CheckHeartRes, __DEF_OBJ_POOL_OBJ_NUM__)
CheckHeartRes::CheckHeartRes()
{
    _packetLength = sizeof(CheckHeartRes);
    _cmd = ProtocolCmd::CheckHeartRes;
}

void ProtocoalAssist::DelNetMsg(NetMsg_DataHeader *header)
{
    switch(header->_cmd)
    {
        case ProtocolCmd::LoginReq:
        {
            delete static_cast<LoginReq *>(header);
            header = NULL;
        }
        break;
        case ProtocolCmd::LoginNty:
        {
            delete static_cast<LoginNty *>(header);
            header = NULL;
        }
        break;
        case ProtocolCmd::LoginRes:
        {
            delete static_cast<LoginRes *>(header);
            header = NULL;
        }
        break;
        case ProtocolCmd::CreatePlayerNty:
        {
            delete static_cast<CreatePlayerNty *>(header);
            header = NULL;
        }
        break;
        case ProtocolCmd::CheckHeartReq:
        {
            delete static_cast<CheckHeartReq *>(header);
            header = NULL;
        }
        break;
        case ProtocolCmd::CheckHeartRes:
        {
            delete static_cast<CheckHeartRes *>(header);
            header = NULL;
        }
        break;
        default:
        {
            g_Log->e<ProtocoalAssist>(_LOGFMT_("unknown net msg cmd[%hu] data size[%hu]"), header->_cmd, header->_packetLength);
        }
        break;
    }
}


FS_NAMESPACE_END
