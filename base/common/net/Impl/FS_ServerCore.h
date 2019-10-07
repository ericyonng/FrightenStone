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
 * @file  : FS_ServerCore.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_ServerCore_H__
#define __Base_Common_Net_Impl_FS_ServerCore_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT IFS_ServerConfigMgr;
class BASE_EXPORT IFS_Connector;
class BASE_EXPORT IFS_MsgTransfer;
class BASE_EXPORT IFS_MsgHandler;
class BASE_EXPORT FS_SessionMgr;

class BASE_EXPORT FS_ServerCore
{
public:
    FS_ServerCore();
    virtual ~FS_ServerCore();

    /* 对外接口 */
    #pragma region api
public:
    virtual Int32 Start();
    virtual void Close();
    #pragma endregion

    /* 内部方法 */
    #pragma region inner api
private:
    Int32 _ReadConfig();
    Int32 _CreateNetModules();
    Int32 _StartModules();

    Int32 _BeforeStart();
    Int32 _OnStart();
    Int32 _AfterStart();
    #pragma endregion

private:
    IFS_ServerConfigMgr *_serverConfigMgr;
    IFS_Connector *_connector;
    IFS_MsgTransfer *_msgTransfer;
    IFS_MsgHandler *_msgHandler;
    FS_SessionMgr *_sessiomMgr;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_ServerCoreImpl.h"

#endif
