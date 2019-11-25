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
 * @file  : TriggerDefs.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Defs/TriggerDefs.h"
#include "base/common/assist/utils/utils.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/Trigger.h"
#include <algorithm>

FS_NAMESPACE_BEGIN

TriggerExecuteBody::TriggerExecuteBody(Int32 triggerType, Int32 execTimes /*= 1*/)
    :_execTimes(execTimes)
    ,_triggerType(triggerType)
    ,_exec(NULL)
{

}

TriggerExecuteBody::~TriggerExecuteBody()
{
    Fs_SafeFree(_exec);
}

FS_String TriggerExecuteBody::ToString() const
{
    return FS_String().AppendFormat("_triggerType[%d] _execTimes[%d]",
                                       _triggerType, _execTimes);
}

TriggerOccasion::TriggerOccasion(Trigger *trigger, Int32 occasion)
    :_trigger(trigger)
    , _occasion(occasion)
{

}

TriggerOccasion::~TriggerOccasion()
{
    STLUtil::DelMapContainer(_triggerTypeRefBodys);
}

Int32 TriggerOccasion::Reg(Int32 triggerType, const IDelegate<Int32, TriggerExecuteBody *> &exec, Int32 execTimes /*= 1*/)
{
    if(UNLIKELY(execTimes != TriggerDefs::Trig_Infinite && execTimes < 0))
        return StatusDefs::ParamError;

    auto iterTrigger = _triggerTypeRefBodys.find(triggerType);
    if(UNLIKELY(iterTrigger != _triggerTypeRefBodys.end()))
        return StatusDefs::Trigger_TriggerTypeRepeatInOccasion;

    TriggerExecuteBody *newBody = new TriggerExecuteBody(triggerType, execTimes);
    newBody->_exec = exec.CreateNewCopy();
    _triggerTypeRefBodys.insert(std::make_pair(triggerType, newBody));

    return StatusDefs::Success;
}

Int32 TriggerOccasion::Reg(Int32 triggerType, IDelegate<Int32, TriggerExecuteBody *> *exec, Int32 execTimes /*= 1*/)
{
    if(UNLIKELY(execTimes != TriggerDefs::Trig_Infinite && execTimes < 0))
        return StatusDefs::ParamError;

    auto iterTrigger = _triggerTypeRefBodys.find(triggerType);
    if(UNLIKELY(iterTrigger != _triggerTypeRefBodys.end()))
        return StatusDefs::Trigger_TriggerTypeRepeatInOccasion;

    TriggerExecuteBody *newBody = new TriggerExecuteBody(triggerType, execTimes);
    newBody->_exec = exec;
    _triggerTypeRefBodys.insert(std::make_pair(triggerType, newBody));

    return StatusDefs::Success;
}

Int32 TriggerOccasion::Exec(std::set<Int32> &triggerType2Erase)
{
    for(auto iterBody = _triggerTypeRefBodys.begin(); iterBody != _triggerTypeRefBodys.end();)
    {
        Int32 ret = StatusDefs::Success;
        const Int32 triggerType = iterBody->first;
        auto body = iterBody->second;
        if(body->_execTimes != 0)
        {
            ret = body->_exec->Invoke(body);
            if(body->_execTimes != TriggerDefs::Trig_Infinite &&
               ret == StatusDefs::Success)
                body->_execTimes = std::max<Int32>(body->_execTimes - 1, 0);
        }

        if(body->_execTimes == 0)
        {
            triggerType2Erase.insert(triggerType);
            iterBody = _Erase(iterBody);
        }
        else
        {
            ++iterBody;
        }

        if(ret != StatusDefs::Success)
        {
            if(_trigger->_canInterrupt->Invoke(_occasion))
                return ret;
        }
    }

    return StatusDefs::Success;
}

void TriggerOccasion::Erase(Int32 triggerType)
{
    auto iterBody = _triggerTypeRefBodys.find(triggerType);
    if(iterBody == _triggerTypeRefBodys.end())
        return;

    FsSafeDelete(iterBody->second);
    _triggerTypeRefBodys.erase(iterBody);
}

bool TriggerOccasion::IsExist(Int32 triggerType) const
{
    auto iterTrigger = _triggerTypeRefBodys.find(triggerType);
    return iterTrigger != _triggerTypeRefBodys.end();
}

FS_String TriggerOccasion::ToString() const
{
    FS_String info;
    info.AppendFormat("TriggerOccasion _execOccasion[%d] info:\n", _occasion);
    for(auto iterBody = _triggerTypeRefBodys.begin(); iterBody != _triggerTypeRefBodys.end(); ++iterBody)
        info.AppendFormat("%s\n", iterBody->second->ToString().c_str());

    return info;
}

std::map<Int32, TriggerExecuteBody *>::iterator TriggerOccasion::_Erase(const std::map<Int32, TriggerExecuteBody *>::iterator &iter)
{
    FsSafeDelete(iter->second);
    return _triggerTypeRefBodys.erase(iter);
}

ITriggerOpInfo::ITriggerOpInfo()
    :_opType(TriggerOpType::Invalid)
{

}

RegTriggerInfo::RegTriggerInfo()
    :_occasionType(0)
    ,_triggerType(0)
    ,_callback(NULL)
    ,_execTimes(1)
    ,_addType(TriggerDefs::AddIfExist)
{
    _opType = TriggerOpType::Reg;
}

RegTriggerInfo::~RegTriggerInfo()
{
    _callback = NULL;
}

EraseTriggerTypeInfo::EraseTriggerTypeInfo()
    :_triggerType(0)
{
    _opType = TriggerOpType::Erase;
}

FS_NAMESPACE_END
