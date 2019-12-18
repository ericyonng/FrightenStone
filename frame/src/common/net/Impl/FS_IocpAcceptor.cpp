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
 * @file  : FS_IocpAcceptor.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/08
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_IocpAcceptor.h"
#include "FrightenStone/common/net/Impl/FS_NetEngine.h"
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Defs/FS_IocpBuffer.h"
#include "FrightenStone/common/net/Impl/IFS_ServerConfigMgr.h"
#include "FrightenStone/common/net/Impl/FS_IocpSession.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Defs/BriefListenAddrInfo.h"
#include <FrightenStone/common/net/Defs/NetCfgDefs.h>

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/net/Impl/FS_SessionMgr.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"
#include "FrightenStone/common/component/Impl/File/FS_IniFile.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN
FS_IocpAcceptor::FS_IocpAcceptor(Locker &sessionLocker
                                 , Int32 &curSessionCnt
                                 , Int32 &maxSessionQuantityLimit
                                 , UInt64 &curMaxSessionId
                                 , const UInt64 &maxSessionIdLimit
                                 , FS_NetEngine *netEngine )
    :IFS_Acceptor(netEngine)
    ,_threadPool(NULL)
    , _sock(INVALID_SOCKET)
    , _closeIocpDelegate(NULL)
    , _locker(sessionLocker)
    , _curSessionCnt(curSessionCnt)
    , _maxSessionQuantityLimit(maxSessionQuantityLimit)
    , _curMaxSessionId(curMaxSessionId)
    , _maxSessionIdLimit(maxSessionIdLimit)
    ,_cfgs(NULL)
{
    /*     _CrtMemCheckpoint(&s1);*/
}

FS_IocpAcceptor::~FS_IocpAcceptor()
{
    Fs_SafeFree(_closeIocpDelegate);
    Fs_SafeFree(_threadPool);
    Fs_SafeFree(_cfgs);

    //     _CrtMemCheckpoint(&s2);
    //     if(_CrtMemDifference(&s3, &s1, &s2))
    //         _CrtMemDumpStatistics(&s3);
}

Int32 FS_IocpAcceptor::BeforeStart(const AcceptorCfgs &acceptorCfgs)
{
    _cfgs = new AcceptorCfgs;
    *_cfgs = acceptorCfgs;
    _threadPool = new FS_ThreadPool(0, 1);

    // 初始化
    auto sock = _InitSocket();
    if(sock == INVALID_SOCKET)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("init listen socket fail"));
        return StatusDefs::IocpAcceptor_InitListenSocketFail;
    }

    auto &ip = _cfgs->_ip;
    Int32 st = _Bind(ip.GetLength() == 0 ? NULL : ip.c_str(), _cfgs->_port);
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("listen sock[%llu] bind ip[%s:%hu] fail st[%d]")
                                   , _sock, ip.c_str(), _cfgs->_port, st);
        return st;
    }

    st = _Listen();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("listen sock[%llu] listen ip[%s:%hu] fail st[%d]")
                                   , _sock, ip.c_str(), _cfgs->_port, st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpAcceptor::Start()
{
    auto monitorTask = DelegatePlusFactory::Create(this, &FS_IocpAcceptor::_OnIocpMonitorTask);
    if(!_threadPool->AddTask(monitorTask, true))
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("add monitor task fail"));
        return StatusDefs::IocpAcceptor_StartFailOfMonitorTaskFailure;
    }

    return StatusDefs::Success;
}

void FS_IocpAcceptor::BeforeClose()
{
    // 连接器需要再此时关闭入口
    _closeIocpDelegate->Invoke();
    _threadPool->Close();
}

void FS_IocpAcceptor::Close()
{
}

void FS_IocpAcceptor::OnDisconnected(IFS_Session *session)
{
    _locker.Lock();
    auto iterSessinId = _sucConnectedSessionIds.find(session->GetSessionId());
    if(iterSessinId != _sucConnectedSessionIds.end())
    {
        --_curSessionCnt;
        _sucConnectedSessionIds.erase(iterSessinId);
    }

    _locker.Unlock();
}

SOCKET FS_IocpAcceptor::_InitSocket()
{
    if(INVALID_SOCKET != _sock)
    {
        g_Log->w<FS_IocpAcceptor>(_LOGFMT_("initSocket has already init socket<%d>..."), static_cast<Int32>(_sock));
        return _sock;
    }

    // ipv4 的流数据
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == _sock)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("create socket failed..."));
        return _sock;
    }

    // 套接字重用
    auto st = SocketUtil::MakeReUseAddr(_sock);
    if(st != StatusDefs::Success)
    {
        g_Log->w<FS_IocpAcceptor>(_LOGFMT_("make reuse of socket failed sock[%llu]"), _sock);
    }

    g_Log->net<FS_IocpAcceptor>("create socket<%llu> success...", _sock);
    return _sock;
}

