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
 * @file  : FS_IocpServer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_IocpServer_H__
#define __Base_Common_Net_Impl_FS_IocpServer_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/net/Impl/FS_Server.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_Iocp;
struct BASE_EXPORT IO_EVENT;

 //网络消息接收处理服务类
class BASE_EXPORT FS_IocpServer :public FS_Server
{
public:
    FS_IocpServer();
    ~FS_IocpServer() noexcept;

protected:
    Int32 _BeforeClientMsgTransfer();
    //每次只处理一件网络事件
    //ret = -1 iocp出错
    //ret =  0 没有事件
    //ret =  1 有事件发生
    Int32 _ListenIocpNetEvents();
    void _RmClient(FS_Client *client);
    void _RmClient(IO_EVENT &ioEvent);
    void _OnClientJoin(FS_Client *client);

private:
    FS_Iocp *_iocp;
    IO_EVENT *_ioEvent;
};

FS_NAMESPACE_END

#endif

