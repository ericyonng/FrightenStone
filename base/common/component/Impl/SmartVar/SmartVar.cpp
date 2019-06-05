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
 * @file  : SmartVar.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/component/Impl/SmartVar/SmartVarTraits.h"
#include "base/common/component/Impl/SmartVar/SmartVar.h"
#include "base/common/assist/utils/utils.h"

FS_NAMESPACE_BEGIN

std::map<unsigned int, FS_String> SmartVarRtti::_rttiTypeRefString;
const FS_String SmartVarRtti::_nullString;

const FS_String &SmartVarRtti::GetTypeName(unsigned int rttiType)
{
    auto iterStr = _rttiTypeRefString.find(rttiType);
    if(iterStr == _rttiTypeRefString.end())
        return _nullString;

    return iterStr->second;
}

void SmartVarRtti::InitRttiTypeNames()
{
    if(LIKELY(_rttiTypeRefString.empty()))
    {
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_BOOL, "bool"));

        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_BYTE8, "Byte8"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_UINT8, "U8"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_INT16, "Int16"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_UINT16, "UInt16"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_INT32, "Int32"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_UINT32, "UInt32"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_LONG, "Long"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_ULONG, "ULong"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_PTR, "Pointer"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_INT64, "Int64"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_UINT64, "UInt64"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_FLOAT, "Float"));
        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_DOUBLE, "Double"));
        _rttiTypeRefString.insert(std::make_pair(SV_STRING_DEF, "string"));

        _rttiTypeRefString.insert(std::make_pair(SV_DICTIONARY_DEF, "dictionary"));
        _rttiTypeRefString.insert(std::make_pair(SV_NIL, "NULL"));
    }
}

FS_NAMESPACE_END

std::ostream &operator <<(std::ostream &o, const FS_NS SmartVar &smartVar)
{
    FS_NS FS_String str = smartVar.ToString();
    o.write(str.c_str(), str.GetLength());
    return o;
}

FS_NAMESPACE_BEGIN

static void BecomeAndAllocDict(SmartVar &var, SmartVar::Raw &raw)
{
    if(!var.IsDict())
        var.BecomeDict();
    if(!raw._obj._dictData)
        raw._obj._dictData = new (SmartVar::Dict);
}

static const SmartVar::Str __g_nullStr;
static const SmartVar::Str __g_trueStr = "true";
static const SmartVar::Dict __g_nullDict;
static const SmartVar __g_nilVariant;

SmartVar::Raw::Raw()
    :_type(SmartVarRtti::SV_NIL)
{
    memset(&_briefData, 0, sizeof(_briefData));
    memset(&_obj, 0, sizeof(_obj));
}

SmartVar::Raw::~Raw()
{
    if(_type == SmartVarRtti::SV_STRING_DEF)
        Fs_SafeFree(_obj._strData);
    else if(_type == SmartVarRtti::SV_DICTIONARY_DEF)
        Fs_SafeFree(_obj._dictData);
}

SmartVar::SmartVar(const Byte8 *cstrVal)
{
    _raw._type = SmartVarRtti::SV_STRING_DEF;
    if(LIKELY(cstrVal != NULL))
    {
        auto strLen = ::strlen(cstrVal);
        if(LIKELY(strLen != 0))
        {
            _raw._obj._strData = new FS_String(cstrVal);
        }
    }
}

SmartVar::SmartVar(const SmartVar &varVal)
{
    SmartVarTraits::assign(*this, varVal);
}

bool SmartVar::AsBool() const
{
    if(IsNil())
    {
        return false;
    }
    else if(IsDict())
    {
        return false;
    }
    else if(IsStr())
    {
        const Str *str = _raw._obj._strData;
        if(!str || str->empty())
            return false;

        const Str trimedData = str->strip();
        if(trimedData.size() == 4 && trimedData.isalpha())
            return trimedData.tolower() == __g_trueStr;
        else
            return AsUInt64() != 0;
    }

    return _raw._briefData._uint64Data != 0;
}

Int64 SmartVar::AsInt64() const
{
    if(IsNil())
        return 0;
    else if(IsDict())
        return 0;
    else if(IsStr())
        return (_raw._obj._strData && !_raw._obj._strData->empty()) ? StringUtil::StringToInt64(_raw._obj._strData->c_str()) : 0;

    if(IsDouble() || IsFloat())
        return static_cast<Int64>(_raw._briefData._doubleData);

    return _raw._briefData._int64Data;
}

