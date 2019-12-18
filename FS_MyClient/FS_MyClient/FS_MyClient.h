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
 * @file  : FS_MyClient.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/6
 * @brief :
 * 
 *
 * 
 */
#ifndef __FS_MyClient_FS_MyClient_FS_MyClient_H__
#define __FS_MyClient_FS_MyClient_FS_MyClient_H__

#include <FrightenStone/exportbase.h>

#pragma once

#ifdef _WIN32

class FS_MyClient : public fs::FS_IocpTcpClient
{
public:
    FS_MyClient();
    ~FS_MyClient();

public:
    Int32 Init();
    virtual void OnNetMsg(fs::NetMsg_DataHeader *header);
    Int32 SendTest(fs::LoginReq *login);
    bool CheckSend(time_t dt);

public:
    // 发送时间计数
    time_t _restTime = 0; // 毫秒
private:
    // 接收消息id计数
    int _recvMsgID = 1;
    // 发送消息id计数
    int _sendMsgID = 1;
    // 发送条数计数
    int _sendCount = 0;
    // 检查接收到的服务端消息ID是否连续
    bool _checkMsgID = false;
    //
    bool _isSend = false;
};

class ClientTask : public fs::ITask
{
public:
    ClientTask(fs::FS_ThreadPool *pool, Int32 id);
    ~ClientTask();

    virtual Int32 Run();

private:
    Int32 _id;
    fs::FS_ThreadPool *_pool;
};

class FS_ClientRun
{
public:
    static void Run();
};
#endif

#endif