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
 * @file  : SmartVarTraits.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/31
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_SmartVar_SmartVarTraits__H__
#define __Base_Common_Component_Impl_SmartVar_SmartVarTraits__H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"

FS_NAMESPACE_BEGIN
class SmartVar;

class BASE_EXPORT SmartVarTraits
{
public:
    /**
     * Assign operator.
     */
    static void assign(SmartVar &left, const SmartVar &right);

    /**
     * Relational operators.
     * ==, !=, <, >, <=, >=
     */
    static bool eq(const SmartVar &left, const SmartVar &right);
    static bool ne(const SmartVar &left, const SmartVar &right);
    static bool lt(const SmartVar &left, const SmartVar &right);
    static bool gt(const SmartVar &left, const SmartVar &right);
    static bool le(const SmartVar &left, const SmartVar &right);
    static bool ge(const SmartVar &left, const SmartVar &right);

    /**
     * Arithmetic operators.
     * +, -, *, /
     */
    static SmartVar add(const SmartVar &left, const SmartVar &right);
    static SmartVar sub(const SmartVar &left, const SmartVar &right);
    static SmartVar mul(const SmartVar &left, const SmartVar &right);
    static SmartVar div(const SmartVar &left, const SmartVar &right);

    /**
     * Arithmetic operators.
     * +=, -=, *=, /=
     */
    static void add_equal(SmartVar &left, const SmartVar &right);
    static void sub_equal(SmartVar &left, const SmartVar &right);
    static void mul_equal(SmartVar &left, const SmartVar &right);
    static void div_equal(SmartVar &left, const SmartVar &right);
};
FS_NAMESPACE_END

#endif
