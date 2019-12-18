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
 * @file  : FS_MyClient.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/6
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include <FS_MyClient/FS_MyClient/FS_MyClient.h>

static fs::FS_ClientCfgMgr g_CfgMgr;

#ifdef _WIN32

FS_MyClient::FS_MyClient()
{

}

FS_MyClient::~FS_MyClient()
{
}

Int32 FS_MyClient::Init()
{
    _checkMsgID = g_CfgMgr.NeedCheckMsgId();

    return  fs::FS_IocpTcpClient::Init();
}

void FS_MyClient::OnNetMsg(fs::NetMsg_DataHeader *header)
{
    _isSend = false;
    switch(header->_cmd)
    {
        case fs::ProtocolCmd::LoginRes:
        {
            fs::LoginRes *loginRes = static_cast<fs::LoginRes *>(header);
            if(_checkMsgID)
            {
                if(loginRes->_msgId != _recvMsgID)
                {// 当前消息ID和本地收消息次数不匹配
                    g_Log->e<FS_MyClient>(_LOGFMT_("OnNetMsg sessionId<%llu> socket<%llu> msgID<%d> _nRecvMsgID<%d> diff<%d>")
                                          , _session->GetSessionId()
                                          , _session->GetSocket()
                                          , loginRes->_msgId
                                          , _recvMsgID
                                          , loginRes->_msgId - _recvMsgID);
                }
                ++_recvMsgID;
            }
            //CELLLog_Info("<socket=%d> recv msgType：CMD_LOGIN_RESULT", (int)_pClient->sockfd());
        }
        break;
        default:
        {
            g_Log->w<FS_MyClient>(_LOGFMT_("error, <sessionId<%llu>, socket=%llu, cmd<%hu> len<%hu>> recv undefine msgType")
                                  , _session->GetSessionId(), _session->GetSocket()
                                  , header->_cmd, header->_packetLength);
        }
    }
}

Int32 FS_MyClient::SendTest(fs::LoginReq *login)
{
    Int32 ret = 0;
    //如果剩余发送次数大于0
    if(_sendCount > 0 && !_isSend)
    {
        login->_msgId = _sendMsgID;
        ret = SendData(login);
        //CELLLog_Info("%d", _nSendMsgID);
        if(SOCKET_ERROR != ret)
        {
            _isSend = g_CfgMgr.IsSendAfterSvrResArrive();
            ++_sendMsgID;
            // 如果剩余发送次数减少一次
            --_sendCount;
        }
    }

    return ret;
}

bool FS_MyClient::CheckSend(time_t dt)
{
    _restTime += dt;
    // 每经过nSendSleep毫秒
    auto sleepIntval = g_CfgMgr.GetSendPeriod();
    sleepIntval = sleepIntval > 0 ? sleepIntval : 1;

    if(_restTime >= sleepIntval)
    {
        // 重置计时
        _restTime -= sleepIntval;
        // 重置发送计数
        _sendCount = g_CfgMgr.GetMsgNumPerPeriod();
    }

    return _sendCount > 0;
}



void OnTaskRun(fs::FS_ThreadPool *pool)
{
    
}

ClientTask::ClientTask(fs::FS_ThreadPool *pool, Int32 id)
    :_id(id)
    ,_pool(pool)
{

}

ClientTask::~ClientTask()
{
}

std::atomic_int g_SendCount(0);
std::atomic_int g_ReadyCount(0);
std::atomic_int g_ConnectNum(0);

