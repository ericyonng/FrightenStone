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
#include "base/common/component/Impl/SmartVar.h"

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

        _rttiTypeRefString.insert(std::make_pair(SV_BRIEF_INT8, "Byte8"));
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