Int32 FS_IocpAcceptor::_Bind(const Byte8 *ip, UInt16 port)
{
    // TODO:读取配置表
    // 1.bind 绑定用于接受客户端连接的网络端口
    sockaddr_in sin = {};
    sin.sin_family = AF_INET;
    // host to net unsigned short
    sin.sin_port = htons(port);

#ifdef _WIN32
    if(ip) {
        inet_pton(sin.sin_family, ip, &(sin.sin_addr));// 比较新的函数对比inet_addr
    }
    else {
        sin.sin_addr.S_un.S_addr = INADDR_ANY;
    }
#else
    if(ip) {
        sin.sin_addr.s_addr = inet_addr(ip);
    }
    else {
        sin.sin_addr.s_addr = INADDR_ANY;
    }
#endif

    int ret = bind(_sock, (sockaddr *)&sin, sizeof(sin));
    if(SOCKET_ERROR == ret)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("bind port<%hu> failed..."), port);
        return StatusDefs::Socket_Error;
    }

    g_Log->net<FS_IocpAcceptor>("bind port<%hu> success...", port);
    return StatusDefs::Success;
}

Int32 FS_IocpAcceptor::_Listen(Int32 unconnectQueueLen)
{
    // 1.listen 监听网络端口
    int ret = listen(_sock, unconnectQueueLen);
    if(SOCKET_ERROR == ret)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("listen socket<%llu> failed..."), _sock);
        return StatusDefs::Socket_Error;
    }

    g_Log->net<FS_IocpAcceptor>("listen port socket<%llu> success... %s:%hu", _sock, _cfgs->_ip.c_str(), _cfgs->_port);
    return StatusDefs::Success;
}

void FS_IocpAcceptor::_OnConnected(SOCKET sock, const sockaddr_in *addrInfo, FS_Iocp *iocpListener)
{
    if(INVALID_SOCKET == sock)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("_IocpAccept.sock invalid socket"));
        return;
    }

    _locker.Lock();
    if(_curSessionCnt < _maxSessionQuantityLimit && _curMaxSessionId < _maxSessionIdLimit)
    {
        ++_curSessionCnt;
        ++_curMaxSessionId;
        _sucConnectedSessionIds.insert(_curMaxSessionId);
        _locker.Unlock();

        SocketUtil::MakeReUseAddr(sock);

        // TODO:连接回调 
        BriefSessionInfo *newSessionInfo = new BriefSessionInfo;
        newSessionInfo->_sessionId = _curMaxSessionId;
        newSessionInfo->_sock = sock;
        newSessionInfo->_addrInfo = *addrInfo;
        _netEngine->_OnConnected(newSessionInfo);
    }
    else {
        _locker.Unlock();

        // 获取IP地址 inet_ntoa(clientAddr.sin_addr)
        SocketUtil::DestroySocket(sock);
        g_Log->w<FS_IocpAcceptor>(_LOGFMT_("Accept to MaxClient[%d] or curMaxId[%llu] too large"), _maxSessionQuantityLimit, _curMaxSessionId);
    }
}

