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
 * @file  : SmartVarArithmetic.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_SmartVar_SmartVarArithmetic_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_SmartVar_SmartVarArithmetic_H__
#pragma once

#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
#include "FrightenStone/common/basedefs/DataType/DataType.h"

FS_NAMESPACE_BEGIN
class SmartVar;
FS_NAMESPACE_END

FS_NAMESPACE_BEGIN

class BASE_EXPORT SmartVarArithmetic
{
public:
    /**
     * Operations type enumeration.
     */
    enum
    {
        SV_ARITHMETIC_ADD = 0,
        SV_ARITHMETIC_SUB = 1,
        SV_ARITHMETIC_MUL = 2,
        SV_ARITHMETIC_DIV = 3,

        SV_ARITHMETIC_END
    };

    /**
     * Performs arithmetic.
     * @param[in/out] left - left object.
     * @param[in] right    - right object.
     * @param[in] type     - operation type, see enumerations value.
     */
    static void Performs(SmartVar &left, const SmartVar &right, int type);

private:
    /**
     * Implementation functions.
     */
    static void _Performs_bool_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_byte8_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_uint8_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_int16_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_uint16_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_int32_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_uint32_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_long_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_ulong_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_ptr_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_int64_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_uint64_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_float_any(SmartVar &left, const SmartVar &right, int type);
    static void _Performs_double_any(SmartVar &left, const SmartVar &right, int type);

    /**
     * Implementation function.
     */
    template <typename RawType>
    static RawType _Performs_raw_operation(RawType left, RawType right, int type);
};

FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/SmartVar/SmartVarArithmeticImpl.h"

#endif
