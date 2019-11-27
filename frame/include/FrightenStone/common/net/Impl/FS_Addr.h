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
 * @file  : FS_Addr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 *  
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_Addr_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_Addr_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/component/Impl/FS_String.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT IFS_Session;

class BASE_EXPORT FS_Addr
{
    OBJ_POOL_CREATE_DEF(FS_Addr);
public:
    FS_Addr(IFS_Session *session, const sockaddr_in *addrInfo);
    ~FS_Addr();

public:
    const FS_String &GetAddr() const;
    UInt16 GetPort() const;
    bool UpdateAddrInfo();
    FS_String ToString() const;

private:
    BUFFER64 _ip;
    FS_String _strIp;
    char *_ipPtr;
    UInt16 _port;
    sockaddr_in _addrInfo;
    IFS_Session *_session;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_AddrImpl.h"

#endif
