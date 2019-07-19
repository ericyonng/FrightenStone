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
 * @file  : FS_Iocp.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/17
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_Iocp_FS_Iocp_H__
#define __Base_Common_Net_Impl_Iocp_FS_Iocp_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include<MSWSock.h>

FS_NAMESPACE_BEGIN

struct IO_DATA_BASE;
struct IO_EVENT;

class BASE_EXPORT FS_Iocp
{
public:
    FS_Iocp();
    virtual ~FS_Iocp();

    /* ³õÊ¼»¯Óë×¢²á */
    Int32 Create();
    void Destroy();
    Int32 Reg(SOCKET sockfd);
    Int32 LoadAcceptEx(SOCKET listenSocket);

    /* Post Request */
    Int32 PostAccept(SOCKET listenSocket, IO_DATA_BASE *ioData);
    Int32 PostRecv(IO_DATA_BASE *ioData);
    Int32 PostSend(IO_DATA_BASE *ioData);

    Int32 WaitForComplete(IO_EVENT &ioEvent, ULong millisec = INFINITE);

private:

private:
    HANDLE _completionPort = NULL;
    LPFN_ACCEPTEX _fnAcceptEx = NULL;
};

FS_NAMESPACE_END


#endif
