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
 * @file  : SmartVarImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_SmartVar_SmartVar_H__

#pragma once

FS_NAMESPACE_BEGIN

inline SmartVar::SmartVar()
{
}

inline SmartVar::SmartVar(const bool &boolVal)
{
    _raw._type = SmartVarRtti::SV_BRIEF_BOOL;
    _raw._briefData._int64Data = boolVal ? 1 : 0;
}

inline SmartVar::SmartVar(const Byte8 &byte8Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_BYTE8;
    _raw._briefData._int64Data = static_cast<Int64>(byte8Val);
}

inline SmartVar::SmartVar(const U8 &uint8Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_UINT8;
    _raw._briefData._uint64Data = static_cast<UInt64>(uint8Val);
}

inline SmartVar::SmartVar(const Int16 &int16Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_INT16;
    _raw._briefData._int64Data = static_cast<Int64>(int16Val);
}

inline SmartVar::SmartVar(const UInt16 &uint16Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_UINT16;
    _raw._briefData._uint64Data = static_cast<UInt64>(uint16Val);
}

inline SmartVar::SmartVar(const Int32 &int32Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_INT32;
    _raw._briefData._int64Data = static_cast<Int64>(int32Val);
}

inline SmartVar::SmartVar(const UInt32 &uint32Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_UINT32;
    _raw._briefData._uint64Data = static_cast<UInt64>(uint32Val);
}

inline SmartVar::SmartVar(const Long &longVal)
{
    _raw._type = SmartVarRtti::SV_BRIEF_LONG;
    _raw._briefData._int64Data = static_cast<Int64>(longVal);
}

inline SmartVar::SmartVar(const ULong &ulongVal)
{
    _raw._type = SmartVarRtti::SV_BRIEF_ULONG;
    _raw._briefData._uint64Data = static_cast<UInt64>(ulongVal);
}

template <typename _T>
inline SmartVar::SmartVar(const _T * const &ptrVal)
{
    _raw._type = SmartVarRtti::SV_BRIEF_PTR;
    _raw._briefData._uint64Data = 0;
    ::memcpy(&_raw._briefData._uint64Data, &ptrVal, sizeof(_T *));
}

inline SmartVar::SmartVar(const Int64 &int64Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_INT64;
    _raw._briefData._int64Data = static_cast<Int64>(int64Val);
}

inline SmartVar::SmartVar(const UInt64 &uint64Val)
{
    _raw._type = SmartVarRtti::SV_BRIEF_UINT64;
    _raw._briefData._uint64Data = static_cast<UInt64>(uint64Val);
}

inline SmartVar::SmartVar(const Float &floatVal)
{
    _raw._type = SmartVarRtti::SV_BRIEF_FLOAT;
    _raw._briefData._doubleData = static_cast<Double>(floatVal);
}

inline SmartVar::SmartVar(const Double &doubleVal)
{
    _raw._type = SmartVarRtti::SV_BRIEF_DOUBLE;
    _raw._briefData._doubleData = static_cast<Double>(doubleVal);
}

inline SmartVar::SmartVar(const std::string &strVal)
{
    _raw._type = SmartVarRtti::SV_STRING_DEF;
    if(LIKELY(!strVal.empty()))
        _raw._obj._strData = new FS_String(strVal);
}

inline SmartVar::SmartVar(const FS_String &strVal)
{
    _raw._type = SmartVarRtti::SV_STRING_DEF;
    if(LIKELY(!strVal.empty()))
        _raw._obj._strData = new FS_String(strVal);
}

inline SmartVar::SmartVar(const Dict &dictVal)
{
    _raw._type = SmartVarRtti::SV_DICTIONARY_DEF;
    if(!dictVal.empty())
        _raw._obj._dictData = new Dict(dictVal);
}

// Fetch smartvar data type and raw data.
inline UInt32 SmartVar::GetType() const
{
    return _raw._type;
}

inline const SmartVar::Raw &SmartVar::GetRaw() const
{
    return _raw;
}

inline bool SmartVar::IsNil() const
{
    return _raw._type == SmartVarRtti::SV_NIL;
}

inline bool SmartVar::IsBriefData() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_DATA) == SmartVarRtti::SV_BRIEF_DATA;
}

inline bool SmartVar::IsSignedBriefData() const
{
    return (IsBriefData() && ((_raw._type & SmartVarRtti::SV_SIGHED) == SmartVarRtti::SV_SIGHED));
}

inline bool SmartVar::IsUnsignedBriefData() const
{
    return (IsBriefData() && ((_raw._type & SmartVarRtti::SV_UNSIGHED) == SmartVarRtti::SV_UNSIGHED));
}

