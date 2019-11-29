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
 * @file  : FS_Iocp.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/7
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_Iocp_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_Iocp_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

struct BASE_EXPORT IoDataBase;
struct BASE_EXPORT IO_EVENT;

class BASE_EXPORT FS_Iocp
{
    OBJ_POOL_CREATE_DEF(FS_Iocp);
public:
    FS_Iocp();
    virtual ~FS_Iocp();

    /* 初始化与注册 */
    #pragma region init/reg
    /*
    *   brief:
    *       1. - Create 创建端口
    *       2. - Destroy 销毁端口
    *       3. - reg 关联端口 reg时候会把自定义的地址数据ulong_ptr传入在wait时候传回，
    *                         便于确认wait传回的是哪一个绑定在完成端口的数据
    *       4. - reg 关联端口，并传入自定义数据作为ulong_ptr的数据
    *       5. - LoadAcceptEx 加载accept函数，
    *                         因为跨dll调用是有损耗的所以要自己提前加载到内存中
    */
public:
    Int32 Create();
    void Destroy();
    Int32 Reg(SOCKET sockfd);
    Int32 Reg(SOCKET sockfd, void *ptr);    // 第二个参数是完成时回传的completekey
    Int32 Reg(SOCKET sockfd, UInt64 sessionId); // 第二个参数是完成时回传的completekey
    Int32 LoadAcceptEx(SOCKET listenSocket);
    #pragma endregion

    /* iocp的操作 */
    #pragma region
    /*
    *   brief:
    *       1. - PostAccept accept时候会把重叠体传入，在wait时候传回
    *                       传入重叠体结构便于区分绑定在完成端口上的socket中的子操作，
    *                       比如绑定的是监听socket但是可能被多个客户端连接，
    *                       为了区分是哪个客户端需要多传入一个重叠体结构以便区分
    *       2. - PostRecv 投递接收数据
    *       3. - PostSend 投递发送数据
    *       4. - PostQuit 投递退出
    *       5. - WaitForCompletion 等待数据io端口完成，
    *                              getqueue是线程安全，
    *                              它会一个一个消息的从完成队列中取出不需要担心线程安全问题
    */
public:
    // post线程安全
    Int32 PostAccept(SOCKET listenSocket, IoDataBase *ioData);
    Int32 PostRecv(SOCKET targetSock, IoDataBase *ioData);
    Int32 PostSend(SOCKET targetSock, IoDataBase *ioData);

    Int32 PostQuit();
    Int32 WaitForCompletion(IO_EVENT &ioEvent, ULong millisec = INFINITE);
    void GetClientAddrInfo(void *wsaBuff, sockaddr_in *&clientAddr);
    #pragma endregion

private:
    HANDLE _completionPort = NULL;
    LPFN_ACCEPTEX _fnAcceptEx = NULL;
    LPFN_GETACCEPTEXSOCKADDRS _fnGetAcceptClientAddrIn = NULL;
};

FS_NAMESPACE_END

#endif