void FS_IocpAcceptor::_OnIocpMonitorTask(FS_ThreadPool *threadPool)
{
    // 1.创建并绑定监听端口
    SmartPtr<FS_Iocp> listenIocp = new FS_Iocp;
    listenIocp->Create();
    listenIocp->Reg(_sock);
    listenIocp->LoadAcceptEx(_sock);

    // 2.定义关闭iocp
    auto __quitIocpFunc = [this, &listenIocp]()->void {
        // 先关闭listensocket
        // SocketUtil::DestroySocket(_sock);
        listenIocp->PostQuit();
    };

    // 3.绑定退出iocp
    _closeIocpDelegate = DelegatePlusFactory::Create<decltype(__quitIocpFunc), void>(__quitIocpFunc);

    // const int len = 2 * (sizeof(sockaddr_in) + 16);
    // 不需要客户端再连接后立即发送数据的情况下最低长度len

    // 4.预先投递FD_SETSIZE个accept 快速连入
    char **bufArray = NULL;
    IoDataBase **ioDataArray = NULL;
    _PreparePostAccept(listenIocp, bufArray, ioDataArray);

    // 5.监听网络连入
    IO_EVENT ioEvent = {};
    while(threadPool->IsPoolWorking())
    {
        // 监听iocp
        auto ret = listenIocp->WaitForCompletion(ioEvent);
        if(ret == StatusDefs::IOCP_WaitTimeOut)
            continue;

        // 出错
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_IocpAcceptor>(_LOGFMT_("_OnIocpMonitorTask.WaitForCompletion error ret[%d]"), ret);
            break;
        }

        // 处理iocp退出
        if(ioEvent._data._code == IocpDefs::IO_QUIT)
        {
            g_Log->sys<FS_IocpAcceptor>(_LOGFMT_("connector iocp退出 threadId<%llu> code=%lld")
                                         , SystemUtil::GetCurrentThreadId(), ioEvent._data._code);
            break;
        }

        // 接受连接 完成
        if(IocpDefs::IO_ACCEPT == ioEvent._ioData->_ioType)
        {
            // TODO:在getacceptAddrInfo时候需要考虑线程是否安全
            sockaddr_in *clientAddrInfo = NULL;
            listenIocp->GetClientAddrInfo(ioEvent._ioData->_wsaBuff.buf, clientAddrInfo);
            _OnConnected(ioEvent._ioData->_sock, clientAddrInfo, listenIocp);

            // 继续向IOCP投递接受连接任务 已经预先post 10万个sock资源不用再post浪费套接字资源
            // 添加到已使用队列
            const auto st = listenIocp->PostAccept(_sock, ioEvent._ioData);
            if(st != StatusDefs::Success)
            {
                g_Log->e<FS_IocpAcceptor>(_LOGFMT_("post accept fail whnen new session create listen sock[%llu], iodata addr[%p] st[%d]")
                                           , _sock, ioEvent._ioData, st);
            }
        }
    }

    SocketUtil::DestroySocket(_sock);
    listenIocp->Destroy();
    _FreePrepareAcceptBuffers(bufArray, ioDataArray);
}

void FS_IocpAcceptor::_PreparePostAccept(FS_Iocp *listenIocp, char **&bufArray, IoDataBase **&ioDataArray)
{
    // 预先创建n个缓冲加速连接过程
    g_MemoryPool->Lock();
    bufArray = g_MemoryPool->Alloc<char *>(_maxSessionQuantityLimit * sizeof(char *));
    g_MemoryPool->Unlock();

    for(Int32 i = 0; i < _maxSessionQuantityLimit; ++i)
    {
        g_MemoryPool->Lock();
        bufArray[i] = g_MemoryPool->Alloc<char>(IOCP_CONNECTOR_BUFFER);
        g_MemoryPool->Unlock();
    }

    // 预先创建n个iodata
    g_MemoryPool->Lock();
    ioDataArray = g_MemoryPool->Alloc<IoDataBase *>(sizeof(IoDataBase *)* _maxSessionQuantityLimit);
    g_MemoryPool->Unlock();

    Int32 st = StatusDefs::Success;
    for(Int32 i = 0; i < _maxSessionQuantityLimit; ++i)
    {
        g_MemoryPool->Lock();
        ioDataArray[i] = g_MemoryPool->Alloc<IoDataBase>(sizeof(IoDataBase));
        g_MemoryPool->Unlock();

        memset(ioDataArray[i], 0, sizeof(IoDataBase));
        ioDataArray[i]->_wsaBuff.buf = bufArray[i];
        ioDataArray[i]->_wsaBuff.len = IOCP_CONNECTOR_BUFFER;

        st = listenIocp->PostAccept(_sock, ioDataArray[i]);
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_IocpAcceptor>(_LOGFMT_("prepare post accept fail sock[%llu] st[%d]")
                                       , _sock, st);
        }
    }

    // TODO:提升连接速度：一次性投递本机可连接的最高连接数，用掉一个再补充一个
}

void FS_IocpAcceptor::_FreePrepareAcceptBuffers(char **&bufArray, IoDataBase **&ioDataArray)
{
    g_MemoryPool->Lock();
    for(Int32 i = 0; i < _maxSessionQuantityLimit; ++i)
    {
        g_MemoryPool->Free(bufArray[i]);
        g_MemoryPool->Free(ioDataArray[i]);
    }
    g_MemoryPool->Free(bufArray);
    g_MemoryPool->Free(ioDataArray);
    g_MemoryPool->Unlock();
}

FS_NAMESPACE_END

#endif