inline bool SmartVar::IsBool() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_BOOL) == SmartVarRtti::SV_BRIEF_BOOL;
}

inline bool SmartVar::IsByte8() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_BYTE8) == SmartVarRtti::SV_BRIEF_BYTE8;
}

inline bool SmartVar::IsUInt8() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_UINT8) == SmartVarRtti::SV_BRIEF_UINT8;
}

inline bool SmartVar::IsInt16() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_INT16) == SmartVarRtti::SV_BRIEF_INT16;
}

inline bool SmartVar::IsUInt16() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_UINT16) == SmartVarRtti::SV_BRIEF_UINT16;
}

inline bool SmartVar::IsInt32() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_INT32) == SmartVarRtti::SV_BRIEF_INT32;
}

inline bool SmartVar::IsUInt32() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_UINT32) == SmartVarRtti::SV_BRIEF_UINT32;
}

inline bool SmartVar::IsLong() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_LONG) == SmartVarRtti::SV_BRIEF_LONG;
}

inline bool SmartVar::IsULong() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_ULONG) == SmartVarRtti::SV_BRIEF_ULONG;
}

inline bool SmartVar::IsPtr() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_PTR) == SmartVarRtti::SV_BRIEF_PTR;
}

inline bool SmartVar::IsInt64() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_INT64) == SmartVarRtti::SV_BRIEF_INT64;
}

inline bool SmartVar::IsUInt64() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_UINT64) == SmartVarRtti::SV_BRIEF_UINT64;
}

inline bool SmartVar::IsFloat() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_FLOAT) == SmartVarRtti::SV_BRIEF_FLOAT;
}

inline bool SmartVar::IsDouble() const
{
    return (_raw._type & SmartVarRtti::SV_BRIEF_DOUBLE) == SmartVarRtti::SV_BRIEF_DOUBLE;
}

inline bool SmartVar::IsStr() const
{
    return (_raw._type & SmartVarRtti::SV_STRING_DEF) == SmartVarRtti::SV_STRING_DEF;
}

inline bool SmartVar::IsDict() const
{
    return (_raw._type & SmartVarRtti::SV_DICTIONARY_DEF) == SmartVarRtti::SV_DICTIONARY_DEF;
}

inline SmartVar &SmartVar::BecomeNil()
{
    if(IsNil())
        return *this;

    _CleanTypeData(_raw._type);
    _raw._type = SmartVarRtti::SV_NIL;

    return *this;
}

inline SmartVar &SmartVar::BecomeBool()
{
    if(!IsBool())
        *this = AsBool();

    return *this;
}

inline SmartVar &SmartVar::BecomeByte8()
{
    if(!IsByte8())
        *this = AsByte8();

    return *this;
}

inline SmartVar &SmartVar::BecomeUInt8()
{
    if(!IsUInt8())
        *this = AsUInt8();

    return *this;
}

inline SmartVar &SmartVar::BecomeInt16()
{
    if(!IsInt16())
        *this = AsInt16();

    return *this;
}

inline SmartVar &SmartVar::BecomeUInt16()
{
    if(!IsUInt16())
        *this = AsUInt16();

    return *this;
}

inline SmartVar &SmartVar::BecomeInt32()
{
    if(!IsInt32())
        *this = AsInt32();

    return *this;
}

inline SmartVar &SmartVar::BecomeUInt32()
{
    if(!IsUInt32())
        *this = AsUInt32();

    return *this;
}

inline SmartVar &SmartVar::BecomeLong()
{
    if(!IsLong())
        *this = AsLong();

    return *this;
}

inline SmartVar &SmartVar::BecomeULong()
{
    if(!IsULong())
        *this = AsULong();

    return *this;
}

inline SmartVar &SmartVar::BecomePtr()
{
    if(!IsPtr())
        *this = AsPtr<void>();

    return *this;
}

inline SmartVar &SmartVar::BecomeInt64()
{
    if(!IsInt64())
        *this = AsInt64();

    return *this;
}

inline SmartVar &SmartVar::BecomeUInt64()
{
    if(!IsUInt64())
        *this = AsUInt64();

    return *this;
}

inline SmartVar &SmartVar::BecomeFloat()
{
    if(!IsFloat())
        *this = AsFloat();

    return *this;
}

inline SmartVar &SmartVar::BecomeDouble()
{
    if(!IsDouble())
        *this = AsDouble();

    return *this;
}

inline SmartVar &SmartVar::BecomeStr()
{
    if(!IsStr())
        *this = AsStr();

    return *this;
}

