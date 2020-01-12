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
 * @file  : FS_LogImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Src_Common_Log_Impl_Log_H__
#pragma once

FS_NAMESPACE_BEGIN

inline IDelegate<void, const LogData *> *FS_Log::_InstallLogHookFunc(Int32 level, IDelegate<void, const LogData *> *delegate)
{
    if(!_levelRefHook[level])
        _levelRefHook[level] = new  std::list<IDelegate<void, const LogData *> *>;

    _levelRefHook[level]->push_back(delegate);
    return delegate;
}

inline const IDelegate<void, LogData *> *FS_Log::_InstallBeforeLogHookFunc(Int32 level, IDelegate<void, LogData *> *delegate)
{
    if(!_levelRefBeforeLogHook[level])
        _levelRefBeforeLogHook[level] = new  std::list<IDelegate<void, LogData *> *>;

    _levelRefBeforeLogHook[level]->push_back(delegate);
    return delegate;
}

inline bool FS_Log::_IsAllowToConsole(Int32 level) const
{
    if(level == LogLevel::Net ||
       level == LogLevel::MemPool ||
       level == LogLevel::ObjPool)
        return false;

    return true;
}

FS_NAMESPACE_END

#endif
