#ifndef __Base_Common_BaseDefs_Structure_SquareBinaryArray_H__
#define __Base_Common_BaseDefs_Structure_SquareBinaryArray_H__

#pragma once

/**
 * @file    SquareBinaryArray.h
 * @author  Huiya Song
 * @date    2019/03/24
 * @brief   随机访问型二分数组,
            需求解决BinaryArray在大数据量逆序插入效率不高的问题
            方案：通过构建二维表，将一次的内存拷贝分解到x,y方向，大幅降低内存拷贝对性能的影响（耗时接近减半）
 */

#pragma once

// #include "BinaryArray.h"
// 
// FS_NAMESPACE_BEGIN
// 
// template<typename Value>
// class BASE_EXPORT SBALess
// {
// public:
//     inline bool operator()(const BinaryArray<Value> *l, const BinaryArray<Value> *r) const
//     {
//         if(l == NULL || r == NULL)
//             return  l < r;
// 
//         if(l == r)
//             return l < r;
// 
//         return *l < *r;
//     }
// };
// 
// template<typename Value>
// class BinaryArray<BinaryArray<Value> *>;
// 
// // template<typename Value>
// // BASE_EXPORT using SquareBinaryArray = BASE_EXPORT BinaryArray<BinaryArray<Value> *>;
// 
// FS_NAMESPACE_END

#endif // !__Base_Common_BaseDefs_Structure_SquareBinaryArray_H__
