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
 * @file  : FS_Iocp.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/8/7
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Defs/IoEvDefs.h"

#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"

#ifdef _WIN32
#pragma region windows api
#include <windows.h>
// #include <WinSock2.h>
// #pragma comment (lib, "ws2_32.lib")
 // #pragma comment (lib, "MSWSock.lib")
#pragma endregion
#endif

#ifdef _WIN32

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_ANCESTOR_IMPL(FS_Iocp, __DEF_OBJ_POOL_OBJ_NUM__);

FS_Iocp::FS_Iocp()
{

}

FS_Iocp::~FS_Iocp()
{
    Destroy();
}

#pragma region init/reg
Int32 FS_Iocp::Create()
{
    if(_completionPort)
        return StatusDefs::Success;

    // 创建完成端口IOCP NumberOfConcurrentThreads=0表示默认cpu核数
    _completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if(!_completionPort)
    {
        auto err = GetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("Create failed windows error<%d> status[%d]")
                          , err, StatusDefs::IOCP_CreateCompletionPortFail);
        return StatusDefs::IOCP_CreateCompletionPortFail;
    }

    return StatusDefs::Success;
}

void FS_Iocp::Destroy()
{
    // 关闭完成端口
    if(LIKELY(_completionPort))
        CloseHandle(_completionPort);
    _completionPort = NULL;
}

Int32 FS_Iocp::Reg(SOCKET sockfd)
{
    // 关联IOCP 与 sockfd
    // completionKey传入的一个数值，完成时会原样传回来; NumberOfConcurrentThreads这个参数在关联完成端口时被忽略
    // completekey可以是自定义的结构体指针或者其他数据的指针，便于获取完成状态时候识别 当处于关联时numofthread会被忽略
    HANDLE ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sockfd), _completionPort, ULONG_PTR(sockfd), 0);
    if(!ret)
    {
        auto err = GetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("Reg sockfd[%llu] to completionport failed windows error<%d> status[%d]")
                          , sockfd, err, StatusDefs::IOCP_RegSocketToCompletionPortFail);
        return StatusDefs::IOCP_RegSocketToCompletionPortFail;
    }

    return StatusDefs::Success;
}

Int32 FS_Iocp::Reg(SOCKET sockfd, void *ptr)
{
    // 关联IOCP 与 sockfd
    // completionKey传入的一个数值，完成时会原样传回来; NumberOfConcurrentThreads这个参数在关联完成端口时被忽略
    // completekey可以是自定义的结构体指针或者其他数据的指针，便于获取完成状态时候识别 当处于关联时numofthread会被忽略
    HANDLE ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sockfd), _completionPort, ULONG_PTR(ptr), 0);
    if(!ret)
    {
        auto err = GetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("Reg sockfd[%llu] ptr[%p] to completionport failed windows error<%d> status[%d]")
                          , sockfd, ptr, err, StatusDefs::IOCP_RegSocketToCompletionPortFail);
        return StatusDefs::IOCP_RegSocketToCompletionPortFail;
    }

    return StatusDefs::Success;
}

Int32 FS_Iocp::Reg(SOCKET sockfd, UInt64 sessionId)
{
    // 关联IOCP 与 sockfd
// completionKey传入的一个数值，完成时会原样传回来; NumberOfConcurrentThreads这个参数在关联完成端口时被忽略
// completekey可以是自定义的结构体指针或者其他数据的指针，便于获取完成状态时候识别 当处于关联时numofthread会被忽略
    HANDLE ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sockfd), _completionPort, ULONG_PTR(sessionId), 0);
    if(!ret)
    {
        auto err = GetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("Reg sockfd[%llu] clientId[%llu] to completionport failed windows error<%d> status[%d]")
                          , sockfd, sessionId, err, StatusDefs::IOCP_RegSocketToCompletionPortFail);
        return StatusDefs::IOCP_RegSocketToCompletionPortFail;
    }

    return StatusDefs::Success;
}

