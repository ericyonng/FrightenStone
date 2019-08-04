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
 * @file  : FS_Server.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/3
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_Server.h"
#include "base/common/log/Log.h"

FS_NAMESPACE_BEGIN

FS_Server::~FS_Server()
{
    g_Log->net("FS_Server%d.~FS_Server exit begin", _id);
    g_Log->sys(_LOGFMT_( "FS_Server%d.~FS_Server exit begin"), _id);
    Close();
    g_Log->net("FS_Server%d.~FS_Server exit begin", _id);
    g_Log->sys(_LOGFMT_("FS_Server%d.~FS_Server exit begin"), _id);
}

// 关闭Socket
void FS_Server::Close()
{
    g_Log->net("FS_Server%d.Close begin", _id);
    g_Log->sys(_LOGFMT_("FS_Server%d.Close begin"), _id);
    _threadPool->Clear();
    g_Log->net("FS_Server%d.Close end", _id);
    g_Log->sys(_LOGFMT_("FS_Server%d.Close end"), _id);
}

void FS_Server::NetMsgHandler(const FS_ThreadPool *pool)
{
    while(pool->IsClearingPool())
    {
        if(!_clientsBuff.empty())
        {// 从缓冲队列里取出客户数据
            _locker.Lock();
            for(auto client : _clientsBuff)
            {
                _clients[client->sockfd()] = client;
                client->serverId = _id;
                if(_eventHandleObj)
                    _eventHandleObj->OnNetJoin(client);
                OnClientJoin(client);
            }
            _clientsBuff.clear();
            _clientsChange = true;
            _locker.Unlock();
        }

        // 如果没有需要处理的客户端，就跳过
        if(_clients.empty())
        {
            CELLThread::Sleep(1);
            //旧的时间戳
            _oldTime = CELLTime::getNowInMilliSec();
            continue;
        }

        CheckTime();
        if(!DoNetEvents())
        {
            pThread->Exit();
            break;
        }
        DoMsg();
    }
    CELLLog_Info("CELLServer%d.OnRun exit", _id);
}

FS_NAMESPACE_END