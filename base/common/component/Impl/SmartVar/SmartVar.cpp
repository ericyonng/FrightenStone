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

FS_NAMESPACE_BEGIN

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
    if(_type & SmartVarRtti::SV_STRING_DEF)
        Fs_SafeFree(_obj._strData);

    if(_type & SmartVarRtti::SV_DICTIONARY_DEF)
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

void SmartVar::_CleanTypeData(UInt32 type)
{
    typedef SmartVarRtti _RttiType;

    const int topType = (type & _RttiType::SV_TYPEINFO_MASK);
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