Int32 FS_Iocp::LoadAcceptEx(SOCKET listenSocket)
{
    if(_fnAcceptEx)
        return StatusDefs::Success;

    DWORD dwBytes = 0;
    GUID guidAcceptEx = WSAID_ACCEPTEX;
    if(WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                &guidAcceptEx, sizeof(guidAcceptEx),
                &_fnAcceptEx, sizeof(_fnAcceptEx),
                &dwBytes, NULL, NULL) != 0)
    {
        auto error = WSAGetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("load acceptex fail windows error[%d] statuscode[%d] IOCP_LoadAcceptExError")
                          , error, StatusDefs::IOCP_LoadAcceptExError);
        return StatusDefs::IOCP_LoadAcceptExError;
    }

    // 获取GetAcceptExSockAddrs函数指针
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    if(WSAIoctl(
        listenSocket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &GuidGetAcceptExSockAddrs,
        sizeof(GuidGetAcceptExSockAddrs),
        &_fnGetAcceptClientAddrIn,
        sizeof(_fnGetAcceptClientAddrIn),
        &dwBytes,
        NULL,
        NULL) != 0)
    {
        auto error = WSAGetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("load _fnGetAcceptClientAddrIn fail windows error[%d] statuscode[%d] IOCP_LoadGetAcceptExSockAddrFunFail")
                          , error, StatusDefs::IOCP_LoadGetAcceptExSockAddrFunFail);
        return StatusDefs::IOCP_LoadGetAcceptExSockAddrFunFail;
    }

    return StatusDefs::Success;
}
#pragma endregion

#pragma region
Int32 FS_Iocp::PostAccept(SOCKET listenSocket, IoDataBase *ioData)
{
    ioData->_ioType = IocpDefs::IO_ACCEPT;
    ioData->_sock = socket(AF_INET, __FS_TCP_SOCKTYPE__, IPPROTO_TCP);
    if(!_fnAcceptEx(listenSocket
                    , ioData->_sock
                    , ioData->_wsaBuff.buf
                    , 0
                    , sizeof(sockaddr_in) + 16     // msdn指定参数
                    , sizeof(sockaddr_in) + 16     // msdn指定参数
                    , NULL
                    , &ioData->_overlapped)) // 重叠体的地址必须是自定义结构体的初始位置，必须是自定义结构体的第一个成员可以是自定义的结构体
    {
        auto error = WSAGetLastError();
        if(error != ERROR_IO_PENDING)
        {
            g_Log->e<FS_Iocp>(_LOGFMT_("PostAccept listenSocket[%llu] ioSocket[%llu] to completionport failed windows error<%d> status[%d]")
                              , listenSocket, ioData->_sock, error, StatusDefs::IOCP_PostAcceptFail);
            return StatusDefs::IOCP_PostAcceptFail;
        }
    }

    return StatusDefs::Success;
}

Int32 FS_Iocp::PostRecv(SOCKET targetSock, IoDataBase *ioData)
{
    ioData->_ioType = IocpDefs::IO_RECV;
    DWORD flags = 0;
    memset(&ioData->_overlapped, 0, sizeof(ioData->_overlapped));
    if(SOCKET_ERROR == WSARecv(targetSock, &ioData->_wsaBuff, 1, NULL, &flags, &ioData->_overlapped, NULL))
    {
        auto error = WSAGetLastError();
        if(error != ERROR_IO_PENDING)
        {
            // 对端关闭
            if(error == WSAECONNRESET)
            {
                g_Log->net<FS_Iocp>("targetSock[%llu]An existing connection was forcibly closed by the remote host. win error<%d> status<%d>"
                           ,targetSock, error, StatusDefs::IOCP_ClientForciblyClosed);
                return StatusDefs::IOCP_ClientForciblyClosed;
            }

            g_Log->e<FS_Iocp>(_LOGFMT_("PostRecv ioSocket[%llu] to completionport failed windows error<%d> status[%d]")
                              , targetSock, error, StatusDefs::IOCP_PostRecvFail);
            return StatusDefs::IOCP_PostRecvFail;
        }
    }

    return StatusDefs::Success;
}