UInt64 SmartVar::AsUInt64() const
{
    if(IsNil())
        return 0;
    else if(IsDict())
        return 0;
    else if(IsStr())
        return (_raw._obj._strData && !_raw._obj._strData->empty()) ? StringUtil::StringToUInt64(_raw._obj._strData->c_str()) : 0;

    if(IsDouble() || IsFloat())
        return static_cast<UInt64>(_raw._briefData._doubleData);

    return _raw._briefData._uint64Data;
}

Double SmartVar::AsDouble() const
{
    if(IsNil())
        return 0.0;
    else if(IsDict())
        return 0.0;
    else if(IsStr())
        return (_raw._obj._strData && !_raw._obj._strData->empty()) ? StringUtil::StringToDouble(_raw._obj._strData->c_str()) : 0;

    if(IsDouble() || IsFloat())
        return _raw._briefData._doubleData;

    if(IsSignedBriefData())
        return static_cast<double>(_raw._briefData._int64Data);

    return static_cast<double>(_raw._briefData._uint64Data);
}

FS_String SmartVar::AsStr() const
{
    if(IsNil())
    {
        return __g_nullStr;
    }
    else if(IsBriefData())
    {
        if(IsBool())
            return _raw._briefData._uint64Data ? "true" : "false";
        else if(IsFloat() || IsDouble())
            return StringUtil::Num2Str(_raw._briefData._doubleData);
        else if(IsSignedBriefData())
            return StringUtil::Num2Str(_raw._briefData._int64Data);
        else
            return StringUtil::Num2Str(_raw._briefData._uint64Data);
    }
    else if(IsStr())
    {
        return _raw._obj._strData ? *_raw._obj._strData : __g_nullStr;
    }
    else
    {
        return __g_nullStr;
    }
}

const SmartVar::Dict &SmartVar::AsDict() const
{
    if(IsDict() && _raw._obj._dictData)
        return *_raw._obj._dictData;

    return __g_nullDict;
}

SmartVar::DictIter SmartVar::Begin()
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->begin();
    else
        return const_cast<Dict &>(__g_nullDict).begin();
}

SmartVar::DictConstIter SmartVar::Begin() const
{
    if(IsDict() &&  _raw._obj._dictData)
        return  _raw._obj._dictData->begin();
    else
        return __g_nullDict.begin();
}

SmartVar::DictIter SmartVar::End()
{
    if(IsDict() &&  _raw._obj._dictData)
        return  _raw._obj._dictData->end();
    else
        return const_cast<Dict &>(__g_nullDict).end();
}

SmartVar::DictConstIter SmartVar::End() const
{
    if(IsDict() && _raw._obj._dictData)
        return  _raw._obj._dictData->end();
    else
        return __g_nullDict.end();
}

SmartVar::DictReverseIter SmartVar::ReverseBegin()
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->rbegin();
    else
        return const_cast<Dict &>(__g_nullDict).rbegin();
}

SmartVar::DictConstReverseIter SmartVar::ReverseBegin() const
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->rbegin();
    else
        return __g_nullDict.rbegin();
}

SmartVar::DictReverseIter SmartVar::ReverseEnd()
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->rend();
    else
        return const_cast<Dict &>(__g_nullDict).rend();
}

SmartVar::DictConstReverseIter SmartVar::ReverseEnd() const
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->rend();
    else
        return __g_nullDict.rend();
}

std::pair<SmartVar::DictIter, bool> SmartVar::Insert(const Dict::key_type &key, const Dict::mapped_type &val)
{
    return Insert(Dict::value_type(key, val));
}

std::pair<SmartVar::DictIter, bool> SmartVar::Insert(const SmartVar::Dict::value_type &val)
{
    BecomeAndAllocDict(*this, _raw);
    return _raw._obj._dictData->insert(val);
}

SmartVar::DictIter SmartVar::Find(const Dict::key_type &key)
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->find(key);
    else
        return const_cast<Dict &>(__g_nullDict).end();
}

SmartVar::DictConstIter SmartVar::Find(const Dict::key_type &key) const
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->find(key);
    else
        return __g_nullDict.end();
}

