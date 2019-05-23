/**
* @file Trigger.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/24
* @brief
*/
#include "stdafx.h"
#include "base/common/component/Impl/Trigger.h"
#include "base/common/component/Defs/TriggerDefs.h"
#include "base/common/assist/utils/utils.h"
#include "base/common/status/status.h"

FS_NAMESPACE_BEGIN

Trigger::Trigger(const std::function<bool(Int32 thisOccation)> &canInterrupt)
    :_canInterrupt(canInterrupt)
{

}

Trigger::~Trigger()
{
    STLUtil::DelMapContainer(_occasions);
}

Int32 Trigger::Reg(Int32 occasion, Int32 triggerType, const std::function<Int32(TriggerExecuteBody *)> &exec, Int32 execTimes /*= 1 */, Int32 addType /*= TriggerDefs::AddIfExist*/)
{
    switch(addType)
    {
        case TriggerDefs::NotAddIfExist:
        {
            for(auto iterOccation = _occasions.begin(); iterOccation != _occasions.end(); ++iterOccation)
            {
                if(UNLIKELY(iterOccation->second->IsExist(triggerType)))
                    return StatusDefs::Repeat;
            }
        }
        break;
        case TriggerDefs::AddIfExist:
        {
        }
        break;
        case TriggerDefs::EraseAllBeforeAdd:
        {
            for(auto iterOccation = _occasions.begin(); iterOccation != _occasions.end(); ++iterOccation)
            {
                if(iterOccation->second->IsExist(triggerType))
                    iterOccation->second->Erase(triggerType);
            }
        }
        break;
        default:
        {
            return StatusDefs::Trigger_UnkownAddType;
        }
        break;
    }

    auto iterBodys = _occasions.find(occasion);
    if(UNLIKELY(iterBodys == _occasions.end()))
    {
        auto newOccasion = new TriggerOccasion(this, occasion);
        iterBodys = _occasions.insert(std::make_pair(occasion, newOccasion)).first;
    }

    if(addType == TriggerDefs::AddIfExist)
    {
        if(UNLIKELY(iterBodys->second->IsExist(triggerType)))
            return StatusDefs::Trigger_TriggerTypeRepeatInOccasion;
    }

    const auto ret = iterBodys->second->Reg(triggerType, exec, execTimes);
    if(ret == StatusDefs::Success)
    {
        auto iterOccations = _triggerTypeRefOccasions.find(triggerType);
        if(iterOccations == _triggerTypeRefOccasions.end())
            iterOccations = _triggerTypeRefOccasions.insert(std::make_pair(triggerType, std::set<Int32>())).first;

        auto iterData = iterOccations->second.find(occasion);
        if(iterData == iterOccations->second.end())
            iterOccations->second.insert(occasion);
    }

    return ret;
}

Int32 Trigger::Exec(Int32 occasion)
{
    auto iterBodys = _occasions.find(occasion);
    if(LIKELY(iterBodys != _occasions.end()))
    {
        auto bodys = iterBodys->second;
        std::set<Int32> triggerType2Erase;
        const auto execSt = bodys->Exec(triggerType2Erase);

        _CancelTriggerTypeOccasionsRelevance(triggerType2Erase, occasion);
        if(execSt != StatusDefs::Success)
            return execSt;
    }
    
    return StatusDefs::Success;
}

void Trigger::EraseAllTrigger(Int32 triggerType)
{
    auto iterOccations = _triggerTypeRefOccasions.find(triggerType);
    if(UNLIKELY(iterOccations == _triggerTypeRefOccasions.end()))
        return;

    auto &occations = iterOccations->second;
    for(auto iterOcation = occations.begin(); iterOcation != occations.end(); ++iterOcation)
    {
        auto iterBodys = _occasions.find(*iterOcation);
        if(LIKELY(iterBodys != _occasions.end()))
        {
            auto bodys = iterBodys->second;
            bodys->Erase(triggerType);
        }
    }

    _triggerTypeRefOccasions.erase(iterOccations);
}


void Trigger::Clear()
{
    _triggerTypeRefOccasions.clear();
    STLUtil::DelMapContainer(_occasions);
}


bool Trigger::IsExist(Int32 occasionType, Int32 triggerType)
{
    auto iterExecBodys = _occasions.find(occasionType);
    if(iterExecBodys == _occasions.end())
        return false;

    auto bodys = iterExecBodys->second;
    return bodys->IsExist(triggerType);
}

void Trigger::_CancelTriggerTypeOccasionsRelevance(const std::set<Int32> &triggerTypes, Int32 occasion)
{
    for(auto iterTriggerType = triggerTypes.begin(); iterTriggerType != triggerTypes.end(); ++iterTriggerType)
    {
        auto iterOccations = _triggerTypeRefOccasions.find(*iterTriggerType);
        if(UNLIKELY(iterOccations == _triggerTypeRefOccasions.end()))
            continue;

        iterOccations->second.erase(occasion);
    }
}

FS_NAMESPACE_END
