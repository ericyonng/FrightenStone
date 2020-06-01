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
 * @file  : SmartVarArithmetic.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/6/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/component/Impl/SmartVar/SmartVar.h"
#include "FrightenStone/common/component/Impl/SmartVar/SmartVarArithmetic.h"

FS_NAMESPACE_BEGIN

void SmartVarArithmetic::Performs(SmartVar &left, const SmartVar &right, int type)
{
    switch(left.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
            SmartVarArithmetic::_Performs_bool_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_BYTE8:
            SmartVarArithmetic::_Performs_byte8_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT8:
            SmartVarArithmetic::_Performs_uint8_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_INT16:
            SmartVarArithmetic::_Performs_int16_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT16:
            SmartVarArithmetic::_Performs_uint16_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_INT32:
            SmartVarArithmetic::_Performs_int32_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT32:
            SmartVarArithmetic::_Performs_uint32_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            SmartVarArithmetic::_Performs_long_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            SmartVarArithmetic::_Performs_ulong_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            SmartVarArithmetic::_Performs_ptr_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            SmartVarArithmetic::_Performs_int64_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            SmartVarArithmetic::_Performs_uint64_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
            SmartVarArithmetic::_Performs_float_any(left, right, type);
            break;

        case SmartVarRtti::SV_BRIEF_DOUBLE:
            SmartVarArithmetic::_Performs_double_any(left, right, type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_bool_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
            left = _Performs_raw_operation(left.AsBool(), right.AsBool(), type);
            break;

        case SmartVarRtti::SV_BRIEF_BYTE8:
            left = _Performs_raw_operation(left.AsByte8(), right.AsByte8(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT8:
            left = _Performs_raw_operation(left.AsUInt8(), right.AsUInt8(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT16:
            left = _Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT16:
            left = _Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT32:
            left = _Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT32:
            left = _Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_byte8_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
            left = _Performs_raw_operation(left.AsByte8(), right.AsByte8(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT8:
            left = _Performs_raw_operation(left.AsUInt8(), right.AsUInt8(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT16:
            left = _Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT16:
            left = _Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT32:
            left = _Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT32:
            left = _Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_uint8_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
            left = _Performs_raw_operation(left.AsUInt8(), right.AsUInt8(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT16:
            left = _Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT16:
            left = _Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT32:
            left = _Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT32:
            left = _Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_int16_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
            left = _Performs_raw_operation(left.AsInt16(), right.AsInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT16:
            left = _Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT32:
            left = _Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT32:
            left = _Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_uint16_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
            left = _Performs_raw_operation(left.AsUInt16(), right.AsUInt16(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT32:
            left = _Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT32:
            left = _Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_int32_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_INT32:
            left = _Performs_raw_operation(left.AsInt32(), right.AsInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT32:
            left = _Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_uint32_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_INT32:
        case SmartVarRtti::SV_BRIEF_UINT32:
            left = _Performs_raw_operation(left.AsUInt32(), right.AsUInt32(), type);
            break;

        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_long_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_INT32:
        case SmartVarRtti::SV_BRIEF_UINT32:
        case SmartVarRtti::SV_BRIEF_LONG:
            left = _Performs_raw_operation(left.AsLong(), right.AsLong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_ulong_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_INT32:
        case SmartVarRtti::SV_BRIEF_UINT32:
        case SmartVarRtti::SV_BRIEF_LONG:
        case SmartVarRtti::SV_BRIEF_ULONG:
            left = _Performs_raw_operation(left.AsULong(), right.AsULong(), type);
            break;

        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_ptr_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_INT32:
        case SmartVarRtti::SV_BRIEF_UINT32:
        case SmartVarRtti::SV_BRIEF_LONG:
        case SmartVarRtti::SV_BRIEF_ULONG:
        case SmartVarRtti::SV_BRIEF_PTR:
            left = _Performs_raw_operation(left.AsPtr<void>(), right.AsPtr<void>(), type);
            break;

        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_int64_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_INT32:
        case SmartVarRtti::SV_BRIEF_UINT32:
        case SmartVarRtti::SV_BRIEF_LONG:
        case SmartVarRtti::SV_BRIEF_ULONG:
        case SmartVarRtti::SV_BRIEF_PTR:
        case SmartVarRtti::SV_BRIEF_INT64:
            left = _Performs_raw_operation(left.AsInt64(), right.AsInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_uint64_any(
    SmartVar &left, const SmartVar &right, int type)
{
    switch(right.GetType())
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_INT32:
        case SmartVarRtti::SV_BRIEF_UINT32:
        case SmartVarRtti::SV_BRIEF_LONG:
        case SmartVarRtti::SV_BRIEF_ULONG:
        case SmartVarRtti::SV_BRIEF_PTR:
        case SmartVarRtti::SV_BRIEF_INT64:
        case SmartVarRtti::SV_BRIEF_UINT64:
            left = _Performs_raw_operation(left.AsUInt64(), right.AsUInt64(), type);
            break;

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
            break;

        default:
            break;
    }
}

void SmartVarArithmetic::_Performs_float_any(
    SmartVar &left, const SmartVar &right, int type)
{
    left = static_cast<float>(_Performs_raw_operation(left.AsDouble(), right.AsDouble(), type));
}

void SmartVarArithmetic::_Performs_double_any(
    SmartVar &left, const SmartVar &right, int type)
{
    left = _Performs_raw_operation(left.AsDouble(), right.AsDouble(), type);
}

FS_NAMESPACE_END
