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
 * @file  : TimeWeel.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/6
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/assist/utils/utils.h"
#include "base/common/component/Impl/TimeWheel/Comp/TimeData.h"
#include "base/common/component/Impl/TimeWheel/TimeWheel.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/TimeSlice.h"
#include <iostream>

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_IMPL(TimeWheel, _objPoolHelper, 2)

TimeWheel::TimeWheel(const TimeSlice &resolutionSlice)
    :_resolutionSlice(resolutionSlice)
    ,_increaseId(0)
    ,_curTime(Time::Now())
    ,_rotating(0)
{

}

TimeWheel::~TimeWheel()
{
    STLUtil::DelVectorContainer(_asynData);
}

void TimeWheel::RotateWheel()
{
    _BeforeRotateWheel();

    // 更新时间
    if(UNLIKELY(_curTime == 0))
    {
        _curTime.FlushTime(Time::NowMicroTimestamp());
    }
    else
    {
        _curTime.FlushTime();
    }

    // 轮盘滚动时间需要大于等于最小扫描时间精度才处理
//     _nowTimeToJudgeWheelTimeOut.FlushTime();
//     if(UNLIKELY((_nowTimeToJudgeWheelTimeOut - _curTime) < _resolutionSlice))
//         return;

    // 遍历
    std::set<TimeData *> timeDataToRefresh;
    for(auto iterUniqueRefTimeDatas = _timeDatas.begin(); iterUniqueRefTimeDatas != _timeDatas.end();)
    {
        // 判断是否过期
        auto timeData = *iterUniqueRefTimeDatas;
        if(timeData->_expiredTime > _curTime)
            break;

        // 执行超时delegate
        timeData->_isRotatingWheel = true;
        if(!timeData->_isCancel)// 本轮循环中若被cancel掉也不会执行timeout
            timeData->_timer->OnTimeOut(_curTime);
        timeData->_isRotatingWheel = false;

        // 重新加入
        if(!timeData->_isCancel)
            timeDataToRefresh.insert(timeData);

        // 移除
        iterUniqueRefTimeDatas = _timeDatas.erase(iterUniqueRefTimeDatas);
    }

    // 更新超时时间并重新加入
    for(auto iterToRefresh = timeDataToRefresh.begin(); iterToRefresh != timeDataToRefresh.end(); ++iterToRefresh)
    {
        auto timeData = *iterToRefresh;
        timeData->_expiredTime.FlushTime(timeData->_expiredTime.GetMicroTimestamp() + timeData->_period.GetTotalMicroSeconds());
        _timeDatas.insert(timeData);
    }

    _AfterRotateWheel();
}

Int32 TimeWheel::Register(TimeData *timeData)
{
    if(timeData->_isRotatingWheel||_IsRotating())
    {
        _NewAsynTimeData(AsynOpType::Op_Register, timeData);
        return StatusDefs::TimeWheel_CantRegisterWhenRotatingWheel;
    }

    const auto expiredTime = timeData->_expiredTime.GetMicroTimestamp();
    if(UNLIKELY(!expiredTime))
        return StatusDefs::TimeWheel_ExpiredTimeIsNull;

    if(!timeData->_timeWheelUniqueId)
        timeData->_timeWheelUniqueId = _NewIncreasId();
    
    auto iterTimeData = _timeDatas.find(timeData);
    if(UNLIKELY(iterTimeData != _timeDatas.end()))
        return StatusDefs::TimeWheel_RegisterAExistsTimeData;

    _timeDatas.insert(timeData);

    return StatusDefs::Success;
}

void TimeWheel::_AfterRotateWheel()
{
    if(--_rotating <= 0)
    {
        _rotating = 0;
        for(auto &asynData : _asynData)
        {
            if(asynData->_opType == AsynOpType::Op_Register)
            {
                Register(asynData->_timeData);
            }
            else if(asynData->_opType == AsynOpType::Op_UnRegister)
            {
                UnRegister(asynData->_timeData);
            }
        }

        STLUtil::DelVectorContainer(_asynData);
        _asynRegisterFlag.clear();
    }
}

FS_NAMESPACE_END

