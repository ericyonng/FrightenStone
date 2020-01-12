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
#include "FrightenStone/exportbase.h"
#include "FS_MyClient/FS_MyClient/User/UserMgrFactory.h"
#include "FS_MyClient/FS_MyClient/User/UserMgr.h"
#include "FS_MyClient/FS_MyClient/User/User.h"
#include "FS_MyClient/FS_MyClient/ClientCfgs/FS_ClientCfgMgr.h"
#include "FS_MyClient/FS_MyClient/Defs/StatisticsData.h"

#ifdef _WIN32

StatisticsData g_StaticsticsForSimulation;

class MyLogic : public fs::IFS_BusinessLogic
{
public:
    MyLogic() {}
    virtual ~MyLogic()
    {
    }
    virtual void Release()
    {
        delete this;
    }

public:
    virtual void OnRegisterFacades()
    {
        // 注册facades
        RegisterFacade<UserMgrFactory>();
    }

    virtual void OnSessionDisconnected(UInt64 sessionId, std::list<fs::IDelegate<void, UInt64> *> *disconnectedDelegate)
    {
        auto user = g_UserMgr->GetUserBySessionId(sessionId);
        if(user)
        {
            if(disconnectedDelegate)
            {
                for(auto iterDelegate = disconnectedDelegate->begin(); iterDelegate != disconnectedDelegate->end(); )
                {
                    auto item = *iterDelegate;
                    item->Invoke(sessionId);
                    FS_Release(item);
                    iterDelegate = disconnectedDelegate->erase(iterDelegate);
                }
            }

            user->OnDisconnect();
        }

        g_UserMgr->RemoveUser(sessionId);
        // g_Log->any<MyLogic>("sessionid[%llu] Disconnected", sessionId);
    }

    virtual fs::IUserSys *OnSessionConnected(UInt64 sessionId)
    {
        auto newUser = g_UserMgr->NewUser(sessionId);
        newUser->OnConnected();
        return newUser;
    }

    virtual void OnMsgDispatch(UInt64 sessionId, fs::NetMsgDecoder *msgDecoder)
    {
        auto user = g_UserMgr->GetUserBySessionId(sessionId);
        if(!user)
            return;

        const auto cmd = msgDecoder->GetCmd();
        if(cmd >= fs::ProtocolCmd::CMD_End ||
           cmd <= fs::ProtocolCmd::CMD_Begin)
        {
            // TODO:错误的协议号可能遭受试探性攻击
            g_Log->w<MyLogic>(_LOGFMT_("unknown protocolcmd[%u] len[%u] sessionId[%llu]")
                              , cmd, msgDecoder->GetMsgLen(), sessionId);
            return;
        }

        // TODO:需不需要对单包数据长度进行限制,保证不会收到长度攻击?
        // if(msgData->_packetLength)
        InvokeProtocolHandler(sessionId, msgDecoder);
    }
};
// 
class FS_MyClient : public fs::IFS_NetEngine
{
public:
    FS_MyClient()
    {
        _config = new FS_ClientCfgMgr;
    }
    ~FS_MyClient()
    {
        fs::STLUtil::DelVectorContainer(_logics);
        Fs_SafeFree(_config);
    }

public:
    void Connect(const fs::FS_ConnectInfo &info)
    {
        Int32 st = _connector->Connect(info);
        if(st != StatusDefs::Success)
            g_Log->w<FS_MyClient>(_LOGFMT_("connect to svr fail..."), st);
    }

protected:
    virtual fs::IFS_ConfigMgr *_PreparConfigFile() const
    {
        auto ret = _config->Init("./ClientCfg.ini");
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_MyClient>(_LOGFMT_("config Init fail ret[%d]"), ret);
        }

        return _config;
    }

    // 自定义的配置
    virtual Int32 _ReadCustomCfgs() 
    { 
        return StatusDefs::Success;
    } 

    // 初始化结束时
    virtual Int32 _OnInitFinish()
    {
        _logics.resize(_totalCfgs->_commonCfgs._dispatcherQuantity);
        Int32 quantity = static_cast<Int32>(_logics.size());
        for(Int32 i = 0; i < quantity; ++i)
            _logics[i] = new MyLogic;

        return StatusDefs::Success;
    }
    // 获取业务层,以便绑定到dispatcher上
    virtual void _GetLogics(std::vector<fs::IFS_BusinessLogic *> &logics)
    {
        logics = _logics;
    }

    virtual void _ModifyCustomPrintInfo(fs::FS_String &toPrintInfo)
    {
        auto &staticsLock = g_StaticsData->_lock;
        staticsLock.Lock();
        Int32 hasSend = g_StaticsData->_curSendMsgCount;
        g_StaticsData->_curSendMsgCount = 0;
        staticsLock.Unlock();

        toPrintInfo.AppendFormat("hasSendCount<%d> hopeconnect<%d>,realconnect<%d>."
                                 , g_ClientCfgMgr->GetClientQuantity()
                                 , hasSend, (Int32)g_StaticsData->_curSucConnectedClient);
    }


private:
    FS_ClientCfgMgr * _config;
    std::vector<fs::IFS_BusinessLogic *> _logics;
};

