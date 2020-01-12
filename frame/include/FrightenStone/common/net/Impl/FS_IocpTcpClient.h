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
 * @file  : FS_IocpTcpClient.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/05
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpTcpClient_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpTcpClient_H__

#pragma once

#ifdef _WIN32

#include <FrightenStone/exportbase.h>
#include <FrightenStone/common/basedefs/BaseDefs.h>
#include <FrightenStone/common/net/Impl/FS_TcpClient.h>
#include <FrightenStone/common/net/Impl/FS_Iocp.h>
#include <FrightenStone/common/net/Defs/IocpDefs.h>
#include <FrightenStone/common/net/Impl/FS_IocpSession.h>
#include <FrightenStone/common/net/Impl/FS_Addr.h>
#include "FrightenStone/common/net/Defs/IoEvDefs.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_IocpTcpClient : public FS_TcpClient
{
    OBJ_POOL_CREATE_DEF(FS_IocpTcpClient);
public:
    FS_IocpTcpClient();
    ~FS_IocpTcpClient();

    /* ÖØÐ´½Ó¿Ú */
public:
    virtual void OnInitSocket();
    virtual void OnConnect();
    virtual void Close();
    virtual bool OnRun(int microseconds = 1);

protected:
    Int32 DoIocpNetEvents(int microseconds);

private:
    FS_Iocp _iocp;
    IoEvent _ev;
};

FS_NAMESPACE_END

#include <FrightenStone/common/net/Impl/FS_IocpTcpClientImpl.h>

#endif

#endif
