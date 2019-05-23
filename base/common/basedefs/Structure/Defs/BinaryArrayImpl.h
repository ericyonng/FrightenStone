#ifdef __Base_Common_BaseDefs_Structure_Defs_BinaryArray_H__
/**
 * @file    BinaryArrayImpl.h
 * @author  Huiya Song
 * @date    2019/03/24
 * @brief   随机访问型二分数组
 */

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

#pragma region 增删查排序通用接口
template<typename Value, typename less>
int BinaryArray<Value, less>::Add(Value v)
{
    // 1.判断是否空序列
    if(_elements.empty())
    {
        _elements.push_back(v);
        return 0;
    }

    // 2.初始化 pos 值
    const auto sz = static_cast<int>(_elements.size());
    _left = 0;
    _right = sz - 1;

    // 3.比较左端点
    if(_IsLess(v, _elements[_left]))
    {
        _elements.insert(_elements.begin() + _left, v);
        return _left;
    }

    // 4.比较右端点
    if(_IsBigger(v, _elements[_right]))
    {
        _elements.insert(_elements.begin() + _right + 1, v);
        return _right + 1;
    }

    // 中间值位置
    _mid = (_left + _right) / 2;

    // 3.寻找合适位置添加新值
    while(true)
    {
        // a.在left mid之间?
        if(_IsLess(v, _elements[_mid]))
        {
            _right = _mid;
        }
        else
        {
            _left = _mid;
        }

        // b.更新中值
        _mid = (_left + _right) / 2;

        // c.终止条件
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

/* 采用二分查找法 */
template<typename Value, typename less>
int BinaryArray<Value, less>::Find(Value v)
{
    // 1.判断是否空序列
    if(_elements.empty())
        return BinaryArrayDefs::End;

    // 2.初始化 pos 值
    const int sz = static_cast<int>(_elements.size());
    _left = 0;
    _right = sz - 1;

    // 3.判断是否在范围内
    if(_IsLess(v, _elements[_left]) || _IsBigger(v, _elements[_right]))
        return BinaryArrayDefs::End;

    // 4.终止条件：mid==left 或者 mid == right
    _mid = (_left + _right) / 2;
    while(true)
    {
        // d.更新端点
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

// 消耗性能不建议用，建议先删除元素后Add
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

#pragma region 下标随机访问
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

    // 队列比较需要大的一方的最小元素大于小的一方的最大元素
    return _IsLess(_elements.back(), other._elements.front());
}
#pragma endregion

#pragma region 比较大小
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


#endif //!__Base_Common_BaseDefs_Structure_BinaryArray_H__
