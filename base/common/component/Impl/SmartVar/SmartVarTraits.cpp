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
#include "base/common/component/Impl/SmartVar/SmartVarTraits.h"
#include "base/common/component/Impl/SmartVar/SmartVar.h"
#include "base/common/component/Impl/SmartVar/SmartVarArithmetic.h"
#include "base/common/assist/utils/Impl/StringUtil.h"

FS_NAMESPACE_BEGIN

void SmartVarTraits::assign(SmartVar &left, const SmartVar &right)
{
    // TODO
    if(&left == &right)
        return;

    // Execute assignment.
    if(right.IsNil())// Do NIL type data assignment.
    {
        left.BecomeNil();
    }
    else if(right.IsBriefData()) // Do RAW type data assignment.
    {
        // If left type is STR/DICT type, clean first.
        if(left.IsStr())
            left._CleanStrData();
        else if(left.IsDict())
            left._CleanDictData();

        // Assignment.
        left._SetType(right.GetType());
        left._GetRaw()._briefData._uint64Data = right.GetRaw()._briefData._uint64Data;
    }
    else if(right.IsStr()) // Do STR type data assignment.
    {
        // If left type is RAW/DICT type, clean first.
        if(left.IsBriefData())
            left._CleanBriefData();
        else if(left.IsDict())
            left._CleanDictData();

        // Assignment.
        left._SetType(right.GetType());
        auto &lRaw = left._GetRaw();
        const auto &rRaw = right.GetRaw();
        if(rRaw._obj._strData == NULL || rRaw._obj._strData->empty())
        {
            if(lRaw._obj._strData)
                lRaw._obj._strData->Clear();
        }
        else
        {
            if(lRaw._obj._strData == NULL)
                lRaw._obj._strData = new FS_String(*rRaw._obj._strData);
            else if(lRaw._obj._strData != rRaw._obj._strData)
                *lRaw._obj._strData = *rRaw._obj._strData;
        }
    }
    else if(right.IsDict()) // Do DICT type data assignment.
    {
        // If left type is RAW/STR type, clean first.
        if(left.IsBriefData())
            left._CleanBriefData();
        else if(left.IsStr())
            left._CleanStrData();

        left._SetType(right.GetType());
        auto &lRaw = left._GetRaw();
        const auto &rRaw = right.GetRaw();
        if(rRaw._obj._dictData == NULL || rRaw._obj._dictData->empty())
        {
            if(lRaw._obj._dictData)
                lRaw._obj._dictData->clear();
        }
        else
        {
            if(lRaw._obj._dictData == NULL)
                lRaw._obj._dictData = new SmartVar::Dict(*rRaw._obj._dictData);
            else if(lRaw._obj._dictData != rRaw._obj._dictData)
                *lRaw._obj._dictData = *rRaw._obj._dictData;
        }
    }
}

bool SmartVarTraits::eq(const SmartVar &left, const SmartVar &right)
{
    const auto &lRaw = left.GetRaw();
    const auto &rRaw = right.GetRaw();
    if(left.IsStr())
    {
        if(!right.IsStr())
            return false;

        if(lRaw._obj._strData)
        {
            if(rRaw._obj._strData)
                return *lRaw._obj._strData == *rRaw._obj._strData;
            else
                return lRaw._obj._strData->empty();
        }
        else
        {
            if(rRaw._obj._strData)
                return rRaw._obj._strData->empty();
            else
                return true;
        }
    }
    else if(left.IsDict())
    {
        if(!right.IsDict())
            return false;

        const auto *lDict = lRaw._obj._dictData;
        const auto *rDict = rRaw._obj._dictData;
        if(lDict)
        {
            if(rDict)
                return *lDict == *rDict;
            else
                return lDict->empty();
        }
        else
        {
            if(rDict)
                return rDict->empty();
            else
                return true;
        }
    }
    else if(left.IsBriefData())
    {
        if(!right.IsBriefData())
            return false;

        if((left.IsDouble() || left.IsFloat()) ||
            (right.IsDouble() || right.IsFloat()))
        {
            // TODO:浮点数精度问题
            return left.AsDouble() == right.AsDouble();
        }

        return (left.GetRaw()._briefData._uint64Data ==
                right.GetRaw()._briefData._uint64Data);
    }

    return (left.IsNil() && right.IsNil());
}

bool SmartVarTraits::ne(const SmartVar &left, const SmartVar &right)
{
    return !SmartVarTraits::eq(left, right);
}

bool SmartVarTraits::lt(const SmartVar &left, const SmartVar &right)
{
    if(left.IsDict() && right.IsDict())
    {
        const auto *lDict = left.GetRaw()._obj._dictData;
        const auto *rDict = right.GetRaw()._obj._dictData;

        if(lDict == rDict)
            return false;

        if(lDict == NULL || lDict->empty())
            return rDict && !rDict->empty();
        if(rDict == NULL || rDict->empty())
            return false;

        return *lDict < *rDict;
    }
    else if(left.IsStr() && right.IsStr())
    {
        return left.AsStr() < right.AsStr();
    }
    else if(left.IsBriefData() && right.IsBriefData())
    {
        if((left.IsDouble() || left.IsFloat()) ||
            (right.IsDouble() || right.IsFloat()))
            return left.AsDouble() < right.AsDouble();

        return left.AsUInt64() < right.AsUInt64();
    }

    return false;
}

bool SmartVarTraits::gt(const SmartVar &left, const SmartVar &right)
{
    return right < left;
}

bool SmartVarTraits::le(const SmartVar &left, const SmartVar &right)
{
    return !(right < left);
}

bool SmartVarTraits::ge(const SmartVar &left, const SmartVar &right)
{
    return !(left < right);
}