inline SmartVar &SmartVar::BecomeDict()
{
    if(!IsDict())
        *this = AsDict();

    return *this;
}


inline Byte8 SmartVar::AsByte8() const
{
    return static_cast<Byte8>(AsInt64());
}

inline U8 SmartVar::AsUInt8() const
{
    return static_cast<U8>(AsUInt64());
}

inline Int16 SmartVar::AsInt16() const
{
    return static_cast<Int16>(AsInt64());
}

inline UInt16 SmartVar::AsUInt16() const
{
    return static_cast<UInt16>(AsUInt64());
}

inline Int32 SmartVar::AsInt32() const
{
    return static_cast<Int32>(AsInt64());
}

inline UInt32 SmartVar::AsUInt32() const
{
    return static_cast<UInt32>(AsUInt64());
}

inline Long SmartVar::AsLong() const
{
    return static_cast<Long>(AsInt64());
}

inline ULong SmartVar::AsULong() const
{
    return static_cast<ULong>(AsUInt64());
}

template <typename _T>
inline _T *SmartVar::AsPtr() const
{
    return reinterpret_cast<_T *>(AsUInt64());
}

inline Float SmartVar::AsFloat() const
{
    return static_cast<Float>(AsDouble());
}

inline SmartVar::operator bool() const
{
    return AsBool();
}

inline SmartVar::operator Byte8 () const
{
    return AsByte8();
}

inline SmartVar::operator U8 () const
{
    return AsUInt8();
}

inline SmartVar::operator Int16 () const
{
    return AsInt16();
}

inline SmartVar::operator UInt16 () const
{
    return AsUInt16();
}

inline SmartVar::operator Int32 () const
{
    return AsInt32();
}

inline SmartVar::operator UInt32 () const
{
    return AsUInt32();
}

inline SmartVar::operator Long() const
{
    return AsLong();
}

inline SmartVar::operator ULong () const
{
    return AsULong();
}

template <typename _T>
inline SmartVar::operator _T * () const
{
    return AsPtr<_T>();
}

inline SmartVar::operator Int64 () const
{
    return AsInt64();
}

inline SmartVar::operator UInt64 () const
{
    return AsUInt64();
}

inline SmartVar::operator Float() const
{
    return AsFloat();
}

inline SmartVar::operator Double() const
{
    return AsDouble();
}

inline SmartVar::operator FS_String () const
{
    return AsStr();
}

inline SmartVar::operator std::string() const
{
    return static_cast<std::string>(AsStr());
}

inline SmartVar::operator const SmartVar::Dict &() const
{
    return AsDict();
}

template <typename _Kty, typename _Ty>
inline std::pair<SmartVar::DictIter, bool> SmartVar::Insert(const _Kty &key, const _Ty &val)
{
    return this->Insert(SmartVar::Dict::key_type(key),
                        SmartVar::Dict::mapped_type(val));
}

template <typename _Kty>
inline SmartVar::DictIter SmartVar::Find(const _Kty &key)
{
    return this->Find(SmartVar::Dict::key_type(key));
}

template <typename _Kty>
inline SmartVar::DictConstIter SmartVar::Find(const _Kty &key) const
{
    return this->Find(SmartVar::Dict::key_type(key));
}

template <typename _Kty>
inline SmartVar::Dict::size_type SmartVar::Erase(const _Kty &key)
{
    return this->Erase(SmartVar::Dict::key_type(key));
}

template <typename _Kty>
inline SmartVar::Dict::mapped_type &SmartVar::operator [](const _Kty &key)
{
    return this->operator [](SmartVar::Dict::key_type(key));
}

template <typename _Kty>
inline const SmartVar::Dict::mapped_type &SmartVar::operator [](const _Kty &key) const
{
    return this->operator [](SmartVar::Dict::key_type(key));
}

template <typename _T>
inline SmartVar &SmartVar::operator =(const _T * const &val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_PTR;

    _raw._briefData._uint64Data = 0;
    ::memcpy(&_raw._briefData._uint64Data, &val, sizeof(_T *));

    return *this;
}

inline void SmartVar::_SetType(int type)
{
    _raw._type = static_cast<SmartVarRtti::RttiType>(type);
}

inline SmartVar::Raw &SmartVar::_GetRaw()
{
    return _raw;
}

inline void SmartVar::_CleanBriefData()
{
    _raw._briefData._uint64Data = 0;
}

inline void SmartVar::_CleanStrData()
{
    Fs_SafeFree(_raw._obj._strData);
}

inline void SmartVar::_CleanDictData()
{
    Fs_SafeFree(_raw._obj._dictData);
}

FS_NAMESPACE_END

#endif
