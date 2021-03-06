/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : FS_TimerImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/06/08
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Component_Impl_TimeWheel_FS_Timer_H__
#pragma once


FS_NAMESPACE_BEGIN

// inline void FS_Timer::SetTimeWheel(TimeWheel *timeWheel)
// {
//     _timeWheel = timeWheel;
// }

inline Int32 FS_Timer::Schedule(Int64 milliSecPeriod)
{
    return Schedule(Time::Now(), milliSecPeriod);
}

inline bool FS_Timer::IsScheduling() const
{
    return _timeWheel->IsRegister(_timeData);
}

template<typename ObjType>
inline void FS_Timer::SetTimeOutHandler(ObjType *objType, void (ObjType::*handler)(FS_Timer *, const Time &, const Time &))
{
    if(UNLIKELY(_timeOutDelegate))
        Fs_SafeFree(_timeOutDelegate);

    if(objType && handler)
        _timeOutDelegate = DelegatePlusFactory::Create(objType, handler);
}

inline void FS_Timer::SetTimeOutHandler(void(*handler)(FS_Timer *, const Time &, const Time &))
{
    if(UNLIKELY(_timeOutDelegate))
        Fs_SafeFree(_timeOutDelegate);

    if(handler)
        _timeOutDelegate = DelegatePlusFactory::Create(handler);
}

template<typename ObjType>
inline void FS_Timer::SetCancelHandler(ObjType *objType, void (ObjType::*handler)(FS_Timer *))
{
    if(UNLIKELY(_cancelTimerDelegate))
        Fs_SafeFree(_cancelTimerDelegate);

    if(objType && handler)
        _cancelTimerDelegate = DelegatePlusFactory::Create(objType, handler);
}

inline void FS_Timer::SetCancelHandler(void(*handler)(FS_Timer *))
{
    if(UNLIKELY(_cancelTimerDelegate))
        Fs_SafeFree(_cancelTimerDelegate);

    if(handler)
        _cancelTimerDelegate = DelegatePlusFactory::Create(handler);
}

FS_NAMESPACE_END


#endif // __Frame_Include_FrightenStone_Common_Component_Impl_TimeWheel_FS_Timer_H__

