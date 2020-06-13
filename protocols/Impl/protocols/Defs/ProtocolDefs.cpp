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
 * @file  : ProtocolDefs.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "protocols/Defs/ProtocolDefs.h"

#include "FrightenStone/common/log/Log.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_ANCESTOR_IMPL(LoginData, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(LoginReq, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(LoginRes, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(LoginNty, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(CreatePlayerNty, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(CheckHeartReq, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(CheckHeartRes, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(CheckNetReq, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DERIVE_IMPL(CheckNetRes, __DEF_OBJ_POOL_OBJ_NUM__);

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
        case ProtocolCmd::CheckNetReq:
            return "CheckNetReq";
        case ProtocolCmd::CheckNetRes:
            return "CheckNetRes";
        default:
        {
            g_Log->w<ProtocolCmd>(_LOGFMT_("unknown net msg cmd[%hu]"), cmd);
        }
        break;
    }

    return "";
}

// OBJ_POOL_CREATE_DEF_IMPL(NetMsg_Buffer, __DEF_OBJ_POOL_OBJ_NUM__)
// NetMsg_DataHeader::NetMsg_DataHeader()
// {
// }

LoginData::LoginData()
    :_data{0}
    ,_msgId(0)
{
    memset(_userName, 0, sizeof(_userName));
    memset(_pwd, 0, sizeof(_pwd));
}

LoginReq::LoginReq()
{
//     _packetLength = _msgHeaderSize +
//         sizeof(_loginData._userName) + sizeof(_loginData._pwd) +
//         sizeof(_loginData._data) + sizeof(_loginData._msgId);
    _packetLength = sizeof(LoginReq);
    _cmd = ProtocolCmd::LoginReq;

}

bool LoginReq::SerializeToByteBuffer(NetMsgCoder *coder)
{
    // 写入包体数据
    coder->WriteByte(_loginData._userName, sizeof(_loginData._userName));
    coder->WriteByte(_loginData._pwd, sizeof(_loginData._pwd));
    coder->WriteByte(_loginData._data, sizeof(_loginData._data));
    coder->WriteByte(_loginData._msgId);
    return true;
}

void LoginReq::SerializeToStringBuffer(NetMsgCoder *coder)
{
    // 写入包体数据
    coder->WriteToString(_loginData._userName, sizeof(_loginData._userName));
    coder->WriteToString(_loginData._pwd, sizeof(_loginData._pwd));
    coder->WriteToString(_loginData._data, sizeof(_loginData._data));
    coder->WriteToString(_loginData._msgId);
}

bool LoginReq::DeserializeFrom(NetMsgDecoder *decoder)
{
    // 2.读取包体数据
    decoder->Read(_loginData._userName, sizeof(_loginData._userName));
    decoder->Read(_loginData._pwd, sizeof(_loginData._pwd));
    decoder->Read(_loginData._data, sizeof(_loginData._data));
    decoder->Read(_loginData._msgId);

    return true;
}

LoginRes::LoginRes()
{
//     _packetLength =_msgHeaderSize+
//         sizeof(_result) + 
//         sizeof(data) + 
//         sizeof(_msgId);
    _packetLength = sizeof(LoginRes);
    _cmd = ProtocolCmd::LoginRes;
    _result = 0;
}

bool LoginRes::SerializeToByteBuffer(NetMsgCoder *coder)
{
    coder->WriteByte(_result);
    coder->WriteByte(data, sizeof(data));
    coder->WriteByte(_msgId);
    return true;
}

void LoginRes::SerializeToStringBuffer(NetMsgCoder *coder)
{
    coder->WriteToString(_result);
    coder->WriteToString(data, sizeof(data));
    coder->WriteToString(_msgId);
}

bool LoginRes::DeserializeFrom(NetMsgDecoder *decoder)
{
    // 2.读取数据
    decoder->Read(_result);
    decoder->Read(data, sizeof(data));
    decoder->Read(_msgId);
    return true;
}

LoginNty::LoginNty()
{
//    _packetLength = _msgHeaderSize + sizeof(_userName) + sizeof(_pwd);
    _packetLength = sizeof(LoginNty);
    _cmd = ProtocolCmd::LoginNty;

    memset(_userName, 0, sizeof(_userName));
    memset(_pwd, 0, sizeof(_pwd));
}

bool LoginNty::SerializeToByteBuffer(NetMsgCoder *coder)
{
    coder->WriteByte(_userName, sizeof(_userName));
    coder->WriteByte(_pwd, sizeof(_pwd));
    return true;
}

void LoginNty::SerializeToStringBuffer(NetMsgCoder *coder)
{
    coder->WriteToString(_userName, sizeof(_userName));
    coder->WriteToString(_pwd, sizeof(_pwd));
}

bool LoginNty::DeserializeFrom(NetMsgDecoder *decoder)
{
    // 2.读取数据
    decoder->Read(_userName, sizeof(_userName));
    decoder->Read(_pwd, sizeof(_pwd));
    return true;
}

CreatePlayerNty::CreatePlayerNty()
{
    // _packetLength = _msgHeaderSize + sizeof(_socket);
    _packetLength = sizeof(CreatePlayerNty);
    _cmd = ProtocolCmd::CreatePlayerNty;
    _socket = 0;
}