SmartVar SmartVarTraits::add(const SmartVar &left, const SmartVar &right)
{
    SmartVar ret;
    SmartVarTraits::assign(ret, left);
    SmartVarTraits::add_equal(ret, right);
    return ret;
}

SmartVar SmartVarTraits::sub(const SmartVar &left, const SmartVar &right)
{
    SmartVar ret;
    SmartVarTraits::assign(ret, left);
    SmartVarTraits::sub_equal(ret, right);
    return ret;
}

SmartVar SmartVarTraits::mul(const SmartVar &left, const SmartVar &right)
{
    SmartVar ret;
    SmartVarTraits::assign(ret, left);
    SmartVarTraits::mul_equal(ret, right);
    return ret;
}

SmartVar SmartVarTraits::div(const SmartVar &left, const SmartVar &right)
{
    SmartVar ret;
    SmartVarTraits::assign(ret, left);
    SmartVarTraits::div_equal(ret, right);
    return ret;
}

void SmartVarTraits::add_equal(SmartVar &left, const SmartVar &right)
{
    if(left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if(left.IsDict() || right.IsDict())
    {
        if(!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        auto *&lDict = left._GetRaw()._obj._dictData;
        const auto *rDict = right.GetRaw()._obj._dictData;
        if(lDict == NULL)
        {
            if(rDict == NULL || rDict->empty())
                return;

            lDict = new (SmartVar::Dict);
        }

        if(rDict)
            lDict->insert(rDict->begin(), rDict->end());

        return;
    }
    else if(left.IsStr() || right.IsStr())
    {
        left = left.AsStr() + right.AsStr();
        return;
    }

    SmartVarArithmetic::Performs(left, right, SmartVarArithmetic::SV_ARITHMETIC_ADD);
}

void SmartVarTraits::sub_equal(SmartVar &left, const SmartVar &right)
{
    if(left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if(left.IsDict() || right.IsDict())
    {
        if(!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        auto *&lDict = left._GetRaw()._obj._dictData;
        const auto *rDict = right.GetRaw()._obj._dictData;
        if(lDict == NULL)
        {
            if(rDict == NULL || rDict->empty())
                return;

            lDict = new (SmartVar::Dict);
        }

        if(rDict)
        {
            typedef SmartVar::Dict::const_iterator _It;
            for(_It rIt = rDict->begin();
                rIt != rDict->end() && !lDict->empty();
                rIt++)
            {
                lDict->erase(rIt->first);
            }
        }

        return;
    }
    else if(left.IsStr() || right.IsStr())
    {
        left = StringUtil::FilterOutString(left, right);
        return;
    }

    SmartVarArithmetic::Performs(left, right, SmartVarArithmetic::SV_ARITHMETIC_SUB);
}

void SmartVarTraits::mul_equal(SmartVar &left, const SmartVar &right)
{
    if(left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if(left.IsDict() || right.IsDict())
    {
        if(!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        auto *&lDict = left._GetRaw()._obj._dictData;
        const auto *rDict = right.GetRaw()._obj._dictData;
        if((lDict == NULL || lDict->empty()) ||
            (rDict == NULL || rDict->empty()))
        {
            if(lDict)
                lDict->clear();
            return;
        }

        SmartVar cLeft(left);
        const auto *clDict = cLeft.GetRaw()._obj._dictData;
        for(SmartVar::Dict::const_iterator clIt = clDict->begin();
            clIt != clDict->end(); clIt++)
        {
            if(rDict->find(clIt->first) == rDict->end())
                lDict->erase(clIt->first);
        }

        return;
    }
    else if(left.IsStr() || right.IsStr())
    {
        if(left.IsStr() && right.IsBriefData())
        {
            size_t rawRight = static_cast<size_t>(right.AsUInt32());
            SmartVar::Str *&lStr = left._GetRaw()._obj._strData;
            if(lStr && !lStr->empty())
                *lStr *= rawRight;
        }
        else
        {
            left.BecomeNil();
        }
        return;
    }

    SmartVarArithmetic::Performs(left, right, SmartVarArithmetic::SV_ARITHMETIC_MUL);
}

void SmartVarTraits::div_equal(SmartVar &left, const SmartVar &right)
{
    if(left.IsNil() || right.IsNil())
    {
        left.BecomeNil();
        return;
    }
    else if(left.IsDict() || right.IsDict())
    {
        if(!left.IsDict() || !right.IsDict())
        {
            left.BecomeNil();
            return;
        }

        SmartVar::Dict *&lDict = left._GetRaw()._obj._dictData;
        const auto *rDict = right.GetRaw()._obj._dictData;
        if((!lDict || lDict->empty()) ||
            (!rDict || rDict->empty()))
        {
            if(lDict)
                lDict->clear();
            return;
        }

        SmartVar cLeft(left);
        const SmartVar::Dict *clDict = cLeft.GetRaw()._obj._dictData;
        for(SmartVar::Dict::const_iterator clIt = clDict->begin();
            clIt != clDict->end();
            clIt++)
        {
            if(rDict->find(clIt->first) != rDict->end())
                lDict->erase(clIt->first);
        }

        for(SmartVar::Dict::const_iterator rIt = rDict->begin();
            rIt != rDict->end();
            rIt++)
        {
            if(clDict->find(rIt->first) == clDict->end())
                lDict->insert(std::make_pair(rIt->first, rIt->second));
        }

        return;

    }
    else if(left.IsStr() || right.IsStr())
    {
        left.BecomeNil();
        return;
    }

    SmartVarArithmetic::Performs(left, right, SmartVarArithmetic::SV_ARITHMETIC_DIV);
}

FS_NAMESPACE_END
