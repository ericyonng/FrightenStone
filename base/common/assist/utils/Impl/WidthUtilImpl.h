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
 * @file  : WidthUtilImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Assist_Utils_Impl_WidthUtil_H__


#pragma once

FS_NAMESPACE_BEGIN

template<typename T>
inline int WidthUtil::GetBinaryWidth()
{
    int cnt = 1;
    auto max = (std::numeric_limits<T>::max)();
    while((max /= 2) != 0)
        ++cnt;

    return cnt;
}

template<typename T>
inline int WidthUtil::GetBinaryWidth(T val)
{
    int cnt = 1;
    while((val /= 2) != 0)
        ++cnt;

    return cnt;
}

template<typename T>
inline int WidthUtil::GetDecimalWidth()
{
    int cnt = 1;
    T max = (std::numeric_limits<T>::max)();
    while((max /= 10) != 0)
        ++cnt;

    return cnt;
}

template<typename T>
inline int WidthUtil::GetDecimalWidth(T val)
{
    int cnt = 1;
    while((val /= 10) != 0)
        ++cnt;

    return cnt;
}

FS_NAMESPACE_END

#endif