Int32 ClientTask::Run()
{
    g_Log->i<ClientTask>(_LOGFMT_("thead<%d>,start"), _id);

    // 参数
    const auto clientQuantity = g_CfgMgr.GetClientQuantity();
    const auto threadQuantity = g_CfgMgr.GetThreadQuantity();
    const auto sendSleepInterval = std::max<Int32>(g_CfgMgr.GetSendPeriod(), 1);
    const auto &ip = g_CfgMgr.GetTargetSvrIp();
    const auto port = g_CfgMgr.GetTargetSvrPort();
    const auto msgNumPerPeriod = g_CfgMgr.GetMsgNumPerPeriod();
    const auto workSleep = 1;

    // 客户端组
    std::vector<FS_MyClient *> clients(clientQuantity);

    // 计算本线程客户端在clients中对应的index
    int begin = 0;
    int end = clientQuantity / threadQuantity;
    if(end < 1)
        end = 1;

    int nTemp1 = sendSleepInterval > 0 ? sendSleepInterval : 1;
    for(int n = begin; n < end; n++)
    {
        if(!_pool->IsPoolWorking())
            break;

        clients[n] = new FS_MyClient();
        clients[n]->Init();
        clients[n]->_restTime = n % nTemp1;

        // 多线程时让下CPU
        fs::SocketUtil::Sleep(0);
    }
    for(int n = begin; n < end; n++)
    {
        if(!_pool->IsPoolWorking())
            break;

        if(INVALID_SOCKET == clients[n]->InitSocket())
            break;

        if(StatusDefs::Success != clients[n]->Connect(ip.c_str(), port))
            break;

        ++g_ConnectNum;
        fs::SocketUtil::Sleep(0);
    }

    // 所有连接完成
    g_Log->i<ClientTask>(_LOGFMT_("thread<%d>,Connect<begin=%d, end=%d ,nConnect=%d>")
                         , _id, begin, end, (Int32)g_ConnectNum);

    ++g_ReadyCount;
    while(g_ReadyCount < threadQuantity && _pool->IsPoolWorking())
    {// 等待其它线程准备好，再发送数据
        fs::SocketUtil::Sleep(10);
    }

    // 消息
    fs::LoginReq login;
    // 给点有意义的值
    strcpy(login._userName, "lyd");
    strcpy(login._pwd, "lydmm");

    //
    // 收发数据都是通过onRun线程
    // SendData只是将数据写入发送缓冲区
    // 等待select检测可写时才会发送数据
    // 旧的时间点
    auto t2 = fs::Time::Now();
    // 新的时间点
    auto t0 = t2;
    // 经过的时间
    fs::TimeSlice dt;
    while(_pool->IsPoolWorking())
    {
        t0.FlushTime();
        dt = t0 - t2;
        t2 = t0;

        // 本次while (pThread->isRun())循环主要工作内容
        // 代号work
        {
            Int32 count = 0;
            // 每轮每个客户端发送nMsg条数据
            for(Int32 m = 0; m < msgNumPerPeriod; m++)
            {
                // 每个客户端1条1条的写入消息
                for(int n = begin; n < end; n++)
                {
                    if(clients[n]->IsRun())
                    {
                        if(clients[n]->SendTest(&login) > 0)
                        {
                            ++g_SendCount;
                        }
                    }
                }
            }

            // sendCount += count;
            for(int n = begin; n < end; n++)
            {
                if(clients[n]->IsRun())
                {	// 超时设置为0表示select检测状态后立即返回
                    if(!clients[n]->OnRun(0))
                    {	//连接断开
                        --g_ConnectNum;
                        continue;
                    }
                    //检测发送计数是否需要重置
                    clients[n]->CheckSend(dt.GetTotalMilliSeconds());
                }
            }
        }

        fs::SystemUtil::Sleep(workSleep);
    }

    // --------------------------
    // 关闭消息收发线程
    // tRun.Close();
    // 关闭客户端
    for(int n = begin; n < end; n++)
    {
        clients[n]->Close();
        delete clients[n];
    }

    g_Log->i<ClientTask>(_LOGFMT_("thread<%d>,exit"), _id);
    --g_ReadyCount;

    return 0;
}

void FS_ClientRun::Run()
{
    // 1.时区
    fs::TimeUtil::SetTimeZone();

    // 2.智能变量的类型识别
    fs::SmartVarRtti::InitRttiTypeNames();

    // 3.初始化线程局部存储句柄
    Int32 ret = fs::FS_TlsUtil::CreateUtilTlsHandle();
    if(ret != StatusDefs::Success)
    {
        std::cout << "error:" << ret << std::endl;
        return;
    }

    // 4.log初始化 NULL默认以程序名为基准创建目录
    ret = g_Log->InitModule(NULL);
    if(ret != StatusDefs::Success)
    {
        std::cout << "error:" << ret << std::endl;
        return;
    }

    // 5. crash dump switch start
    ret = fs::CrashHandleUtil::InitCrashHandleParams();
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_ClientRun>(_LOGFMT_("init crash handle params fail ret[%d]"), ret);
        return;
    }

    // 6.大小端判断，服务器只支持x86等小端字节序的cpu
    if(!fs::SystemUtil::IsLittleEndian())
    {
        ret = StatusDefs::SystemUtil_NotLittleEndian;
        g_Log->e<FS_ClientRun>(_LOGFMT_("not little endian ret[%d]"), ret);
        return;
    }

    // 初始化配置
    g_CfgMgr.Init();

    // 启动终端命令线程
    // 用于接收运行时用户输入的指令
    const auto threadQuantity = g_CfgMgr.GetThreadQuantity();
    fs::FS_ThreadPool *pool = new fs::FS_ThreadPool(0, threadQuantity + 1);

    //启动模拟客户端线程
    for(int n = 0; n < threadQuantity; n++)
    {
        fs::ITask *t = new ClientTask(pool, n);
        pool->AddTask(*t, true, 1);
    }

    // 每秒数据统计
    fs::Time lastTime;
    fs::Time nowTime;
    nowTime.FlushTime();
    lastTime.FlushTime();
    fs::TimeSlice diff(1);
    const auto clientQuantity = g_CfgMgr.GetClientQuantity();
    while(true)
    {
        auto t = nowTime.FlushTime() - lastTime;
        if(t >= diff)
        {
            g_Log->custom("thread<%d>,clients<%d>,connect<%d>,time<%d>,send<%d>"
                          , threadQuantity, clientQuantity, (int)g_ConnectNum, t.GetTotalSeconds(), (int)g_SendCount);
            g_SendCount = 0;
            lastTime.FlushTime();
        }
        fs::SystemUtil::Sleep(1);
    }

    //
    pool->Close();
    g_Log->custom("。。已退出。。");
    return;
}
#endif
