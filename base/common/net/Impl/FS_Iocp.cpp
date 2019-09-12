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
 * @file  : FS_Iocp.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/7
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_Iocp.h"
#include "base/common/log/Log.h"
#include "base/common/status/status.h"
#include "base/common/net/Defs/IocpDefs.h"
#include "base/common/crashhandle/CrashHandle.h"

#pragma region windows api
#include <windows.h>
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")
 // #pragma comment (lib, "MSWSock.lib")
#pragma endregion


FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_IMPL(FS_Iocp, _objPoolHelper, __DEF_OBJ_POOL_OBJ_NUM__)

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

Int32 FS_Iocp::Reg(SOCKET sockfd, UInt64 clientId)
{
    // 关联IOCP 与 sockfd
// completionKey传入的一个数值，完成时会原样传回来; NumberOfConcurrentThreads这个参数在关联完成端口时被忽略
// completekey可以是自定义的结构体指针或者其他数据的指针，便于获取完成状态时候识别 当处于关联时numofthread会被忽略
    HANDLE ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sockfd), _completionPort, ULONG_PTR(clientId), 0);
    if(!ret)
    {
        auto err = GetLastError();
        g_Log->e<FS_Iocp>(_LOGFMT_("Reg sockfd[%llu] clientId[%llu] to completionport failed windows error<%d> status[%d]")
                          , sockfd, clientId, err, StatusDefs::IOCP_RegSocketToCompletionPortFail);
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
        g_Log->e<FS_Iocp>(_LOGFMT_("load acceptex fail windows error[%d] statuscode[%d]")
                          , error, StatusDefs::IOCP_LoadAcceptExError);
        return StatusDefs::IOCP_LoadAcceptExError;
    }

    return StatusDefs::Success;
}
#pragma endregion

#pragma region
Int32 FS_Iocp::PostAccept(SOCKET listenSocket, IO_DATA_BASE *ioData)
{
    ioData->_ioType = IocpDefs::IO_ACCEPT;
    ioData->_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

Int32 FS_Iocp::PostRecv(SOCKET targetSock, IO_DATA_BASE *ioData)
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

Int32 FS_Iocp::PostSend(SOCKET targetSock, IO_DATA_BASE *ioData)
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

Int32 FS_Iocp::WaitForCompletion(IO_EVENT &ioEvent, ULong millisec)    // clientId为完成键
{
    // 获取完成端口状态
    // 关键在于 completekey(关联iocp端口时候传入的自定义完成键)会原样返回
    // 以及重叠结构ioDataPtr 用于获取数据重叠结构会原样返回
    ioEvent._bytesTrans = 0;
    ioEvent._ioData = NULL;
    ioEvent._data._clientId = 0;
    if(FALSE == GetQueuedCompletionStatus(_completionPort
                                          , &ioEvent._bytesTrans
                                          , reinterpret_cast<PULONG_PTR>(&ioEvent._data)
                                          , reinterpret_cast<LPOVERLAPPED *>(&ioEvent._ioData)
                                          , millisec))
    {
        const Int32 error = GetLastError();
        if(WAIT_TIMEOUT == error)
        {
//             g_Log->net<FS_Iocp>("WaitForMessage time out error<%d> status[%d]"
//                        , error, StatusDefs::IOCP_WaitTimeOut);
//             g_Log->any<FS_Iocp>("WaitForMessage time out error<%d> status[%d]"
//                                 , error, StatusDefs::IOCP_WaitTimeOut);
            return StatusDefs::IOCP_WaitTimeOut;
        }

        if(ERROR_NETNAME_DELETED == error)
        {
            g_Log->net<FS_Iocp>("WaitForMessage client closed clientId=%llu\n error<%d> status[%d]"
                       , ioEvent._ioData->_ownerId, error, StatusDefs::IOCP_IODisconnect);
//             g_Log->any<FS_Iocp>("WaitForMessage client closed sockfd=%llu\n error<%d> status[%d]"
//                                 , ioEvent._ioData->_sock, error, StatusDefs::IOCP_IODisconnect);
            // 此时ioevent的数据被正确的填充，只是ioEvent._bytesTrans<=0这个事件可以在recv事件做处理
            // closesocket(ioEvent._ioData->_sock);
            // return StatusDefs::IOCP_IODisconnect;
            return StatusDefs::Success;
        }

        if(ERROR_CONNECTION_ABORTED == error)
        {// TODO:这个错误码要不要处理
            g_Log->w<FS_Iocp>(_LOGFMT_("clientId[%llu] WaitForMessage invalid client socket error<%d> status<%d>")
                              , ioEvent._data._clientId, error, StatusDefs::Unknown);
            return StatusDefs::Success;
        }
        if(ERROR_SEM_TIMEOUT == error)
        {// TODO:这个错误码要不要处理 压力过大可以重新投递相应的数据
            g_Log->w<FS_Iocp>(_LOGFMT_("pressure is too large for this machine."
                                       " please improve machine performance or "
                                       "expand net card bandwidth error<%d> status<%d>"
                                       "clientId<%llu> bytesTrans<%lu>")
                              , error
                              , StatusDefs::Unknown
                              , ioEvent._data._clientId
                              , ioEvent._bytesTrans);
            return StatusDefs::Success;
        }

        const auto &stackBackTrace = CrashHandleUtil::FS_CaptureStackBackTrace();
        g_Log->e<FS_Iocp>(_LOGFMT_("clientId[%llu] WaitForMessage other error error<%d> status[%d]\n"
                                   "StackBackTrace:\n%s")
                          , ioEvent._data._clientId
                          , error
                          , StatusDefs::IOCP_PostSendFail
                          , stackBackTrace.c_str());

        return StatusDefs::IOCP_WaitOtherError;
    }

    return StatusDefs::Success;
}
#pragma endregion

FS_NAMESPACE_END