bool CreatePlayerNty::SerializeToByteBuffer(NetMsgCoder *coder)
{
    coder->WriteByte(_socket);
    return true;
}

void CreatePlayerNty::SerializeToStringBuffer(NetMsgCoder *coder)
{
    coder->WriteToString(_socket);
}

bool CreatePlayerNty::DeserializeFrom(NetMsgDecoder *decoder)
{
    // 2.读取数据
    decoder->Read(_socket);
    return true;
}

CheckHeartReq::CheckHeartReq()
{
    // _packetLength = _msgHeaderSize;
    _packetLength = sizeof(CheckHeartReq);
    _cmd = ProtocolCmd::CheckHeartReq;
}

bool CheckHeartReq::SerializeToByteBuffer(NetMsgCoder *coder)
{
    return true;
}

void CheckHeartReq::SerializeToStringBuffer(NetMsgCoder *coder)
{
}

bool CheckHeartReq::DeserializeFrom(NetMsgDecoder *decoder)
{
    return true;
}

CheckHeartRes::CheckHeartRes()
{
    // _packetLength = _msgHeaderSize;
    _packetLength = sizeof(CheckHeartRes);
    _cmd = ProtocolCmd::CheckHeartRes;
}


bool CheckHeartRes::SerializeToByteBuffer(NetMsgCoder *coder)
{
    return true;
}

void CheckHeartRes::SerializeToStringBuffer(NetMsgCoder *coder)
{

}

bool CheckHeartRes::DeserializeFrom(NetMsgDecoder *decoder)
{
    return true;
}


CheckNetReq::CheckNetReq()
    :_requireMsgId(0)
{
    // _packetLength = _msgHeaderSize;
    _packetLength = sizeof(CheckNetReq);
    _cmd = ProtocolCmd::CheckNetReq;
}



bool CheckNetReq::SerializeToByteBuffer(NetMsgCoder *coder)
{
    // 写入包体数据
    coder->WriteByte(_requireMsgId);
    coder->WriteByte(_userName, sizeof(_userName));
    coder->WriteByte(_pwd, sizeof(_pwd));
    coder->WriteByte(_data, sizeof(_data));
    return true;
}

void CheckNetReq::SerializeToStringBuffer(NetMsgCoder *coder)
{
    coder->WriteToString(_requireMsgId);
    coder->WriteToString(_userName, sizeof(_userName));
    coder->WriteToString(_pwd, sizeof(_pwd));
    coder->WriteToString(_data, sizeof(_data));
}

bool CheckNetReq::DeserializeFrom(NetMsgDecoder *decoder)
{
    decoder->Read(_requireMsgId);
    decoder->Read(_userName, sizeof(_userName));
    decoder->Read(_pwd, sizeof(_pwd));
    decoder->Read(_data, sizeof(_data));
    return true;
}

CheckNetRes::CheckNetRes()
    :_recvMsgId(0)
{
    _packetLength = sizeof(CheckNetRes);
    _cmd = ProtocolCmd::CheckNetRes;
}

bool CheckNetRes::SerializeToByteBuffer(NetMsgCoder *coder)
{
    coder->WriteByte(_recvMsgId);
    coder->WriteByte(_userName, sizeof(_userName));
    coder->WriteByte(_pwd, sizeof(_pwd));
    coder->WriteByte(_data, sizeof(_data));
    return true;
}

void CheckNetRes::SerializeToStringBuffer(NetMsgCoder *coder)
{
    coder->WriteToString(_recvMsgId);
    coder->WriteToString(_userName, sizeof(_userName));
    coder->WriteToString(_pwd, sizeof(_pwd));
    coder->WriteToString(_data, sizeof(_data));
}

bool CheckNetRes::DeserializeFrom(NetMsgDecoder *decoder)
{
    decoder->Read(_recvMsgId);
    decoder->Read(_userName, sizeof(_userName));
    decoder->Read(_pwd, sizeof(_pwd));
    decoder->Read(_data, sizeof(_data));
    return true;
}

// 
// void ProtocoalAssist::DelNetMsg(NetMsg_DataHeader *header)
// {
//     switch(header->_cmd)
//     {
//         case ProtocolCmd::LoginReq:
//         {
//             delete static_cast<LoginReq *>(header);
//             header = NULL;
//         }
//         break;
//         case ProtocolCmd::LoginNty:
//         {
//             delete static_cast<LoginNty *>(header);
//             header = NULL;
//         }
//         break;
//         case ProtocolCmd::LoginRes:
//         {
//             delete static_cast<LoginRes *>(header);
//             header = NULL;
//         }
//         break;
//         case ProtocolCmd::CreatePlayerNty:
//         {
//             delete static_cast<CreatePlayerNty *>(header);
//             header = NULL;
//         }
//         break;
//         case ProtocolCmd::CheckHeartReq:
//         {
//             delete static_cast<CheckHeartReq *>(header);
//             header = NULL;
//         }
//         break;
//         case ProtocolCmd::CheckHeartRes:
//         {
//             delete static_cast<CheckHeartRes *>(header);
//             header = NULL;
//         }
//         break;
//         default:
//         {
//             g_Log->e<ProtocoalAssist>(_LOGFMT_("unknown net msg cmd[%hu] data size[%hu]"), header->_cmd, header->_packetLength);
//         }
//         break;
//     }
// }


FS_NAMESPACE_END
