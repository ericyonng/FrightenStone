#ifndef __Base_Common_Component_Defs_TriggerDefs_H__
#define __Base_Common_Component_Defs_TriggerDefs_H__
/**
* @file TriggerDefs.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/ForAll/ForAllMacro.h"
#include "base/common/basedefs/DataType/DataType.h"
#include <functional>
#include <set>
#include <map>

FS_NAMESPACE_BEGIN

class BASE_EXPORT Trigger;

class BASE_EXPORT TriggerDefs
{
public:
    // 多个触发执行体加入方法
    enum AddMultiTriggerType
    {
        NotAddIfExist = 0,              // 若存在则不添加
        AddIfExist = 1,                 // 若存在也可以加入
        EraseAllBeforeAdd = 2,          // 若存在则先移除所有
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

    Int32 _execTimes;
    Int32 _triggerType;
    std::function<Int32(TriggerExecuteBody *)> _exec;
};

// 不支持时间顺序执行每个事件，同一triggerType每个时机中只能注册一次
class BASE_EXPORT TriggerOccasion
{
public:
    TriggerOccasion(Trigger *trigger, Int32 occasion);
    virtual ~TriggerOccasion();

public:
    Int32 Reg(Int32 triggerType, const std::function<Int32(TriggerExecuteBody *)> &exec, Int32 execTimes = 1);
    Int32 Exec(std::set<Int32> &triggerType2Erase);
    void Erase(Int32 triggerType);
    bool IsExist(Int32 triggerType) const;

private:
    std::map<Int32, TriggerExecuteBody *>::iterator _Erase(const std::map<Int32, TriggerExecuteBody *>::iterator &iter);

private:
    Trigger *_trigger;
    Int32 _occasion;                                                    // 执行的时机
    std::map<Int32, TriggerExecuteBody *> _triggerTypeRefBodys;         // 需要释放 
};


FS_NAMESPACE_END

#endif
