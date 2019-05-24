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
 * @file  : BinaryArray.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_BaseDefs_Structure_Defs_BinaryArray_H__
#define __Base_Common_BaseDefs_Structure_Defs_BinaryArray_H__


#pragma once

#include <base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"
#include <vector>
#include<map>

FS_NAMESPACE_BEGIN

#pragma region BALess
template<typename Value>
class BALess
{
public:
    bool operator()(const Value l, const Value r) const;
};
#pragma endregion

class BASE_EXPORT BinaryArrayDefs
{
public:
    enum :int
    {
        End = -1,
    };
};

template<typename Value, typename less = BALess<Value>>
class BinaryArray
{
#pragma region 
public:
    typedef typename std::vector<Value>::iterator ArrayIterator;
    typedef typename std::vector<Value>::const_iterator ConstArrayIterator;

#pragma endregion 
    #pragma region 
public:
    BinaryArray();

    int Add(Value v);
    void Erase(Value v);
    void EraseByKey(int k);
    void Clear();
    int Find(Value v);
    void GetRange(int l, int r, std::map<int, Value> &kvs);

    typename BinaryArray<Value, less>::ArrayIterator begin();
    typename BinaryArray<Value, less>::ConstArrayIterator begin() const;
    typename BinaryArray<Value, less>::ArrayIterator end();
    typename BinaryArray<Value, less>::ConstArrayIterator end() const;

    void Sort();
    size_t Size();
    const Value *Data();
    #pragma endregion

    #pragma region index
public:
    const Value &operator[](int k) const;
    Value &operator[](int k);
    bool operator <(const BinaryArray<Value, less> &other) const;
    #pragma endregion

    #pragma region compare
private:
    bool _IsLess(const Value &l, const Value &r) const;
    bool _IsEqual(const Value &l, const Value &r) const;
    bool _IsBigger(const Value &l, const Value &r) const;
    #pragma endregion

private:
    std::vector<Value> _elements;
    less _less;
    int _left;
    int _mid;
    int _right;
    int _pos;
};

FS_NAMESPACE_END
#include "base/common/basedefs/Structure/Defs/BinaryArrayImpl.h"

#endif //!__Base_Common_BaseDefs_Structure_Defs_BinaryArray_H__