Int32 FS_Iocp::PostSend(SOCKET targetSock, IoDataBase *ioData)
{
    ioData->_ioType = IocpDefs::IO_SEND;
    DWORD flags = 0;
    memset(&ioData->_overlapped, 0, sizeof(ioData->_overlapped));
    if(SOCKET_ERROR == WSASend(targetSock, &ioData->_wsaBuff, 1, NULL, flags, &ioData->_overlapped, NULL))
    {
        auto error = WSAGetLastError();
        if(error != ERROR_IO_PENDING)
        {
            // 对端关闭
            if(error == WSAECONNRESET)
            {
                g_Log->net<FS_Iocp>("targetSock[%llu] An existing connection was forcibly closed by the remote host. win error<%d> status<%d>"
                           ,targetSock, error, StatusDefs::IOCP_ClientForciblyClosed);
                return StatusDefs::IOCP_ClientForciblyClosed;
            }

            g_Log->e<FS_Iocp>(_LOGFMT_("PostSend ioSocket[%llu] to completionport failed windows error<%d> status[%d]")
                              , targetSock, error, StatusDefs::IOCP_PostSendFail);
            return StatusDefs::IOCP_PostSendFail;
        }
    }

    return StatusDefs::Success;
}

Int32 FS_Iocp::PostQuit()
{
    if(false == PostQueuedCompletionStatus(_completionPort, 0, ULONG_PTR(IocpDefs::IO_QUIT), NULL))
    {
        auto err = GetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("PostQuit win error<%d> status[%d]")
                          , err, StatusDefs::IOCP_PostQuitFail);
        return StatusDefs::IOCP_PostQuitFail;
    }

    return StatusDefs::Success;
}

