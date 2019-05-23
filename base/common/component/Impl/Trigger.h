#ifndef __Base_Common_Component_Impl_Trigger__H__
#define __Base_Common_Component_Impl_Trigger__H__
/**
* @file    Trigger.h
* @author  Huiya Song
* @date    2019/03/01
* @brief
*/

#pragma once

#include <base/exportbase.h>
#include "base/common/component/Defs/TriggerDefs.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include<functional>

FS_NAMESPACE_BEGIN

    class BASE_EXPORT TriggerOccasion;

    // 同类型事件只允许注册一个
    class BASE_EXPORT Trigger
    {
    public:
        Trigger(const std::function<bool(Int32 thisOccation)> &canInterrupt);
        virtual ~Trigger();

    public:
        Int32 Reg(Int32 occasion
                   , Int32 triggerType
                   , const std::function<Int32(TriggerExecuteBody *)> &exec
                   , Int32 execTimes = 1
                   , Int32 addType = TriggerDefs::AddIfExist); // 注册时机

        Int32 Exec(Int32 occasion);

        void EraseAllTrigger(Int32 triggerType); // 移除触发
        void Clear();
        bool IsExist(Int32 occasionType, Int32 triggerType);

    private:
        void _CancelTriggerTypeOccasionsRelevance(const std::set<Int32> &triggerTypes, Int32 occasion);

    private:
        friend class TriggerOccasion;
        std::map<Int32, TriggerOccasion *> _occasions;     // key:occationtype
        std::map<Int32, std::set<Int32>> _triggerTypeRefOccasions;        // key:triggerType, val:set(occations) 用于已知trigerType将任意时机的trigger清理
        const std::function<bool(Int32 thisOccation)> _canInterrupt;        // 是否可被打断
    };

FS_NAMESPACE_END

#endif // !__Base_Common_Component_Impl_Trigger__H__