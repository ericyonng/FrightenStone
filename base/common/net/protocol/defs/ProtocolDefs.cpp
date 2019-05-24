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
    _packetLength = sizeof(LoginRes);
    _cmd = ProtocolCmd::LoginRes;

    memset(_userName, 0, sizeof(_userName));
    _status = StatusDefs::Success;
}

LoginNty::LoginNty()
{
    _packetLength = sizeof(LoginNty);
    _cmd = ProtocolCmd::LoginNty;

    memset(_userName, 0, sizeof(_userName));
    memset(_pwd, 0, sizeof(_pwd));
}

CreatePlayerNty::CreatePlayerNty()
{
    _packetLength = sizeof(CreatePlayerNty);
    _cmd = ProtocolCmd::CreatePlayerNty;
    _socket = 0;
}
