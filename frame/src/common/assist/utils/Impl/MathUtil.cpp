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
 * @file  : MathUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/assist/utils/Impl/MathUtil.h"

FS_NAMESPACE_BEGIN

Int64 MathUtil::GetGcd(Int64 a, Int64 b)
{
    // 当b为0时,得出结果,a既为结果
    Int64 t = 0;
    while(b)
    {
        // 存b
        t = b;
        // b为a对b取模
        b = ( a % b);
        // a为上一次的b
        a = t;
    }

    return a;
}

Int64 MathUtil::GetLcm(Int64 a, Int64 b)
{
    return a * b / GetGcd(a, b);
}

FS_NAMESPACE_END

