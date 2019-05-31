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
 * @file  : SmartVarTraits.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/31
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/SmartVar/SmartVar.h"
#include "base/common/component/Impl/SmartVar/SmartVarTraits.h"

FS_NAMESPACE_BEGIN

void SmartVarTraits::assign(SmartVar &left, const SmartVar &right)
{
    // TODO
//     if(&left == &right)
//         return;
// 
//     // Execute assignment.
//     if(right.IsNil())// Do NIL type data assignment.
//     {
//         left.BecomeNil();
//     }
//     else if(right.IsRaw()) // Do RAW type data assignment.
//     {
//         // If left type is STR/DICT type, clean first.
//         if(left.IsStr())
//             left.CleanStrData();
//         else if(left.IsDict())
//             left.CleanDictData();
// 
//         // Assignment.
//         left.SetType(right.GetType());
//         left.GetRaw().raw.uint64Val = right.GetHolder().raw.uint64Val;
//     }
//     else if(right.IsStr()) // Do STR type data assignment.
//     {
//         // If left type is RAW/DICT type, clean first.
//         if(left.IsRaw())
//             left.CleanRawData();
//         else if(left.IsDict())
//             left.CleanDictData();
// 
//         // Assignment.
//         left.SetType(right.GetType());
//         LLBC_Variant::Holder &lHolder = left.GetHolder();
//         const LLBC_Variant::Holder &rHolder = right.GetHolder();
//         if(rHolder.obj.str == NULL || rHolder.obj.str->empty())
//         {
//             if(lHolder.obj.str)
//                 lHolder.obj.str->clear();
//         }
//         else
//         {
//             if(lHolder.obj.str == NULL)
//                 lHolder.obj.str = LLBC_New1(LLBC_String, *rHolder.obj.str);
//             else if(lHolder.obj.str != rHolder.obj.str)
//                 *lHolder.obj.str = *rHolder.obj.str;
//         }
//     }
//     else if(right.IsDict()) // Do DICT type data assignment.
//     {
//         // If left type is RAW/STR type, clean first.
//         if(left.IsRaw())
//             left.CleanRawData();
//         else if(left.IsStr())
//             left.CleanStrData();
// 
//         left.SetType(right.GetType());
//         LLBC_Variant::Holder &lHolder = left.GetHolder();
//         const LLBC_Variant::Holder &rHolder = right.GetHolder();
//         if(rHolder.obj.dict == NULL || rHolder.obj.dict->empty())
//         {
//             if(lHolder.obj.dict)
//                 lHolder.obj.dict->clear();
//         }
//         else
//         {
//             if(lHolder.obj.dict == NULL)
//                 lHolder.obj.dict = LLBC_New1(LLBC_Variant::Dict, *rHolder.obj.dict);
//             else if(lHolder.obj.dict != rHolder.obj.dict)
//                 *lHolder.obj.dict = *rHolder.obj.dict;
//         }
//     }
}

bool SmartVarTraits::eq(const SmartVar &left, const SmartVar &right)
{
    return false;
}

bool SmartVarTraits::ne(const SmartVar &left, const SmartVar &right)
{
    return false;
}

bool SmartVarTraits::lt(const SmartVar &left, const SmartVar &right)
{
    return false;
}

bool SmartVarTraits::gt(const SmartVar &left, const SmartVar &right)
{
    return false;
}

bool SmartVarTraits::le(const SmartVar &left, const SmartVar &right)
{
    return false;
}

bool fs::SmartVarTraits::ge(const SmartVar &left, const SmartVar &right)
{
    return false;
}

SmartVar SmartVarTraits::add(const SmartVar &left, const SmartVar &right)
{
    return SmartVar();
}

SmartVar SmartVarTraits::sub(const SmartVar &left, const SmartVar &right)
{
    return SmartVar();
}

SmartVar SmartVarTraits::mul(const SmartVar &left, const SmartVar &right)
{
    return SmartVar();
}

SmartVar SmartVarTraits::div(const SmartVar &left, const SmartVar &right)
{
    return SmartVar();
}

void SmartVarTraits::add_equal(SmartVar &left, const SmartVar &right)
{

}

void SmartVarTraits::sub_equal(SmartVar &left, const SmartVar &right)
{

}

void SmartVarTraits::mul_equal(SmartVar &left, const SmartVar &right)
{

}

void SmartVarTraits::div_equal(SmartVar &left, const SmartVar &right)
{

}

FS_NAMESPACE_END
