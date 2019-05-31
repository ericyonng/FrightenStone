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
 * @file  : SmartVar.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_SmartVar_SmartVar__H__
#define __Base_Common_Component_Impl_SmartVar_SmartVar__H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/component/Impl/FS_String.h"
#include <map>

FS_NAMESPACE_BEGIN

// RawData
// RTTI

// 运行时类型识别
class BASE_EXPORT SmartVarRtti
{
public:
    // 类型识别信息32bit 
    // [high eight bits][middle 23 bits][first bit]
    // [    类型信息   ][ 具体类型枚举 ][  符号位 ]
    enum RttiType:unsigned int
    {
        // 初始化类型
        SV_NIL = 0x0U,

        // 符号标记
        SV_SIGHED           = 0x01U,             // 有符号
        SV_UNSIGHED         = 0x0U,              // 无符号

        // 类型信息
        SV_BRIEF_DATA       = 0x01000000U,       // 基本数据类型
        SV_STRING           = 0x02000000U,       // 字符串类型
        SV_DICTIONARY       = 0x04000000U,       // 字典类型

        // 简单类型的具体类型枚举
        SV_BRIEF_SIGHED_DATA        = SV_BRIEF_DATA     | SV_SIGHED,                        // 有符号简单数据类型
        SV_BRIEF_UNSIGHED_DATA      = SV_BRIEF_DATA     | SV_UNSIGHED,                      // 无符号简单数据类型
        SV_BRIEF_BOOL       = SV_BRIEF_SIGHED_DATA      | (0x1U << 1),                     // bool类型
        SV_BRIEF_INT8       = SV_BRIEF_SIGHED_DATA      | (0x1U << 2),                     // I8
        SV_BRIEF_UINT8      = SV_BRIEF_UNSIGHED_DATA    | (0x1U << 3),                     // U8
        SV_BRIEF_INT16      = SV_BRIEF_SIGHED_DATA      | (0x1U << 4),                     // INT16
        SV_BRIEF_UINT16     = SV_BRIEF_UNSIGHED_DATA    | (0x1U << 5),                     // UINT16
        SV_BRIEF_INT32      = SV_BRIEF_SIGHED_DATA      | (0x1U << 6),                     // INT32
        SV_BRIEF_UINT32     = SV_BRIEF_UNSIGHED_DATA    | (0x1U << 7),                     // UINT32
        SV_BRIEF_LONG       = SV_BRIEF_SIGHED_DATA      | (0x1U << 8),                     // LONG
        SV_BRIEF_ULONG      = SV_BRIEF_UNSIGHED_DATA    | (0x1U << 9),                     // ULONG
        SV_BRIEF_PTR        = SV_BRIEF_UNSIGHED_DATA    | (0x1U << 10),                    // PTR
        SV_BRIEF_INT64      = SV_BRIEF_SIGHED_DATA      | (0x1U << 11),                    // INT64
        SV_BRIEF_UINT64     = SV_BRIEF_UNSIGHED_DATA    | (0x1U << 12),                    // UINT64
        SV_BRIEF_FLOAT      = SV_BRIEF_SIGHED_DATA      | (0x1U << 13),                    // float
        SV_BRIEF_DOUBLE     = SV_BRIEF_SIGHED_DATA      | (0x1U << 14),                    // double

        // 字符串类型
        SV_STRING_DEF       = SV_STRING | SV_SIGHED,        // 默认的字符串类型

        // 字典类型
        SV_DICTIONARY_DEF   = SV_DICTIONARY | SV_SIGHED,    // 默认的字典类型

        // 掩码
        SV_TYPEINFO_MASK    = 0xFFU << (32-8),                   // 高8位为类型信息掩码
        SV_SIGNED_MASK      = 0x01U,                                // 符号位
    };

    static const FS_String &GetTypeName(unsigned int rttiType);
    static void InitRttiTypeNames();
private:
    static std::map<unsigned int, FS_String> _rttiTypeRefString;   // 类型字符串
    static const FS_String _nullString;
};
FS_NAMESPACE_END

FS_NAMESPACE_BEGIN

class BASE_EXPORT SmartVar
{
public:
    typedef std::map<SmartVar, SmartVar> Dict;
    typedef FS_String Str;
    typedef Dict::iterator DictIter;
    typedef Dict::const_iterator DictConstIter;
    typedef Dict::reverse_iterator DictReverseIter;
    typedef Dict::const_reverse_iterator DictConstReverseIter;

public:
    struct BASE_EXPORT Raw
    {
        Raw();
        ~Raw();
        UInt32 _type;

        union
        {
            Int64 _int64Data;
            UInt64 _uint64Data;
            Double _doubleData;

            Dict *_dictData;
            Str *_strData;
        }_data;
    };

public:
   SmartVar();

    // Constructors(all parameter constructors is explicit, copy constructor is non-explicit).
    explicit SmartVar(const bool &boolVal);
    explicit SmartVar(const Byte8 &byte8Val);
    explicit SmartVar(const U8 &uint8Val);
    explicit SmartVar(const Int16 &int16Val);
    explicit SmartVar(const UInt16 &uint16Val);
    explicit SmartVar(const Int32 &int32Val);
    explicit SmartVar(const UInt32 &uint32Val);
    explicit SmartVar(const Long &longVal);
    explicit SmartVar(const ULong &ulongVal);
    template <typename _T>
    explicit SmartVar(const _T * const &ptrVal);
    explicit SmartVar(const Int64 &int64Val);
    explicit SmartVar(const UInt64 &uint64Val);
    explicit SmartVar(const Float &floatVal);
    explicit SmartVar(const Double &doubleVal);
    explicit SmartVar(const Byte8 *cstrVal);
    explicit SmartVar(const std::string &strVal);
    explicit SmartVar(const FS_String &strVal);
    explicit SmartVar(const Dict &dictVal);
    SmartVar(const SmartVar &varVal);

private:
    Raw _raw;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/SmartVar/SmartVarImpl.h"

#endif
