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
 * @file  : FS_EpollMsgTransfer.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/20
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollMsgTransfer_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollMsgTransfer_H__

#pragma once

#ifndef _WIN32
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN
struct BriefSessionInfo;
class FS_EpollTransferPoller;

class BASE_EXPORT FS_EpollMsgTransfer : public IFS_MsgTransfer
{
public:
    FS_EpollMsgTransfer(UInt32 compId, IFS_NetEngine *engine);
    ~FS_EpollMsgTransfer();

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &totalCfgs);
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();

public:
    virtual bool IsIoFinish() const;
    // ªÒ»°poller
    virtual IFS_BasePoller *GetPoller();

private:
    std::atomic_bool _isInit;
    FS_EpollTransferPoller *_poller;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_EpollMsgTransferImpl.h"

#endif

#endif
