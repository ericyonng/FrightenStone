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
 * @file  : IFS_NetEngine.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/28
 * @brief :
 * 
 *
 * 
 */
#pragma once
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_NetEngine_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_NetEngine_H__

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class TimeSlice;
struct NetEngineTotalCfgs;

class BASE_EXPORT IFS_NetEngine
{
public:
    IFS_NetEngine();
    virtual ~IFS_NetEngine();

public:
    virtual Int32 Init();
    virtual Int32 Start();
    virtual void Wait();
    virtual void Close();

protected:
    virtual void _Monitor(FS_ThreadPool *threadPool);
    void _PrintInfo(const TimeSlice &dis);

    /* 启服关服时序调用 */
protected:
    Int32 _ReadConfig();
    Int32 _CreateNetModules();
    Int32 _StartModules();

    Int32 _BeforeStart();
    Int32 _OnStart();
    Int32 _AfterStart();

    void _WillClose();
    void _BeforeClose();
    void _AfterClose();

    /* 配置 */
protected:
    // 读取配置位置
    virtual Int32 _OnReadCfgs() = 0;
    // 初始化结束时
    virtual Int32 _OnInitFinish() { return StatusDefs::Success; }

protected:
    NetEngineTotalCfgs *_totalCfgs;
    std::atomic_bool _isInit;

    Locker _locker;
};


FS_NAMESPACE_END

#endif