Int32 FS_Iocp::WaitForCompletion(IoEvent &ioEvent, Int32 &errorCode, ULong millisec)    // clientId为完成键
{
    // 获取完成端口状态
    // 关键在于 completekey(关联iocp端口时候传入的自定义完成键)会原样返回
    // 以及重叠结构ioDataPtr 用于获取数据重叠结构会原样返回
    ioEvent._bytesTrans = 0;
    ioEvent._ioData = NULL;
    ioEvent._sessionId = 0;
    errorCode = StatusDefs::Success;
    if(FALSE == GetQueuedCompletionStatus(_completionPort
                                          , LPDWORD(&ioEvent._bytesTrans)
                                          , reinterpret_cast<PULONG_PTR>(&ioEvent._sessionId)
                                          , reinterpret_cast<LPOVERLAPPED *>(&ioEvent._ioData)
                                          , millisec))
    {
        const Int32 error = GetLastError();
        if (WAIT_TIMEOUT == error)
        {
            errorCode = StatusDefs::IOCP_WaitTimeOut;
            return StatusDefs::Ignore;
        }

        DWORD flags = 0;
        DWORD bytesTransfer = 0;
        IoDataBase *ioData = reinterpret_cast<IoDataBase *>(ioEvent._ioData);
        if (ioData)
        {
#if _DEBUG
            BOOL grRet =
#endif
                ::WSAGetOverlappedResult(ioData->_sock,
                (LPOVERLAPPED)(ioEvent._ioData),
                    &bytesTransfer,
                    TRUE,
                    &flags);
#if _DEBUG
            ASSERT(grRet == FALSE && "library internal error, in GetQueuedCompletionStatus()!");
#endif
        }
        
        if(ERROR_NETNAME_DELETED == error)
        {
//             g_Log->net<FS_Iocp>("WaitForMessage session closed sessionId[%llu] bytesTrans<%lld> error<%d> status[%d]"
//                        , ioEvent._data._sessionId,ioEvent._bytesTrans, error, StatusDefs::IOCP_IODisconnect);
//             g_Log->any<FS_Iocp>("WaitForMessage client closed sockfd=%llu\n error<%d> status[%d]"
//                                 , ioEvent._ioData->_sock, error, StatusDefs::IOCP_IODisconnect);
            // 此时ioevent的数据被正确的填充，只是ioEvent._bytesTrans<=0这个事件可以在recv事件做处理
            // closesocket(ioEvent._ioData->_sock);
            // return StatusDefs::IOCP_IODisconnect;
            return StatusDefs::Success;
        }

        if(ERROR_CONNECTION_ABORTED == error)
        {// TODO:这个错误码要不要处理 本系统终止网络连接
//             g_Log->net<FS_Iocp>("local system closesocket sessionId[%llu] bytesTrans<%lld>. WaitForMessage invalid client socket error<%d> status<%d>"
//                               , ioEvent._data._sessionId, ioEvent._bytesTrans, error, StatusDefs::Unknown);
            return StatusDefs::Ignore;
        }

        if(ERROR_SEM_TIMEOUT == error)
        {// TODO:这个错误码要不要处理 压力过大可以重新投递相应的数据
//             g_Log->w<FS_Iocp>(_LOGFMT_("pressure is too large for this machine."
//                                        " please improve machine performance or "
//                                        "expand net card bandwidth error<%d> status<%d>"
//                                        "sessionId<%llu> bytesTrans<%lld>")
//                               , error
//                               , StatusDefs::Unknown
//                               , ioEvent._data._sessionId
//                               , ioEvent._bytesTrans);
            return StatusDefs::Success;
        }

        if(ERROR_OPERATION_ABORTED == error)
        {// 由于其他原因或者调用CancelIo，CancelIoEx等导致io被取消 此时bytestransfer为0 可以断开session连接
//             g_Log->net<FS_Iocp>("windows error<%d> sessionId[%llu] bytesTrans[%lld] io data has be abort. check if invoke CancelIo or CancelIoEx, or other reason."
//                               , error, ioEvent._data._sessionId, ioEvent._bytesTrans);
            return StatusDefs::Success;
        }

//         if (ERROR_ABANDONED_WAIT_0 == error)
//         {// session断开 与ERROR_NETNAME_DELETED处理相同
//             errorCode = StatusDefs::Session_Abort;
//             return StatusDefs::Ignore;
//         }

        g_Log->e<FS_Iocp>(_LOGFMT_("sessionId[%llu] _bytesTrans[%lld] WaitForMessage other error error<%d> status[%d]\n"
                                   "StackBackTrace:\n")
                          , ioEvent._sessionId
                          , ioEvent._bytesTrans
                          , error
                          , StatusDefs::IOCP_WaitOtherError);

        return StatusDefs::IOCP_WaitOtherError;
    }

    return StatusDefs::Success;
}

void FS_Iocp::GetClientAddrInfo(void *wsaBuff, sockaddr_in *&clientAddr)
{
    // todo 需要测试返回的clientAddr是否是系统内核对应的 sockaddr_in信息，还是说可以外部提供一个sockaddr_in对象传入，然后系统去填充
    // 测试方法：可以先传入sockaddr_in的实体对象，判断实体对象最终有没被填充，再传入一个指针，判断该指针是否是系统对应的wsabuff的指针
    sockaddr_in *localAddr = NULL;
    Int32 localLen = sizeof(sockaddr_in);
    Int32 remoteLen = sizeof(sockaddr_in);
    _fnGetAcceptClientAddrIn(wsaBuff
                             , 0
                             , sizeof(sockaddr_in) + 16     // msdn指定参数
                             , sizeof(sockaddr_in) + 16     // msdn指定参数
                             , (LPSOCKADDR *)(&localAddr)    // 获取本地地址
                             , &localLen
                             , (LPSOCKADDR *)(&clientAddr)   // 获取远程客户端地址
                             , &remoteLen);
}

#pragma endregion

FS_NAMESPACE_END

#endif
