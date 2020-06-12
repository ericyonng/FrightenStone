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
 * @file  : IFS_Acceptor.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/4/17
 * @brief :
 */

#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_Acceptor.h"
#include "FrightenStone/common/net/Defs/BriefListenAddrInfo.h"
#include "FrightenStone/common/net/Impl/IFS_AcceptorPoller.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/socket/socket.h"

FS_NAMESPACE_BEGIN
IFS_Acceptor::~IFS_Acceptor()
{
    Fs_SafeFree(_cfgs);
    if (SocketUtil::IsValidSock(_sock))
        SocketUtil::DestroySocket(_sock);
}

Int32 IFS_Acceptor::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    // _cfg在构造中已经初始化这里不必再初始化
    auto poller = _CreatePoller();

    // 初始化
    auto sock = _InitSocket();
    if (sock == INVALID_SOCKET)
    {
        g_Log->e<IFS_Acceptor>(_LOGFMT_("init listen socket fail"));
        return StatusDefs::IFS_Acceptor_InitListenSocketFail;
    }

    auto &ip = _cfgs->_ip;
    const Byte8 *realIp = NULL;
    _GetRealIp(ip, realIp);

    BriefListenAddrInfo listenAddrInfo;
    listenAddrInfo._ip = realIp ? realIp : "";
    listenAddrInfo._port = _cfgs->_port;
    listenAddrInfo._family = AF_INET;
    Int32 st = _Bind(listenAddrInfo);
    if (st != StatusDefs::Success)
    {
        g_Log->e<IFS_Acceptor>(_LOGFMT_("listen sock[%llu] bind ip[%s:%hu] fail st[%d]")
            , _sock, ip.c_str(), _cfgs->_port, st);
        return st;
    }

    st = _Listen();
    if (st != StatusDefs::Success)
    {
        g_Log->e<IFS_Acceptor>(_LOGFMT_("listen sock[%llu] listen ip[%s:%hu] fail st[%d]")
            , _sock, ip.c_str(), _cfgs->_port, st);
        return st;
    }

    auto netEngine = GetEngine();
    poller->AttachAcceptorParam(_sock, _maxSessionQuantityLimit, &_locker, &_curSessionCnt, &_curMaxSessionId);

    st = poller->BeforeStart();
    if (st != StatusDefs::Success)
    {
        g_Log->e<IFS_Acceptor>(_LOGFMT_("_poller BeforeStart fail st[%d]"), st);
        return st;
    }

    BUFFER256 strip = {};
    Byte8 *pIp = strip;
    UInt16 port = 0;
    Int32 lastError = 0;
    sockaddr_in addrInfo;
    st = SocketUtil::GetSockName(_sock, addrInfo, lastError);
    if (st != StatusDefs::Success)
    {
        g_Log->w<IFS_Acceptor>(_LOGFMT_("GetSockName error st[%d] lastError[%d]"), st, lastError);
    }

    // 监听端口地址信息
    SocketUtil::GetAddrInfoFromNetInfo(addrInfo, sizeof(strip), pIp, port);
    g_Log->sys<IFS_Acceptor>(_LOGFMT_("listen sock[%d] addr[%s:%hu] lastError[%d]")
        , _sock, strip, port, lastError);

    return StatusDefs::Success;
}

SOCKET IFS_Acceptor::_InitSocket()
{
    if(INVALID_SOCKET != _sock)
    {
        g_Log->w<IFS_Acceptor>(_LOGFMT_("initSocket has already init socket<%d>..."), static_cast<Int32>(_sock));
        return _sock;
    }

    // ipv4 的流数据
    _sock = socket(AF_INET, __FS_TCP_SOCKTYPE__, IPPROTO_TCP);
    if(INVALID_SOCKET == _sock)
    {
        g_Log->e<IFS_Acceptor>(_LOGFMT_("create socket failed..."));
        return _sock;
    }

    auto st = SocketUtil::SetNoBlock(_sock);
    if(st != StatusDefs::Success)
    {
        g_Log->w<IFS_Acceptor>(_LOGFMT_("set no block fail sock[%llu]"), _sock);
        SocketUtil::DestroySocket(_sock);
        return _sock;
    }

    // 套接字重用
    st = SocketUtil::MakeReUseAddr(_sock);
    if(st != StatusDefs::Success)
    {
        g_Log->w<IFS_Acceptor>(_LOGFMT_("make reuse of socket failed sock[%llu]"), _sock);
        SocketUtil::DestroySocket(_sock);
        return _sock;
    }

    g_Log->net<IFS_Acceptor>("create socket<%llu> success...", _sock);
    return _sock;
}

Int32 IFS_Acceptor::_Bind(const BriefListenAddrInfo &addrInfo)
{
    // TODO:读取配置表
 // 1.bind 绑定用于接受客户端连接的网络端口
    sockaddr_in sin = {};
    sin.sin_family = addrInfo._family;
    // host to net unsigned short
    sin.sin_port = htons(addrInfo._port);

#ifdef _WIN32
    if(addrInfo._ip.size()) {
        inet_pton(sin.sin_family, addrInfo._ip.c_str(), &(sin.sin_addr));// 比较新的函数对比inet_addr
    }
    else {
        sin.sin_addr.S_un.S_addr = INADDR_ANY;  // 绑定任意网卡
    }
#else
    if(addrInfo._ip.size()) {
        sin.sin_addr.s_addr = inet_addr(addrInfo._ip.c_str());
    }
    else {
        sin.sin_addr.s_addr = INADDR_ANY;
    }
#endif

    int ret = bind(_sock, (sockaddr *)&sin, sizeof(sin));
    if(SOCKET_ERROR == ret)
    {
        g_Log->e<IFS_Acceptor>(_LOGFMT_("bind port<%hu> failed..."), addrInfo._port);
        return StatusDefs::Socket_Error;
    }

    g_Log->net<IFS_Acceptor>("bind port<%hu> success...", addrInfo._port);
    return StatusDefs::Success;
}

Int32 IFS_Acceptor::_Listen(Int32 unconnectQueueLen /*= SOMAXCONN*/)
{
    // 1.listen 监听网络端口
    int ret = listen(_sock, unconnectQueueLen);
    if(SOCKET_ERROR == ret)
    {
        g_Log->e<IFS_Acceptor>(_LOGFMT_("listen socket<%llu> failed..."), _sock);
        return StatusDefs::Socket_Error;
    }

    sockaddr_in addrInfo;
    Int32 lastError = 0;
    auto st = SocketUtil::GetSockName(_sock, addrInfo, lastError);
    if(st != StatusDefs::Success)
    {
        g_Log->w<IFS_Acceptor>(_LOGFMT_("get sock name fail st[%d], lasterror[%d]"), st, lastError);
    }
    else
    {
        BUFFER256 ip = {0};
        UInt16 port = 0;
        char *ptr = ip;
        if(SocketUtil::GetAddrInfoFromNetInfo(addrInfo, sizeof(ip), ptr, port))
        {
            g_Log->net<IFS_Acceptor>("listen port socket<%llu> success... %s:%hu", _sock, ip, port);
        }
    }

    g_Log->net<IFS_Acceptor>("listen port socket<%llu> success...", _sock);
    return StatusDefs::Success;
}

void IFS_Acceptor::_GetRealIp(const FS_String &cfgIp, const Byte8 *&realIp)
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

FS_NAMESPACE_END

