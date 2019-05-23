#ifndef __Base_Common_BaseDefs_Structure_Defs_BinaryArray_H__
#define __Base_Common_BaseDefs_Structure_Defs_BinaryArray_H__
/**
 * @file    BinaryArray.h
 * @author  Huiya Song
 * @date    2019/03/24
 * @brief   随机访问型二分数组
            键查询效率o(1),值查询效率o(logn)
            值插入效率逼近o(logn)（由于vector的数据拷贝问题会影响性能，若有所顾忌请自行考虑是否使用）
            适用于中小数据规模
 */

#pragma once

#include <base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"
#include <vector>
#include<map>

FS_NAMESPACE_BEGIN

// 比较方法
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
        End = -1,           // 无效pos
    };
};

// 随机访问型二分数组
template<typename Value, typename less = BALess<Value>>
class BinaryArray
{
#pragma region 迭代器
public:
    typedef typename std::vector<Value>::iterator ArrayIterator;
    typedef typename std::vector<Value>::const_iterator ConstArrayIterator;

#pragma endregion 
    #pragma region 增删查排序通用接口
public:
    BinaryArray();

    int Add(Value v);                                                     // 相同的value加入将失败
    void Erase(Value v);                                                        // v节点（包括v）之后节点key全部失效
    void EraseByKey(int k);
    void Clear();
    int Find(Value v);
    void GetRange(int l, int r, std::map<int, Value> &kvs);

    typename BinaryArray<Value, less>::ArrayIterator begin();
    typename BinaryArray<Value, less>::ConstArrayIterator begin() const;
    typename BinaryArray<Value, less>::ArrayIterator end();
    typename BinaryArray<Value, less>::ConstArrayIterator end() const;

    void Sort();                                                                // 消耗性能不建议用，建议先删除元素后Add
    size_t Size();
    const Value *Data();
    #pragma endregion

    #pragma region 下标随机访问
public:
    const Value &operator[](int k) const;
    Value &operator[](int k);
    bool operator <(const BinaryArray<Value, less> &other) const;
    #pragma endregion

    #pragma region 比较大小
private:
    bool _IsLess(const Value &l, const Value &r) const;
    bool _IsEqual(const Value &l, const Value &r) const;
    bool _IsBigger(const Value &l, const Value &r) const;
    #pragma endregion

private:
    std::vector<Value> _elements;    // value 默认按照pos升序
    less _less;
    int _left;
    int _mid;
    int _right;
    int _pos;
};

FS_NAMESPACE_END
#include "base/common/basedefs/Structure/Defs/BinaryArrayImpl.h"

#endif //!__Base_Common_BaseDefs_Structure_Defs_BinaryArray_H__