void SmartVar::Erase(SmartVar::DictIter it)
{
    if(IsDict() && _raw._obj._dictData)
       _raw._obj._dictData->erase(it);
}

SmartVar::Dict::size_type SmartVar::Erase(const SmartVar::Dict::key_type &key)
{
    if(IsDict() && _raw._obj._dictData)
        return _raw._obj._dictData->erase(key);
    else
        return 0;
}

void SmartVar::Erase(SmartVar::DictIter first, SmartVar::DictIter last)
{
    if(IsDict() && _raw._obj._dictData)
        _raw._obj._dictData->erase(first, last);
}

SmartVar::Dict::mapped_type &SmartVar::operator [](const SmartVar &key)
{
    BecomeAndAllocDict(*this, _raw);
    return (*_raw._obj._dictData)[key];
}

const SmartVar::Dict::mapped_type &SmartVar::operator [](const SmartVar &key) const
{
    if(IsDict() && _raw._obj._dictData)
    {
        Dict::const_iterator it = _raw._obj._dictData->find(key);
        if(it == _raw._obj._dictData->end())
            return __g_nilVariant;
        else
            return it->second;
    }

    return __g_nilVariant;
}

SmartVar &SmartVar::operator =(Byte8 val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_BYTE8;
    _raw._briefData._int64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(U8 val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_UINT8;
    _raw._briefData._uint64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(Int16 val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_INT16;
    _raw._briefData._int64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(UInt16 val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_UINT16;
    _raw._briefData._uint64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(Int32 val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_INT32;
    _raw._briefData._int64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(UInt32 val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_UINT32;
    _raw._briefData._uint64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(Long val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_LONG;
    _raw._briefData._int64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(ULong val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_ULONG;
    _raw._briefData._uint64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(const Int64 &val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_INT64;
    _raw._briefData._int64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(const UInt64 &val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_UINT64;
    _raw._briefData._uint64Data = val;

    return *this;
}

SmartVar &SmartVar::operator =(Float val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_FLOAT;
    _raw._briefData._doubleData = val;

    return *this;
}

SmartVar &SmartVar::operator =(const Double &val)
{
    _CleanTypeData(_raw._type);

    _raw._type = SmartVarRtti::SV_BRIEF_DOUBLE;
    _raw._briefData._doubleData = val;

    return *this;
}

SmartVar &SmartVar::operator =(const FS_String &val)
{
    if(!IsStr())
    {
        _CleanTypeData(_raw._type);
        _raw._type = SmartVarRtti::SV_STRING_DEF;
    }

    if(val.empty())
    {
        if(_raw._obj._strData)
            _raw._obj._strData->Clear();
    }
    else
    {
        if(_raw._obj._strData)
            *_raw._obj._strData = val;
        else
            _raw._obj._strData = new FS_String(val);
    }

    return *this;
}

SmartVar &SmartVar::operator =(const SmartVar::Dict &val)
{
    if(!IsDict())
    {
        _CleanTypeData(_raw._type);
        _raw._type = SmartVarRtti::SV_DICTIONARY_DEF;
    }

    if(val.empty())
    {
        if(_raw._obj._dictData)
            _raw._obj._dictData->clear();
    }
    else
    {
        if(_raw._obj._dictData)
            *_raw._obj._dictData = val;
        else
            _raw._obj._dictData = new Dict(val);
    }

    return *this;
}

SmartVar &SmartVar::operator =(const SmartVar &val)
{
    SmartVarTraits::assign(*this, val);
    return *this;
}

bool SmartVar::operator ==(const SmartVar &another) const
{
    return SmartVarTraits::eq(*this, another);
}

bool SmartVar::operator !=(const SmartVar &another) const
{
    return SmartVarTraits::ne(*this, another);
}

bool SmartVar::operator <(const SmartVar &another) const
{
    return SmartVarTraits::lt(*this, another);
}

bool SmartVar::operator >(const SmartVar &another) const
{
    return SmartVarTraits::gt(*this, another);
}

bool SmartVar::operator <=(const SmartVar &another) const
{
    return SmartVarTraits::le(*this, another);
}

bool SmartVar::operator >=(const SmartVar &another) const
{
    return SmartVarTraits::ge(*this, another);
}

SmartVar SmartVar::operator +(const SmartVar &another) const
{
    return SmartVarTraits::add(*this, another);
}

SmartVar SmartVar::operator -(const SmartVar &another) const
{
    return SmartVarTraits::sub(*this, another);
}

SmartVar SmartVar::operator *(const SmartVar &another) const
{
    return SmartVarTraits::mul(*this, another);
}

SmartVar SmartVar::operator /(const SmartVar &another) const
{
    return SmartVarTraits::div(*this, another);
}

SmartVar &SmartVar::operator +=(const SmartVar &another)
{
    SmartVarTraits::add_equal(*this, another);
    return *this;
}

SmartVar &SmartVar::operator -=(const SmartVar &another)
{
    SmartVarTraits::sub_equal(*this, another);
    return *this;
}

SmartVar &SmartVar::operator *=(const SmartVar &another)
{
    SmartVarTraits::mul_equal(*this, another);
    return *this;
}

SmartVar &SmartVar::operator /=(const SmartVar &another)
{
    SmartVarTraits::div_equal(*this, another);
    return *this;
}

const FS_String &SmartVar::TypeToString() const
{
    return SmartVarRtti::GetTypeName(_raw._type);
}

FS_String SmartVar::ValueToString() const
{
    if(IsStr())
    {
        return _raw._obj._strData ? *_raw._obj._strData : __g_nullStr;
    }
    else if(IsDict())
    {

        FS_String content;
        content << "{";

        if(_raw._obj._dictData)
        {
            for(DictConstIter it = _raw._obj._dictData->begin();
                it != _raw._obj._dictData->end();
                )
            {
                content << it->first.ValueToString() << ":" << it->second.ValueToString();

                if(++it != _raw._obj._dictData->end())
                    content << "|";
            }
        }

        content << "}";
        return content;
    }
    else if(IsNil())
    {
        return "nil";
    }

    // RAW type var data.
    switch(_raw._type)
    {
        case SmartVarRtti::SV_BRIEF_BOOL:
            return _raw._briefData._int64Data != 0 ? "true" : "false";

        case SmartVarRtti::SV_BRIEF_BYTE8:
        case SmartVarRtti::SV_BRIEF_INT16:
        case SmartVarRtti::SV_BRIEF_INT32:
        case SmartVarRtti::SV_BRIEF_LONG:
        case SmartVarRtti::SV_BRIEF_INT64:
            return StringUtil::Num2Str(_raw._briefData._int64Data);

        case SmartVarRtti::SV_BRIEF_UINT8:
        case SmartVarRtti::SV_BRIEF_UINT16:
        case SmartVarRtti::SV_BRIEF_UINT32:
        case SmartVarRtti::SV_BRIEF_ULONG:
        case SmartVarRtti::SV_BRIEF_UINT64:
            return StringUtil::Num2Str(_raw._briefData._uint64Data);

        case SmartVarRtti::SV_BRIEF_PTR:
            return FS_String().Format("0x%p", _raw._briefData._uint64Data);

        case SmartVarRtti::SV_BRIEF_FLOAT:
        case SmartVarRtti::SV_BRIEF_DOUBLE:
            return StringUtil::Num2Str( _raw._briefData._doubleData);

        default:
            break;
    }

    return "";
}

FS_String SmartVar::ToString() const
{
    FS_String desc = "type:";
    desc += TypeToString();
    desc += ", value:";
    desc += ValueToString();
    return desc;
}

void SmartVar::_OptimizePerformance()
{
    if(IsStr())
    {
        if(_raw._obj._strData && _raw._obj._strData->empty())
            Fs_SafeFree(_raw._obj._strData);
    }
    else if(IsDict())
    {
        if(_raw._obj._dictData && _raw._obj._dictData->empty())
            Fs_SafeFree(_raw._obj._dictData);
    }
}

void SmartVar::_CleanTypeData(UInt32 type)
{
    typedef SmartVarRtti _RttiType;

    const auto topType = (type & _RttiType::SV_TYPEINFO_MASK);
    switch(topType)
    {
        case _RttiType::SV_BRIEF_DATA:
            _CleanBriefData();
            break;
        case _RttiType::SV_STRING:
            _CleanStrData();
            break;
        case _RttiType::SV_DICTIONARY:
            _CleanDictData();
            break;
        default:
            break;
    }
}

FS_NAMESPACE_END

