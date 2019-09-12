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
 * @file  : FS_Packet.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/31
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Defs/FS_Packet.h"
#include "base/common/log/Log.h"
#include "base/common/crashhandle/CrashHandle.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_DEF_IMPL(FS_Packet, __DEF_OBJ_POOL_OBJ_NUM__)

void FS_Packet::Pop(Int32 len)
{
    Int32 n = _lastPos - len;
    if(n > 0)
        memcpy(_buff, _buff + len, n);
    else if(n < 0)
    {
        n = 0;
        g_Log->e<FS_Packet>(_LOGFMT_("_ownerId[%llu] n is negative lastPos[%d] len[%d]"),
                               _ownerId, _lastPos, len);
    }

    _lastPos = n;
}

IO_DATA_BASE *FS_Packet::MakeRecvIoData()
{
    int len = _packetSize - _lastPos;
    if(len > 0)
    {
        _ioData._wsaBuff.buf = _buff + _lastPos;
        _ioData._wsaBuff.len = len;
        _ioData._ownerId = _ownerId;
        _ioData._sock = _socket;
        if(!_ioData._completedCallback)
            _ioData._completedCallback = DelegatePlusFactory::Create(this, &FS_Packet::_OnRecvSucCallback);

        if(!_ioData._completedCallback)
            g_Log->e<FS_Packet>(_LOGFMT_("memory cant create a delegate now stack trace back:\n%s")
                                , CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return &_ioData;
    }

    return NULL;
}

IO_DATA_BASE *FS_Packet::MakeSendIoData()
{
    if(_lastPos > 0)
    {
        _ioData._wsaBuff.buf = _buff;
        _ioData._wsaBuff.len = _lastPos;
        _ioData._ownerId = _ownerId;
        _ioData._sock = _socket;
        if(!_ioData._completedCallback)
            _ioData._completedCallback = DelegatePlusFactory::Create(this, &FS_Packet::_OnSendSucCallback);

        if(!_ioData._completedCallback)
            g_Log->e<FS_Packet>(_LOGFMT_("memory cant create a delegate now stack trace back:\n%s")
                                , CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return &_ioData;
    }

    return NULL;
}

void FS_Packet::_OnSendSucCallback(Int32 transferBytes)
{
    // 写入iocp完成多少字节则buffer响应减少多少字节
    if(_lastPos < transferBytes)
    {
        g_Log->e<FS_Packet>(_LOGFMT_("_ownerId[%llu] packetsize<%d> _lastPos<%d> transferBytes<%d>")
                               , _ioData._ownerId, _packetSize, _lastPos, transferBytes);
        return;
    }

    if(_lastPos == transferBytes)
    {// _lastPos=2000 实际发送sendBytes=2000
     // 数据尾部位置清零
        _lastPos = 0;
    }
    else {
        // _lastPos=2000 实际发送ret=1000
        _lastPos -= transferBytes;
        memcpy(_buff, _buff + transferBytes, _lastPos);
    }
}

void FS_Packet::_OnRecvSucCallback(Int32 transferBytes)
{
    // 从iocp读入buffer则buffer数据增加相应字节
    if(transferBytes > 0 && _packetSize - _lastPos >= transferBytes)
    {
        _lastPos += transferBytes;
        return;
    }

    g_Log->e<FS_Packet>(_LOGFMT_("_ownerId<%llu> _packetSize<%d> _lastPos<%d> transferBytes<%d>")
                           , _ioData._ownerId, _packetSize, _lastPos, transferBytes);
}

FS_NAMESPACE_END

