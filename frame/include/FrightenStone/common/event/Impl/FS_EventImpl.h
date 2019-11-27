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
 * @file  : FS_EventImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Event_Impl_FS_Event_H__
#pragma once

FS_NAMESPACE_BEGIN
inline int FS_Event::GetId() const
{
    return _id;
}

inline bool FS_Event::IsDontDelAfterFire() const
{
    return _dontDelAfterFire;
}

inline void FS_Event::SetDontDelAfterFire(bool dontDelAfterFire)
{
    _dontDelAfterFire = dontDelAfterFire;
}

template <typename ParamType>
inline FS_Event &FS_Event::SetParam(int key, const ParamType &param)
{
    const SmartVar varParam(param);
    return SetParam(key, varParam);
}

template <typename ParamType>
inline FS_Event &FS_Event::SetParam(const char *key, const ParamType &param)
{
    const SmartVar varParam(param);
    return SetParam(key, varParam);
}

template <typename ParamType>
inline FS_Event &FS_Event::SetParam(const FS_String &key, const ParamType &param)
{
    const SmartVar varParam(param);
    return SetParam(key, varParam);
}

inline size_t FS_Event::GetIntKeyParamsCount() const
{
    return _intKeyParams != NULL ? _intKeyParams->size() : 0;
}

inline size_t FS_Event::GetConstantStrKeyParamsCount() const
{
    return _constantStrKeyParams != NULL ? _constantStrKeyParams->size() : 0;
}

inline size_t FS_Event::GetStrKeyParamsCount() const
{
    return _strKeyParams != NULL ? _strKeyParams->size() : 0;
}

FS_NAMESPACE_END

#endif
