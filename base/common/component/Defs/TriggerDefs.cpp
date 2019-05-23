/**
* @file TriggerDefs.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
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
{

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

Int32 TriggerOccasion::Reg(Int32 triggerType, const std::function<Int32(TriggerExecuteBody *)> &exec, Int32 execTimes /*= 1*/)
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
            ret = body->_exec(body);    

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
            if(_trigger->_canInterrupt(_occasion))
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

std::map<Int32, TriggerExecuteBody *>::iterator TriggerOccasion::_Erase(const std::map<Int32, TriggerExecuteBody *>::iterator &iter)
{
    FsSafeDelete(iter->second);
    return _triggerTypeRefBodys.erase(iter);
}

FS_NAMESPACE_END