class ClientSimulationTask
{
public:
    ClientSimulationTask(FS_MyClient *engine)
    {
        _netEngine = engine;
    }
    ~ClientSimulationTask() {}

public:
    void Run(fs::FS_ThreadPool *pool)
    {
        // 等待组件全部启动完成
        fs::EngineCompsMethods::WaitForAllCompsReady(_netEngine);
        // 执行任务
        DoSimulationTask();
    }

    void DoSimulationTask()
    {
        // 连接远程
        fs::FS_ConnectInfo connectInfo;
        connectInfo._ip = g_ClientCfgMgr->GetTargetSvrIp();
        connectInfo._port = g_ClientCfgMgr->GetTargetSvrPort();

        auto hopeConnect = g_ClientCfgMgr->GetClientQuantity();
        for(Int32 i = 0; i < hopeConnect; ++i)
            _netEngine->Connect(connectInfo);
    }

private:
    FS_MyClient *_netEngine;
};
// 
// FS_NAMESPACE_END
// 
void FS_ClientRun::Run()
{
    fs::FS_ThreadPool *pool = new fs::FS_ThreadPool(0, 1);
    FS_MyClient *myClient = new FS_MyClient();
    auto st = myClient->Init();

    if(st == StatusDefs::Success)
    {
        // 并发业务逻辑
        //auto dispatcherCnt = g_SvrCfg->GetDispatcherCnt();
        auto st = myClient->Start();
        if(st == StatusDefs::Success)
        {
            ClientSimulationTask *simulationTask = new ClientSimulationTask(myClient);
            auto taskDelegate = fs::DelegatePlusFactory::Create(simulationTask, &ClientSimulationTask::Run);
            pool->AddTask(taskDelegate);

            g_Log->custom("myclient suc start...");
            getchar();
            pool->Close();
            Fs_SafeFree(simulationTask);
        }
        else
        {
            g_Log->e<FS_ClientRun>(_LOGFMT_("Start myclient fail st[%d] server will close now. please check! "));
        }

        g_Log->custom("myclient will close...");
    }
    else
    {
        std::cout << "myclient init fail." << std::endl;
    }

    //serverCore->Wait();
    myClient->Close();
    Fs_SafeFree(myClient);
    std::cout << "free myClient end" << std::endl;
    getchar();

    // 1.时区
//     fs::TimeUtil::SetTimeZone();
// 
//     // 2.智能变量的类型识别
//     fs::SmartVarRtti::InitRttiTypeNames();
// 
//     // 3.初始化线程局部存储句柄
//     Int32 ret = fs::FS_TlsUtil::CreateUtilTlsHandle();
//     if(ret != StatusDefs::Success)
//     {
//         std::cout << "error:" << ret << std::endl;
//         return;
//     }
// 
//     // 4.log初始化 NULL默认以程序名为基准创建目录
//     ret = g_Log->InitModule(NULL);
//     if(ret != StatusDefs::Success)
//     {
//         std::cout << "error:" << ret << std::endl;
//         return;
//     }
// 
//     // 5. crash dump switch start
//     ret = fs::CrashHandleUtil::InitCrashHandleParams();
//     if(ret != StatusDefs::Success)
//     {
//         g_Log->e<FS_ClientRun>(_LOGFMT_("init crash handle params fail ret[%d]"), ret);
//         return;
//     }
// 
//     // 6.大小端判断，服务器只支持x86等小端字节序的cpu
//     if(!fs::SystemUtil::IsLittleEndian())
//     {
//         ret = StatusDefs::SystemUtil_NotLittleEndian;
//         g_Log->e<FS_ClientRun>(_LOGFMT_("not little endian ret[%d]"), ret);
//         return;
//     }
// 
//     // 初始化配置
//     g_CfgMgr.Init();
// 
//     // 启动终端命令线程
//     // 用于接收运行时用户输入的指令
//     const auto threadQuantity = g_CfgMgr.GetThreadQuantity();
//     fs::FS_ThreadPool *pool = new fs::FS_ThreadPool(0, threadQuantity + 1);
// 
//     //启动模拟客户端线程
//     for(int n = 0; n < threadQuantity; n++)
//     {
//         fs::ITask *t = new ClientTask(pool, n);
//         pool->AddTask(*t, true, 1);
//     }
// 
//     // 每秒数据统计
//     fs::Time lastTime;
//     fs::Time nowTime;
//     nowTime.FlushTime();
//     lastTime.FlushTime();
//     fs::TimeSlice diff(1);
//     const auto clientQuantity = g_CfgMgr.GetClientQuantity();
//     while(true)
//     {
//         auto t = nowTime.FlushTime() - lastTime;
//         if(t >= diff)
//         {
//             g_Log->custom("thread<%d>,clients<%d>,connect<%d>,time<%d>,send<%d>"
//                           , threadQuantity, clientQuantity, (int)g_ConnectNum, t.GetTotalSeconds(), (int)g_SendCount);
//             g_SendCount = 0;
//             lastTime.FlushTime();
//         }
//         fs::SystemUtil::Sleep(1);
//     }
// 
//     //
//     pool->Close();
//     g_Log->custom("。。已退出。。");
    return;
}


#endif
