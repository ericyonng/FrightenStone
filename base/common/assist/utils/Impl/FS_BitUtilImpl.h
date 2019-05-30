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
 * @file  : FS_BitUtilImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/30
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Assist_Utils_Impl_FS_BitUtil_H__
#pragma once

template <typename T>
inline bool FS_BitUtil::IsSet(T val, UInt32 pos)
{
    return (val & (static_cast<T>(1) << pos)) != 0;
}

template <typename T>
inline T FS_BitUtil::Set(T val, UInt32 pos, bool flag)
{
    if(flag)
        return val | (static_cast<T>(1) << pos);

    return val &= ~(static_cast<T>(1) << pos);
}

#endif // __Base_Common_Assist_Utils_Impl_FS_BitUtil_H__

