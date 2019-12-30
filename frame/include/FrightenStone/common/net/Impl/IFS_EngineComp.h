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
 * @file  : IFS_EngineComp.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/30
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_EngineComp_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_EngineComp_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

class IFS_NetEngine;

class BASE_EXPORT IFS_EngineComp
{
public:
    IFS_EngineComp(IFS_NetEngine *engine, Int32 compId);
    virtual ~IFS_EngineComp() {}

public:
    // 获取组件id
    virtual UInt32 GetCompId() const;
    // 获取生产者id
    virtual UInt32 GetGeneratorId() const;
    // 获取消费者id
    virtual UInt32 GetConsumerId() const;
    // 绑定生产者id
    virtual void BindGeneratorId(UInt32 generatorId);
    // 绑定消费者id
    virtual void BindConsumerId(UInt32 consumerId);
    // 引擎对象
    IFS_NetEngine *GetEngine();

protected:
    UInt32 _compId;
    UInt32 _generatorId;
    UInt32 _consumerId;
    IFS_NetEngine *_engine;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_EngineCompImpl.h"

#endif
