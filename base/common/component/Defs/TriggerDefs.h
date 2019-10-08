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
 * @file  : TriggerDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Defs_TriggerDefs_H__
#define __Base_Common_Component_Defs_TriggerDefs_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/ForAll/ForAllMacro.h"
#include "base/common/basedefs/DataType/DataType.h"
#include <functional>
#include <set>
#include <map>
#include "base/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT Trigger;

class BASE_EXPORT TriggerDefs
{
public:
    enum AddMultiTriggerType
    {
        NotAddIfExist = 0,              // 
        AddIfExist = 1,                 // 
        EraseAllBeforeAdd = 2,          // 
    };

    enum
    {
        Trig_Infinite = -1,               // 无限触发次数
    };
};

struct BASE_EXPORT TriggerExecuteBody
{
public:
    TriggerExecuteBody(Int32 triggerType, Int32 execTimes = 1);
    ~TriggerExecuteBody();

    Int32 _execTimes;
    Int32 _triggerType;
    IDelegate<Int32, TriggerExecuteBody *> *_exec;
};

class BASE_EXPORT TriggerOccasion
{
public:
    TriggerOccasion(Trigger *trigger, Int32 occasion);
    virtual ~TriggerOccasion();

public:
    Int32 Reg(Int32 triggerType, const IDelegate<Int32, TriggerExecuteBody *> &exec, Int32 execTimes = 1);
    Int32 Exec(std::set<Int32> &triggerType2Erase);
    void Erase(Int32 triggerType);
    bool IsExist(Int32 triggerType) const;

private:
    std::map<Int32, TriggerExecuteBody *>::iterator _Erase(const std::map<Int32, TriggerExecuteBody *>::iterator &iter);

private:
    Trigger *_trigger;
    Int32 _occasion;                                                    // 
    std::map<Int32, TriggerExecuteBody *> _triggerTypeRefBodys;         //  
};


FS_NAMESPACE_END

#endif
