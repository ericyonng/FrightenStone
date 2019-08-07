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
    bool _OnClientStatusDirtied()
    {
        FS_Client *client = NULL;
        for(auto iter = _clients.begin(); iter != _clients.end(); )
        {
            client = iter->second;
            // 需要写数据的客户端,才postSend
            if(pClient->needWrite())
            {
                auto pIoData = pClient->makeSendIoData();
                if(pIoData)
                {
                    if(!_iocp.postSend(pIoData))
                    {
                        OnClientLeave(pClient);
                        iter = _clients.erase(iter);
                        continue;
                    }
                }
                //--
                pIoData = pClient->makeRecvIoData();
                if(pIoData)
                {
                    if(!_iocp.postRecv(pIoData))
                    {
                        OnClientLeave(pClient);
                        iter = _clients.erase(iter);
                        continue;
                    }
                }
            }
            else {
                auto pIoData = pClient->makeRecvIoData();
                if(pIoData)
                {
                    if(!_iocp.postRecv(pIoData))
                    {
                        OnClientLeave(pClient);
                        iter = _clients.erase(iter);
                        continue;
                    }
                }
            }
            iter++;
        }

        //---
        while(true)
        {
            int ret = DoIocpNetEvents();
            if(ret < 0)
            {
                return false;
            }
            else if(ret == 0)
            {
                return true;
            }
        }
        return true;
    }
    //每次只处理一件网络事件
    //ret = -1 iocp出错
    //ret =  0 没有事件
    //ret =  1 有事件发生
    int DoIocpNetEvents()
    {
        int ret = _iocp.wait(_ioEvent, 1);
        if(ret < 0)
        {
            CELLLog_Error("CELLIOCPServer%d.OnRun.wait", _id);
            return ret;
        }
        else if(ret == 0)
        {
            return ret;
        }
        //---

        //接收数据 完成 Completion
        if(IO_TYPE::RECV == _ioEvent.pIoData->iotype)
        {
            if(_ioEvent.bytesTrans <= 0)
            {//客户端断开处理
                //CELLLog_Info("rmClient sockfd=%d, IO_TYPE::RECV bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
                rmClient(_ioEvent);
                return ret;
            }
            //
            CELLClient* pClient = (CELLClient*)_ioEvent.data.ptr;
            if(pClient)
            {
                pClient->recv4iocp(_ioEvent.bytesTrans);
                OnNetRecv(pClient);
            }
            //
            //CELLLog_Info("IO_TYPE::RECV sockfd=%d, bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
        }
        //发送数据 完成 Completion
        else if(IO_TYPE::SEND == _ioEvent.pIoData->iotype)
        {
            if(_ioEvent.bytesTrans <= 0)
            {//客户端断开处理
                //CELLLog_Info("rmClient sockfd=%d, IO_TYPE::SEND bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
                rmClient(_ioEvent);
                return ret;
            }
            //
            CELLClient* pClient = (CELLClient*)_ioEvent.data.ptr;
            if(pClient)
                pClient->send2iocp(_ioEvent.bytesTrans);
            //
            //CELLLog_Info("IO_TYPE::SEND sockfd=%d, bytesTrans=%d", _ioEvent.pIoData->sockfd, _ioEvent.bytesTrans);
        }
        else {
            CELLLog_Warring("undefine io type.");
        }
        return ret;
    }

    void rmClient(CELLClient* pClient)
    {
        auto iter = _clients.find(pClient->sockfd());
        if(iter != _clients.end())
            _clients.erase(iter);
        //
        OnClientLeave(pClient);
    }

    void rmClient(IO_EVENT& ioEvent)
    {
        CELLClient* pClient = (CELLClient*)_ioEvent.data.ptr;
        if(pClient)
            rmClient(pClient);
    }

    void OnClientJoin(CELLClient* pClient)
    {
        _iocp.reg(pClient->sockfd(), pClient);
        auto pIoData = pClient->makeRecvIoData();
        if(pIoData)
            _iocp.postRecv(pIoData);
    }
private:
    FS_Iocp *_iocp;
    IO_EVENT *_ioEvent;
};

FS_NAMESPACE_END

#endif

