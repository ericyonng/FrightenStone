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
 * @file  : Trigger.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/component/Impl/Trigger.h"
#include "FrightenStone/common/component/Defs/TriggerDefs.h"
#include "FrightenStone/common/assist/utils/utils.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/log/Log.h"

FS_NAMESPACE_BEGIN

Trigger::Trigger(IDelegate<bool, Int32> *canInterrupt)
    :_canInterrupt(canInterrupt)
    ,_isExecing(0)
{

}

Trigger::~Trigger()
{
    Fs_SafeFree(_canInterrupt);
    STLUtil::DelMapContainer(_occasions);
}

Int32 Trigger::Reg(Int32 occasion, Int32 triggerType, const IDelegate<Int32, TriggerExecuteBody *> &exec, Int32 execTimes /*= 1 */, Int32 addType /*= TriggerDefs::AddIfExist*/)
{
    if(_IsExecing())
    {
        _DelayReg(occasion, triggerType, exec.CreateNewCopy(), execTimes, addType);
        return StatusDefs::Success;
    }

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
    _BeforeExec();

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

    _AfterExec();
    
    return StatusDefs::Success;
}

void Trigger::EraseAllTrigger(Int32 triggerType)
{
    if(_IsExecing())
    {
        _DelayEraseAllTrigger(triggerType);
        return;
    }

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

FS_String Trigger::ToString() const
{
    FS_String info;
    info.AppendFormat("\nTrigger info:\n");
    for(auto iterBodys = _occasions.begin(); iterBodys != _occasions.end(); ++iterBodys)
        info.AppendFormat("%s\n", iterBodys->second->ToString().c_str());

    return info;
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

void Trigger::_AfterExec()
{
    if(--_isExecing > 0)
        return;

    _isExecing = 0;

    for(auto iterOp = _delayOp.begin(); iterOp != _delayOp.end();)
    {
        ITriggerOpInfo *delayOp = *iterOp;
        switch(delayOp->_opType)
        {
            case TriggerOpType::Reg:
            {
                RegTriggerInfo *regInfo = static_cast<RegTriggerInfo *>(delayOp);
                const Int32 st = _Reg(regInfo->_occasionType
                                     , regInfo->_triggerType
                                     , regInfo->_callback
                                     , regInfo->_execTimes
                                     , regInfo->_triggerType);
                if(st != StatusDefs::Success)
                    g_Log->w<Trigger>(_LOGFMT_("delay reg failed st[%d]"), st);
            }
            break;
            case TriggerOpType::Erase:
            {
                EraseTriggerTypeInfo *eraseInfo = static_cast<EraseTriggerTypeInfo *>(delayOp);
                EraseAllTrigger(eraseInfo->_triggerType);
            }
            break;
            default:
            {
                g_Log->w<Trigger>(_LOGFMT_("unknown delay op type[%d]"), delayOp->_opType);
            }
            break;
        }

        Fs_SafeFree(delayOp);
        iterOp = _delayOp.erase(iterOp);
    }
}

void Trigger::_DelayReg(Int32 occasion
                        , Int32 triggerType
                        , IDelegate<Int32, TriggerExecuteBody *> *exec // 外部请释放delegate，内部会创建delegate的副本
                        , Int32 execTimes
                        , Int32 addType)
{
    RegTriggerInfo *info = new RegTriggerInfo;
    info->_opType = TriggerOpType::Reg;
    info->_callback = exec;
    info->_execTimes = execTimes;
    info->_occasionType = occasion;
    info->_triggerType = triggerType;
    info->_addType = addType;
    _delayOp.push_back(info);
}

Int32 Trigger::_Reg(Int32 occasion
                   , Int32 triggerType
                   , IDelegate<Int32, TriggerExecuteBody *> *exec /* 外部请释放delegate，内部会创建delegate的副本 */
                   , Int32 execTimes /*= 1 */
                   , Int32 addType /*= TriggerDefs::AddIfExist*/)
{
    if(_IsExecing())
    {
        _DelayReg(occasion, triggerType, exec, execTimes, addType);
        return StatusDefs::Success;
    }

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

void Trigger::_DelayEraseAllTrigger(Int32 triggerType)
{
    EraseTriggerTypeInfo *info = new EraseTriggerTypeInfo;
    info->_opType = TriggerOpType::Erase;
    info->_triggerType = triggerType;
    _delayOp.push_back(info);
}
FS_NAMESPACE_END


