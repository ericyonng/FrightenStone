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
 * @file  : SmartVarArithmeticImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/6/5
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Component_Impl_SmartVar_SmartVarArithmetic_H__

#pragma once

FS_NAMESPACE_BEGIN

template <>
inline bool SmartVarArithmetic::_Performs_raw_operation(bool left, bool right, int type)
{
    switch(type)
    {
        case SV_ARITHMETIC_ADD:
            return left || right;

        case SV_ARITHMETIC_SUB:
            if(left) // true - true = false, true - false = true.
                return !right;
            else // false - true = false, false - false = false.
                return false;

        case SV_ARITHMETIC_MUL:
        case SV_ARITHMETIC_DIV:
            return left && right; // left == true and right == true return true, otherwise return false.

        default:
            break;
    }

    return bool();
}

template <>
inline void *SmartVarArithmetic::_Performs_raw_operation(void *left, void *right, int type)
{
    Int64 leftVal = 0, rightVal = 0;
    ::memcpy(&leftVal, &left, sizeof(void *));
    ::memcpy(&rightVal, &right, sizeof(void *));

    switch(type)
    {
        case SV_ARITHMETIC_ADD:
            leftVal += rightVal;
            break;

        case SV_ARITHMETIC_SUB:
            leftVal -= rightVal;
            break;

        case SV_ARITHMETIC_MUL:
            leftVal *= rightVal;
            break;

        case SV_ARITHMETIC_DIV:
            leftVal /= rightVal;
            break;

        default:
            leftVal = 0;
            break;
    }

    void *finalPtr = NULL;
    ::memcpy(&finalPtr, &leftVal, sizeof(void *));

    return finalPtr;
}

template <typename RawType>
inline RawType SmartVarArithmetic::_Performs_raw_operation(RawType left, RawType right, int type)
{
    switch(type)
    {
        case SV_ARITHMETIC_ADD:
            return left + right;

        case SV_ARITHMETIC_SUB:
            return left - right;

        case SV_ARITHMETIC_MUL:
            return left * right;

        case SV_ARITHMETIC_DIV:
            return left / right;

        default:
            break;
    }

    return RawType();
}

FS_NAMESPACE_END

#endif
