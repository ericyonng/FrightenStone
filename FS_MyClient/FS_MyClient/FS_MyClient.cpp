/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/12/6
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include <FS_MyClient/FS_MyClient/FS_MyClient.h>
#include "FrightenStone/exportbase.h"
#include "Service/LogicSvc/Modules/TestProtocols/Defs/FS_ClientCfgMgr.h"
#include "Service/LogicSvc/Modules/TestProtocols/Defs/StatisticsData.h"
#include "Service/Application.h"
#include "Service/LogicSvc/LogicSvc.h"
#include "Service/LogicSvc/Modules/TestProtocols/TestProtocols.h"
#include "Service/ServiceType.h"

class MyClientLogic : public LogicSvc
{
    OBJ_POOL_CREATE_DERIVE(MyClientLogic, LogicSvc);
public:
    MyClientLogic() :LogicSvc(ServiceType::ClientSimulation){}
    virtual ~MyClientLogic()
    {
    }
    virtual void Release()
    {
        delete this;
    }

public:
    virtual void OnRegisterFacades()
    {
        LogicSvc::OnRegisterFacades();

//         // user子系统
//         RegisterFacade<TestProtocolsFactory>();
// 
//         // global子系统
//         RegisterFacade<TestProtocolsGlobalFactory>();
    }

};

OBJ_POOL_CREATE_DERIVE_IMPL(MyClientLogic, 1);

class MyClient : public FS_Application
{
    OBJ_POOL_CREATE_DERIVE(MyClient, FS_Application);
public:
    MyClient() {}
    ~MyClient() {}

protected:
    virtual fs::IFS_ConfigMgr *_NewConfigMgr()
    {
        return new FS_ClientCfgMgr;
    }

    virtual fs::IFS_BusinessLogic *_NewSvc(Int32 serviceId)
    {
        return new MyClientLogic();
    }

    virtual void _ModifyCustomPrintInfo(fs::FS_String &toPrintInfo)
    {
        auto &staticsLock = g_StaticsData->_lock;
        staticsLock.Lock();
        Int32 hasSend = g_StaticsData->_curSendMsgCount;
        g_StaticsData->_curSendMsgCount = 0;
        Int32 realConnect = g_StaticsData->_curSucConnectedClient;
        staticsLock.Unlock();

        toPrintInfo.AppendFormat(", Send<%d pps> hopeconnect<%d>,realconnect<%d>."
                                 ,hasSend, g_ClientCfgMgr->GetClientQuantity()
                                 ,realConnect);
    }
};
OBJ_POOL_CREATE_DERIVE_IMPL(MyClient, 1);
//OBJ_POOL_CREATE_DEF_IMPL(MyClient, 1);

// 
// class ClientSimulationTask
// {
// public:
//     ClientSimulationTask(MyClient *engine)
//     {
//         _netEngine = engine;
//     }
//     ~ClientSimulationTask() {}
// 
// public:
//     void Run(fs::FS_ThreadPool *pool)
//     {
//         // 等待组件全部启动完成
//         fs::EngineCompsMethods::WaitForAllCompsReady(_netEngine);
// 
//         g_Log->i<ClientSimulationTask>(_LOGFMT_("will do simulation task..."));
// 
//         // 执行任务
//         DoSimulationTask();
//     }
// 
//     void DoSimulationTask()
//     {
//         // 连接远程
//         fs::FS_ConnectInfo connectInfo;
//         connectInfo._ip = g_ClientCfgMgr->GetTargetSvrIp();
//         connectInfo._port = g_ClientCfgMgr->GetTargetSvrPort();
// 
//         auto hopeConnect = g_ClientCfgMgr->GetClientQuantity();
//         for(Int32 i = 0; i < hopeConnect; ++i)
//             _netEngine->Connect(connectInfo);
// 
//         _netEngine->PostConnectFinish();
//     }
// 
// private:
//     MyClient *_netEngine;
// };
// 
// FS_NAMESPACE_END
// 
void FS_ClientRun::Run()
{
    MyClient *myClient = new MyClient();
    // ClientSimulationTask *simulationTask = new ClientSimulationTask(myClient);
//     auto taskDelegate = fs::DelegatePlusFactory::Create(simulationTask, &ClientSimulationTask::Run);
//     pool->AddTask(taskDelegate);

    myClient->Run();
    Fs_SafeFree(myClient);
    std::cout << "free myClient end" << std::endl;
  
    getchar();
    return;
}


