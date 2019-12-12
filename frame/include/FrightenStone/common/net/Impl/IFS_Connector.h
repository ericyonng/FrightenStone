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
 * @file  : IFS_Connector.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Connector_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Connector_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/InterfaceComps.h"
#include "FrightenStone/common/net/Defs/ServerCompsDef.h"
#include "FrightenStone/common/component/Impl/SmartVar/SmartVar.h"
#include "FrightenStone/common/status/status.h"

FS_NAMESPACE_BEGIN

class IFS_Session;
struct BriefSessionInfo;
class IUser;
struct FS_ConnectInfo;

class BASE_EXPORT IFS_Connector
{
public:
    IFS_Connector();
    virtual ~IFS_Connector();

public:
    virtual Int32 BeforeStart() { return StatusDefs::Success; }
    virtual Int32 Start() = 0;
    virtual Int32 AfterStart() { return StatusDefs::Success; }
    virtual void WillClose() {} // 断开与模块之间的依赖
    virtual void BeforeClose() {}
    virtual void Close() = 0;
    virtual void AfterClose() {}

    // 连接
    virtual Int32 Connect(const FS_ConnectInfo &connectInfo) = 0;
    virtual std::map<UInt64, IUser *> &GetUsers() = 0;

    // 成功连接时的回调
    virtual void RegOnSucConnect(IDelegate<void, BriefSessionInfo *> *sucCallback) = 0;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_ConnectorImpl.h"

#endif
