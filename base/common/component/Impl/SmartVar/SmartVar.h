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
    // 类型识别信息32bit 每个数据类型都有独立的mask 互斥
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
        SV_BRIEF_BYTE8      = SV_BRIEF_SIGHED_DATA      | (0x1U << 2),                     // Byte8
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

        union BriefDataType
        {
            Int64 _int64Data;
            UInt64 _uint64Data;
            Double _doubleData;
        }_briefData;

        union ObjType
        {
            Dict *_dictData;
            Str *_strData;
        }_obj;
    };

public:
   SmartVar();

    // Constructors(all parameter constructors is explicit, copy constructor is non-explicit).
    SmartVar(const bool &boolVal);
    SmartVar(const Byte8 &byte8Val);
    SmartVar(const U8 &uint8Val);
    SmartVar(const Int16 &int16Val);
    SmartVar(const UInt16 &uint16Val);
    SmartVar(const Int32 &int32Val);
    SmartVar(const UInt32 &uint32Val);
    SmartVar(const Long &longVal);
    SmartVar(const ULong &ulongVal);
    template <typename _T>
    SmartVar(const _T * const &ptrVal);
    SmartVar(const Int64 &int64Val);
    SmartVar(const UInt64 &uint64Val);
    SmartVar(const Float &floatVal);
    SmartVar(const Double &doubleVal);
    SmartVar(const Byte8 *cstrVal);
    SmartVar(const std::string &strVal);
    SmartVar(const FS_String &strVal);
    SmartVar(const Dict &dictVal);
    SmartVar(const SmartVar &varVal);

    // Fetch smartvar data type and raw data.
    UInt32 GetType() const;
    const struct Raw &GetRaw() const;

    // Type diagnose.
    bool IsNil() const;
    bool IsBriefData() const;
    bool IsSignedBriefData() const;
    bool IsUnsignedBriefData() const;
    bool IsBool() const;
    bool IsByte8() const;
    bool IsUInt8() const;
    bool IsInt16() const;
    bool IsUInt16() const;
    bool IsInt32() const;
    bool IsUInt32() const;
    bool IsLong() const;
    bool IsULong() const;
    bool IsPtr() const;
    bool IsInt64() const;
    bool IsUInt64() const;
    bool IsFloat() const;
    bool IsDouble() const;
    bool IsStr() const;
    bool IsDict() const;

    // Type convert.
    SmartVar &BecomeNil();
    SmartVar &BecomeBool();
    SmartVar &BecomeByte8();
    SmartVar &BecomeUInt8();
    SmartVar &BecomeInt16();
    SmartVar &BecomeUInt16();
    SmartVar &BecomeInt32();
    SmartVar &BecomeUInt32();
    SmartVar &BecomeLong();
    SmartVar &BecomeULong();
    SmartVar &BecomePtr();
    SmartVar &BecomeInt64();
    SmartVar &BecomeUInt64();
    SmartVar &BecomeFloat();
    SmartVar &BecomeDouble();
    SmartVar &BecomeStr();
    SmartVar &BecomeDict();

    // Real data fetch.
    bool    AsBool() const;
    Byte8   AsByte8() const;
    U8      AsUInt8() const;
    Int16   AsInt16() const;
    UInt16  AsUInt16() const;
    Int32   AsInt32() const;
    UInt32  AsUInt32() const;
    Long    AsLong() const;
    ULong   AsULong() const;
    template <typename _T>
    _T *    AsPtr() const;
    Int64   AsInt64() const;
    UInt64  AsUInt64() const;
    Float   AsFloat() const;
    Double  AsDouble() const;
    FS_String AsStr() const;
    const Dict &AsDict() const;
// 
//     operator bool() const;
//     operator sint8 () const;
//     operator uint8 () const;
//     operator sint16 () const;
//     operator uint16 () const;
//     operator sint32 () const;
//     operator uint32 () const;
//     operator long() const;
//     operator ulong () const;
//     template <typename _T>
//     operator _T * () const;
//     operator sint64 () const;
//     operator uint64 () const;
//     operator float() const;
//     operator double() const;
//     operator LLBC_String () const;
//     operator const Dict &() const;

        // Dictionary type variant object specify operate methods.
