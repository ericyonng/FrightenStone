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
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Defs/FS_IocpBuffer.h"
#include "FrightenStone/common/net/Impl/IFS_ConfigMgr.h"
#include "FrightenStone/common/net/Impl/FS_IocpSession.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Defs/BriefListenAddrInfo.h"
#include <FrightenStone/common/net/Defs/NetCfgDefs.h>
#include "FrightenStone/common/net/Impl/FS_IocpPoller.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/net/Impl/FS_SessionMgr.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"
#include "FrightenStone/common/component/Impl/File/FS_IniFile.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_IocpAcceptor, __DEF_OBJ_POOL_OBJ_NUM__);

FS_IocpAcceptor::FS_IocpAcceptor(UInt32 compId
                                 , Locker &sessionLocker
                                 , Int32 &curSessionCnt
                                 , Int32 &maxSessionQuantityLimit
                                 , UInt64 &curMaxSessionId
                                 , const AcceptorCfgs &cfg
                                 , IFS_NetEngine *netEngine)
    :IFS_Acceptor(compId, netEngine)
    , _sock(INVALID_SOCKET)
    ,_isInit(false)
    , _locker(sessionLocker)
    , _curSessionCnt(curSessionCnt)
    , _maxSessionQuantityLimit(maxSessionQuantityLimit)
    , _curMaxSessionId(curMaxSessionId)
    ,_cfgs(NULL)
    ,_poller(NULL)
{
    /*     _CrtMemCheckpoint(&s1);*/
    _cfgs = new AcceptorCfgs;
    *_cfgs = cfg;
}

FS_IocpAcceptor::~FS_IocpAcceptor()
{
    Fs_SafeFree(_poller);
    Fs_SafeFree(_cfgs);

    //     _CrtMemCheckpoint(&s2);
    //     if(_CrtMemDifference(&s3, &s1, &s2))
    //         _CrtMemDumpStatistics(&s3);
}

Int32 FS_IocpAcceptor::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    if(_isInit)
        return StatusDefs::Success;

    // _cfg在构造中已经初始化这里不必再初始化
    _poller = new FS_IocpPoller(this, PollerDefs::MonitorType_Acceptor);

    // 初始化
    auto sock = _InitSocket();
    if(sock == INVALID_SOCKET)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("init listen socket fail"));
        return StatusDefs::IocpAcceptor_InitListenSocketFail;
    }

    auto &ip = _cfgs->_ip;
    const Byte8 *realIp = NULL;
    _GetRealIp(ip, realIp);
    Int32 st = _Bind(realIp, _cfgs->_port);
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

    auto netEngine = GetEngine();
    _poller->AttachAcceptorParam(_sock, _maxSessionQuantityLimit, &_locker, &_curSessionCnt, &_curMaxSessionId);

    st = _poller->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("_poller BeforeStart fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpAcceptor::Start()
{
    if(_isInit)
        return StatusDefs::Success;

    Int32 st = _poller->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpAcceptor>(_LOGFMT_("poller start error st[%d]"), st);
        return st;
    }

    _isInit = true;
    return StatusDefs::Success;
}

void FS_IocpAcceptor::AfterStart()
{
    if(_poller)
        _poller->AfterStart();
}

void FS_IocpAcceptor::WillClose()
{
    if(!_isInit)
        return;

    _poller->WillClose();
}

void FS_IocpAcceptor::BeforeClose()
{
    if(!_isInit)
        return;

    _poller->BeforeClose();
}

void FS_IocpAcceptor::Close()
{
    if(!_isInit)
        return;

    _poller->Close();
    _isInit = false;
}

void FS_IocpAcceptor::AfterClose()
{
    if(_poller)
        _poller->AfterClose();
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
        sin.sin_addr.S_un.S_addr = INADDR_ANY;  // 绑定任意网卡
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

void FS_IocpAcceptor::_GetRealIp(const FS_String &cfgIp, const Byte8 *&realIp)
{
    if(cfgIp.GetLength() == 0)
    {
        realIp = NULL;
        return;
    }

    if(cfgIp == IFS_ConfigMgr::_bindAnyIp)
    {
        realIp = NULL;
        return;
    }

    realIp = cfgIp.c_str();
}

void FS_IocpAcceptor::OnSessionDisconnected(UInt64 sessionId)
{
    _poller->OnAcceptorDisconnected(sessionId);
}
FS_NAMESPACE_END

#endif
