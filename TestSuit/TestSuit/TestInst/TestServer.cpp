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
 * @file  : TestServer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "TestSuit/TestSuit/TestInst/TestServer.h"
#include "FrightenStone/exportbase.h"
#include "TestInst/TestServer/User/UserMgrFactory.h"
#include "TestInst/TestServer/User/UserMgr.h"
#include "TestInst/TestServer/User/User.h"

class MyLogic : public fs::IFS_BusinessLogic
{
public:
    MyLogic(){}
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
        return g_UserMgr->NewUser(sessionId);
    }

    virtual void OnMsgDispatch(UInt64 sessionId, fs::NetMsg_DataHeader *msgData)
    {
        auto user = g_UserMgr->GetUserBySessionId(sessionId);
        if(!user)
            return;

        if(msgData->_cmd >= fs::ProtocolCmd::CMD_End || 
           msgData->_cmd <= fs::ProtocolCmd::CMD_Begin)
        {
            // TODO:错误的协议号可能遭受试探性攻击
            g_Log->w<MyLogic>(_LOGFMT_("unknown protocolcmd[%hu] len[%hu] sessionId[%llu]")
                              , msgData->_cmd, msgData->_packetLength, sessionId);
            return;
        }

        // TODO:需不需要对单包数据长度进行限制,保证不会收到长度攻击?
        // if(msgData->_packetLength)
        InvokeProtocolHandler(msgData->_cmd, user->GetUseId(), msgData);
    }
};

class FS_ServerEngine : public fs::IFS_NetEngine
{
public:
    FS_ServerEngine()
    {
        _config = new fs::IFS_ConfigMgr;
    }
    ~FS_ServerEngine()
    {
        fs::STLUtil::DelVectorContainer(_logics);
        Fs_SafeFree(_config);
    }

protected:
    // 读取配置位置
    virtual Int32 _OnReadCfgs()
    {
        auto ret = _config->Init(SVR_CONFIG_PATH_NAME);
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_ServerEngine>(_LOGFMT_("config Init fail ret[%d]"), ret);
            return ret;
        }

        // TODO:
        _totalCfgs = new fs::NetEngineTotalCfgs;
        auto &commonConfig = _totalCfgs->_commonCfgs;
        commonConfig._maxSessionQuantityLimit = _config->GetMaxSessionQuantityLimit();
        commonConfig._acceptorQuantityLimit = _config->GetAcceptorQuantity();
        commonConfig._dispatcherQuantity = _config->GetDispatcherCnt();
        commonConfig._transferQuantity = _config->GetTransferCnt();

        auto &connectorCfg = _totalCfgs->_connectorCfgs;
        connectorCfg._connectTimeOutMs = _config->GetConnectorConnectTimeOutMs();

        // 一组监视器
        _totalCfgs->_acceptorCfgs = new fs::AcceptorCfgs[commonConfig._acceptorQuantityLimit];
        auto &acceptorCfg = _totalCfgs->_acceptorCfgs;
        std::vector<std::pair<fs::FS_String, UInt16>> addrInfos;
        _config->GetListenAddr(addrInfos);
        for(UInt32 i = 0; i < commonConfig._acceptorQuantityLimit; ++i)
        {
            acceptorCfg[i]._ip = addrInfos[i].first;
            acceptorCfg[i]._port = addrInfos[i].second;
        }

        auto &transferCfg = _totalCfgs->_transferCfgs;

        auto &dispatcherCfg = _totalCfgs->_dispatcherCfgs;
        dispatcherCfg._heartbeatDeadTimeMsInterval = _config->GetHeartbeatDeadTimeIntervalMs();
        dispatcherCfg._dispatcherResolutionInterval = _config->GetDispatcherResolutionIntervalMs()*fs::Time::_microSecondPerMilliSecond;
        dispatcherCfg._maxAlloctorBytesPerDispatcher = _config->GetMaxAllowAlloctorBytesPerDispatcher();
        dispatcherCfg._prepareBufferPoolCnt = _config->GetPrepareBufferCnt();

        auto &objPoolCfgs = _totalCfgs->_objPoolCfgs;
        objPoolCfgs._maxAllowObjPoolBytesOccupied = _config->GetMaxAllowObjPoolBytesOccupied();

        auto &mempoolCfgs = _totalCfgs->_mempoolCfgs;
        mempoolCfgs._maxAllowMemPoolBytesOccupied = _config->GetMaxAllowMemPoolBytesOccupied();
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

private:
    fs::IFS_ConfigMgr * _config;
    std::vector<fs::IFS_BusinessLogic *> _logics;
};

static union {
    char c[4];
    unsigned long l;
} endian_test = {{'l', '?', '?', 'b'}};

void TestServer::Run()
{
    endian_test;
    if((char)(endian_test.l) == 'b')
    {
        printf("big endian\n");
    }
    else
    {
        printf("little endian\n");
    }

    fs::IFS_NetEngine *serverCore = new FS_ServerEngine();
    auto st = serverCore->Init();

    if(st == StatusDefs::Success)
    {
        // 并发业务逻辑
        //auto dispatcherCnt = g_SvrCfg->GetDispatcherCnt();
        auto st = serverCore->Start();
        if(st == StatusDefs::Success)
        {
            std::cout << "server suc start" << std::endl;
            getchar();
        }
        else
        {
            g_Log->e<TestServer>(_LOGFMT_("Start server fail st[%d] server will close now. please check! "));
        }
    }
    else
    {
        std::cout << "server core init fail." << std::endl;
    }

    //serverCore->Wait();
    serverCore->Close();
    Fs_SafeFree(serverCore);
    g_Log->FinishModule();
    std::cout << "free server core" << std::endl;
    getchar();
}