//     DictIter Begin();
//     DictIter End();
//     DictConstIter Begin() const;
//     DictConstIter End() const;
//     DictReverseIter ReverseBegin();
//     DictReverseIter ReverseEnd();
//     DictConstReverseIter ReverseBegin() const;
//     DictConstReverseIter ReverseEnd() const;
// 
//     std::pair<DictIter, bool> Insert(const Dict::key_type &key, const Dict::mapped_type &val);
//     std::pair<DictIter, bool> Insert(const Dict::value_type &val);
// 
//     template <typename _Kty, typename _Ty>
//     std::pair<DictIter, bool> Insert(const _Kty &key, const _Ty &val);
// 
//     DictIter Find(const Dict::key_type &key);
//     DictConstIter Find(const Dict::key_type &key) const;
// 
//     template <typename _Kty>
//     DictIter Find(const _Kty &key);
//     template <typename _Kty>
//     DictConstIter Find(const _Kty &key) const;
// 
//     void Erase(DictIter it);
//     Dict::size_type Erase(const Dict::key_type &key);
//     void Erase(DictIter first, DictIter last);
// 
//     template <typename _Kty>
//     Dict::size_type Erase(const _Kty &key);
// 
//     Dict::mapped_type &operator [](const Dict::key_type &key);
//     const Dict::mapped_type &operator [](const Dict::key_type &key) const;
// 
//     template <typename _Kty>
//     Dict::mapped_type &operator [](const _Kty &key);
//     template <typename _Kty>
//     const Dict::mapped_type &operator [](const _Kty &key) const;
// 
//     // assignment operators.
//     LLBC_Variant &operator =(sint8 val);
//     LLBC_Variant &operator =(uint8 val);
//     LLBC_Variant &operator =(sint16 val);
//     LLBC_Variant &operator =(uint16 val);
//     LLBC_Variant &operator =(sint32 val);
//     LLBC_Variant &operator =(uint32 val);
//     LLBC_Variant &operator =(long val);
//     LLBC_Variant &operator =(unsigned long val);
//     template <typename _T>
//     LLBC_Variant &operator =(const _T * const &val);
//     LLBC_Variant &operator =(const sint64 &val);
//     LLBC_Variant &operator =(const uint64 &val);
//     LLBC_Variant &operator =(float val);
//     LLBC_Variant &operator =(const double &val);
//     LLBC_Variant &operator =(const LLBC_String &val);
//     LLBC_Variant &operator =(const Dict &val);
//     LLBC_Variant &operator =(const LLBC_Variant &val);
// 
//     // Relational operators.
//     bool operator ==(const LLBC_Variant &another) const;
//     bool operator !=(const LLBC_Variant &another) const;
// 
//     bool operator <(const LLBC_Variant &another) const;
//     bool operator >(const LLBC_Variant &another) const;
//     bool operator <=(const LLBC_Variant &another) const;
//     bool operator >=(const LLBC_Variant &another) const;
// 
//     // Arithmetic operators.
//     LLBC_Variant operator +(const LLBC_Variant &another) const;
//     LLBC_Variant operator -(const LLBC_Variant &another) const;
//     LLBC_Variant operator *(const LLBC_Variant &another) const;
//     LLBC_Variant operator /(const LLBC_Variant &another) const;
// 
//     LLBC_Variant &operator +=(const LLBC_Variant &another);
//     LLBC_Variant &operator -=(const LLBC_Variant &another);
//     LLBC_Variant &operator *=(const LLBC_Variant &another);
//     LLBC_Variant &operator /=(const LLBC_Variant &another);
// 
//     // Type to string.
//     const LLBC_String &TypeToString() const;
//     // Value to string.
//     LLBC_String ValueToString() const;
//     // To string.
//     LLBC_String ToString() const;
// 
//     // Serialize / DeSerialize support.
//     void Serialize(LLBC_Stream &stream) const;
//     bool DeSerialize(LLBC_Stream &stream);
// 
//     // SerializeEx / DeSerializeEx support.
//     void SerializeEx(LLBC_Stream &stream) const;
//     bool DeSerializeEx(LLBC_Stream &stream);
// 
// public:
//     friend std::ostream &::operator <<(std::ostream &o, const LLBC_Variant &variant);
// 
// private:
//         friend class SmartVarTraits;
// 
//         void SetType(int type);
// 
//         Raw &_GetRaw();
// 
        void _CleanBriefData();
        void _CleanStrData();
        void _CleanDictData();
         void _CleanTypeData(UInt32 type);
//         void _OptimizePerformance();

private:
    Raw _raw;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/SmartVar/SmartVarImpl.h"

#endif
