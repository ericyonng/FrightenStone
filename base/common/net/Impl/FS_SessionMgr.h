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
 * @file  : FS_SessionMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_SessionMgr_H__
#define __Base_Common_Net_Impl_FS_SessionMgr_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT IFS_Session;

class BASE_EXPORT FS_SessionMgr
{
public:
    FS_SessionMgr();
    ~FS_SessionMgr();

public:
    Int32 BeforeStart();
    Int32 Start();
    Int32 AfterStart();
    virtual void WillClose() {} // 断开与模块之间的依赖
    void BeforeClose();
    void Close();
    void AfterClose();

    void AddNewSession(UInt64 sessionId, IFS_Session *session);
    void EraseSession(UInt64 sessionId);

private:
    std::map<UInt64, IFS_Session *> _sessions;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_SessionMgrImpl.h"

#endif
