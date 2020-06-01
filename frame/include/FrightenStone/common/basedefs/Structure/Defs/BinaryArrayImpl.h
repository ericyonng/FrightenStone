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
 * @file  : BinaryArrayImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_BaseDefs_Structure_Defs_BinaryArray_H__


#pragma once

#include<algorithm>

FS_NAMESPACE_BEGIN

#pragma region BTLess
template<typename Value>
inline bool BALess<Value>::operator()(const Value l, const Value r) const
{
    return l < r;
}
#pragma endregion

#pragma region BinaryArray
template<typename Value, typename less>
BinaryArray<Value, less>::BinaryArray()
    : _left(0)
    , _mid(0)
    , _right(0)
    , _pos(0)
{
    
}

#pragma region add/recuse
template<typename Value, typename less>
int BinaryArray<Value, less>::Add(Value v)
{
    if(_elements.empty())
    {
        _elements.push_back(v);
        return 0;
    }

    const auto sz = static_cast<int>(_elements.size());
    _left = 0;
    _right = sz - 1;

    if(_IsLess(v, _elements[_left]))
    {
        _elements.insert(_elements.begin() + _left, v);
        return _left;
    }

    if(_IsBigger(v, _elements[_right]))
    {
        _elements.insert(_elements.begin() + _right + 1, v);
        return _right + 1;
    }

    _mid = (_left + _right) / 2;

    while(true)
    {
        if(_IsLess(v, _elements[_mid]))
        {
            _right = _mid;
        }
        else
        {
            _left = _mid;
        }

        _mid = (_left + _right) / 2;

        if(_mid == _left)
        {
            if(_IsEqual(v, _elements[_left]))
            {
                throw std::logic_error("value is already existed.");
                return _left;
            }

            if(_IsEqual(v, _elements[_right]))
            {
                throw std::logic_error("value is already existed.");
                return _right;
            }

            _elements.insert(_elements.begin() + _left + 1, v);
            return _left + 1;
        }
    }

    return BinaryArrayDefs::End;
}

template<typename Value, typename less>
inline void BinaryArray<Value, less>::Erase(Value v)
{
    _pos = Find(v);
    if(_pos != BinaryArrayDefs::End)
        _elements.erase(_elements.begin() + _pos);
}
    
template<typename Value, typename less>
inline void BinaryArray<Value, less>::EraseByKey(int k)
{
    if(_elements.empty())
        return;

    if(k >= static_cast<int>(_elements.size()))
        return;

    _elements.erase(_elements.begin() + k);
}

template<typename Value, typename less>
inline void BinaryArray<Value, less>::Clear()
{
    _elements.clear();
}

template<typename Value, typename less>
int BinaryArray<Value, less>::Find(Value v)
{
    if(_elements.empty())
        return BinaryArrayDefs::End;

    const int sz = static_cast<int>(_elements.size());
    _left = 0;
    _right = sz - 1;

    if(_IsLess(v, _elements[_left]) || _IsBigger(v, _elements[_right]))
        return BinaryArrayDefs::End;

    _mid = (_left + _right) / 2;
    while(true)
    {
        if(_IsLess(v, _elements[_mid]))
            _right = _mid;
        else
            _left = _mid;

        _mid = (_left + _right) / 2;

        if(_mid == _left)
        {
            if(_IsEqual(v, _elements[_left]))
                return _left;

            if(_IsEqual(v, _elements[_right]))
                return _right;

            break;
        }
    }

    return BinaryArrayDefs::End;
}
    
template<typename Value, typename less>
void BinaryArray<Value, less>::GetRange(int l, int r, std::map<int, Value> &kvs)
{
    if(_elements.empty())
        return;

    if(l < 0)
        return;

    const int sz = static_cast<int>(_elements.size());
    if(l >= sz)
        return;

    r = std::min<int>(r, static_cast<int>(sz - 1));
    for(int i = l; i <= r; ++i)
        kvs.insert(std::make_pair<int, Value>(i, _elements[i]));
}

template<typename Value, typename less>
inline typename BinaryArray<Value, less>::ArrayIterator BinaryArray<Value, less>::begin()
{
    return _elements.begin();
}

template<typename Value, typename less>
inline typename BinaryArray<Value, less>::ConstArrayIterator BinaryArray<Value, less>::begin() const
{
    return _elements.begin();
}

template<typename Value, typename less>
inline typename BinaryArray<Value, less>::ArrayIterator BinaryArray<Value, less>::end()
{
    return _elements.end();
}

template<typename Value, typename less>
inline typename BinaryArray<Value, less>::ConstArrayIterator BinaryArray<Value, less>::end() const
{
    return _elements.end();
}

template<typename Value, typename less>
inline void BinaryArray<Value, less>::Sort()
{
    std::sort(_elements.begin(), _elements.end(), _less);
}

template<typename Value, typename less>
inline size_t BinaryArray<Value, less>::Size()
{
    return _elements.size();
}

template<typename Value, typename less>
inline const Value * BinaryArray<Value, less>::Data()
{
    return _elements.data();
}
#pragma endregion

#pragma region 
template<typename Value, typename less>
inline const Value &BinaryArray<Value, less>::operator[](int k) const
{
    return _elements[k];
}

template<typename Value, typename less>
inline Value &BinaryArray<Value, less>::operator[](int k)
{
    return _elements[k];
}


template<typename Value, typename less /*= BALess<Value>*/>
bool BinaryArray<Value, less>::operator<(const BinaryArray<Value, less> &other) const
{
    if(UNLIKELY(_elements.empty() && other._elements.empty()))
        return this < &other;

    if(UNLIKELY(_elements.empty()))
        return true;

    if(UNLIKELY(other._elements.empty()))
        return false;

    return _IsLess(_elements.back(), other._elements.front());
}
#pragma endregion

#pragma region 
template<typename Value, typename less>
inline bool BinaryArray<Value, less>::_IsLess(const Value &l, const Value &r) const
{
    return _less(l, r);
}

template<typename Value, typename less>
inline bool BinaryArray<Value, less>::_IsEqual(const Value &l, const Value &r) const 
{
    return !_less(l, r) && !_less(r, l);
}

template<typename Value, typename less>
inline bool BinaryArray<Value, less>::_IsBigger(const Value &l, const Value &r) const
{
    return _less(r, l);
}
#pragma endregion

#pragma endregion

FS_NAMESPACE_END


#endif //!__Frame_Include_FrightenStone_Common_BaseDefs_Structure_BinaryArray_H__
