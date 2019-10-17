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
 * @file  : FS_IocpMsgTransfer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "base/common/net/Impl/FS_IocpMsgTransfer.h"

#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"

FS_NAMESPACE_BEGIN
FS_IocpMsgTransfer::FS_IocpMsgTransfer()
    :_threadPool(NULL)
{
}

FS_IocpMsgTransfer::~FS_IocpMsgTransfer()
{
}

Int32 FS_IocpMsgTransfer::BeforeStart()
{
    _threadPool = new FS_ThreadPool(0, 1);
    return StatusDefs::Success;
}

Int32 FS_IocpMsgTransfer::Start()
{
    return StatusDefs::Success;
}

void FS_IocpMsgTransfer::BeforeClose()
{
}

void FS_IocpMsgTransfer::Close()
{
}

void FS_IocpMsgTransfer::OnConnect(IFS_Session *session)
{
    _locker.Lock();
    _willAddSessions.push_back(session);
    _locker.Unlock();
}

void FS_IocpMsgTransfer::OnDestroy()
{

}

void FS_IocpMsgTransfer::OnHeartBeatTimeOut()
{
}

Int32 FS_IocpMsgTransfer::GetSessionCnt()
{
    return 0;
}

void FS_IocpMsgTransfer::_OnMoniterMsg(const FS_ThreadPool *pool)
{
    while(!pool->IsClearingPool())
    {
        // 1.将待连入的session转移到容器中
        // 2.投递接收数据事件
        // 3.
    }
}

FS_NAMESPACE_END
