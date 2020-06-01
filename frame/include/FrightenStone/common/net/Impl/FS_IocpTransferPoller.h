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
 * @file  : FS_IocpTransferPoller.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/05
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpTransferPoller_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpTransferPoller_H__

#pragma once

#ifdef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_BasePoller.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class IFS_NetModule;
class FS_Iocp;
struct IoEvent;

class BASE_EXPORT FS_IocpTransferPoller : public IFS_BasePoller
{
public:
    FS_IocpTransferPoller(IFS_EngineComp *engineComp);
    ~FS_IocpTransferPoller();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();

    virtual IFS_NetModule *GetNetModuleObj();

private:
    void _Monitor(FS_ThreadPool *pool);

private:
    std::atomic_bool _isInit;
    FS_ThreadPool *_pool;
    FS_Iocp *_iocp;
    IoEvent *_ioEv;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_IocpTransferPollerImpl.h"

#endif

#endif
