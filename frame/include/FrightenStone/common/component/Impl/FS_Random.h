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
 * @file  : FS_Random.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/25
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_FS_Random_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_FS_Random_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/DataType/DataType.h"
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
#include <chrono>
#include <random>
// #ifdef _WIN32
// #include <random>
// #else
// #include <tr1/random>
// #endif
#include <limits>
#include <numeric>
#include <atomic>

FS_NAMESPACE_BEGIN

class Locker;

// random的类型定义
#ifdef _WIN32
typedef std::mt19937 FS_Mt19937;

#undef FS_Uniform_Int
#define FS_Uniform_Int(valueType) std::uniform_int<valueType>
#else
typedef std::tr1:mt19937 FS_Mt19937;

// #undef FS_Uniform_Int
// #define FS_Uniform_Int(valueType) std::tr1:uniform_int<valueType>
#endif

class BASE_EXPORT FS_RandomDefs
{
public:

    //随机分布类型
    enum RAND_DIS_TYPE
    {
        RAND_DIS_TYPE_SMALLINT = 0, // 一个整数集上的离散均匀分布
        RAND_DIS_TYPE_INT,          // 一个整数集上的离散均匀分布
        RAND_DIS_TYPE_01,           // [0, 1)上的连续均匀分布
        RAND_DIS_TYPE_REAL,         // [min, max)上的连续均匀分布
        RAND_DIS_TYPE_BERNOULLI,    // 伯努利分布
        RAND_DIS_TYPE_GEOMETRIC,    // 几何分布
        RAND_DIS_TYPE_TRIANGLE,     // 三角分布
        RAND_DIS_TYPE_EXPONENTIAL,  // 指数分布
        RAND_DIS_TYPE_NORMAL,       // 正态分布
        RAND_DIS_TYPE_LOGNORMAL,    // 对数分布
        RAND_DIS_TYPE_ON_SPHERE,    // 球面上的均匀分布
        RAND_DIS_TYPE_BETA,         // 贝塔分布
        RAND_DIS_TYPE_BINOMIAL,     // 二项分布
        RAND_DIS_TYPE_CAUCHY,       // 柯西分布
        RAND_DIS_TYPE_DISCRETE,     // 离散分布
    };

    // 随机数源产生算法 算法速度有高到低，算法质量由低到高
    enum  RAND_GEN_ALGORITHM_TYPE
    {
        RAND_GEN_ALGORITHM_TYPE_RAND48 = 0,             // rand48算法随机数发生器
        RAND_GEN_ALGORITHM_TYPE_MT19937,                // mt19937算法随机数发生器
        RAND_GEN_ALGORITHM_TYPE_MT19937_64,             // mt19937-64算法随机数发生器
        RAND_GEN_ALGORITHM_TYPE_LAGGED_FIBONACCI19937,  // lagged_fibonacci19937算法随机数发生器
    };

#undef RAND_DIS_NUM_SCOPE_MIN
#undef RAND_DIS_NUM_SCOPE_INT64MAX
#undef RAND_DIS_NUM_SCOPE_INT32MAX
#define RAND_DIS_NUM_SCOPE_MIN          0LL
#define RAND_DIS_NUM_SCOPE_INT64MAX     1152921504606846976LL
#define RAND_DIS_NUM_SCOPE_INT32MAX     1073741824

};

// 随机数源
template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE>
struct RandomSource
{

};

#pragma region TypeDefine RandomSource
typedef RandomSource<Int64, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE_MT19937_64> MT1993764RandSrc;
typedef RandomSource<Int64, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE_MT19937> MT19937RandSrc;
#pragma endregion

// 随机数分布器
template<typename ValType, FS_RandomDefs::RAND_DIS_TYPE>
struct Distributor
{

};

template<typename RandValType = Int64, FS_RandomDefs::RAND_DIS_TYPE DisType = FS_RandomDefs::RAND_DIS_TYPE_INT>
class FS_Random
{
public:
    FS_Random();
    FS_Random(RandValType minVal, RandValType maxVal);
    virtual ~FS_Random();

    // 随机数发生 需要指定随机数源，建议选择MT1993764RandSrc，并保证随机数源多线程的同步
    RandValType operator()(MT1993764RandSrc &randomSrc);
    RandValType operator()(MT19937RandSrc &randomSrc);

private:
    Distributor<RandValType, DisType>           _distributor;
};

typedef FS_Random<> FS_Int64Random;

FS_NAMESPACE_END

extern BASE_EXPORT fs::Locker g_RandomLocker;
extern BASE_EXPORT fs::MT1993764RandSrc g_RandomSeed;

#include "FrightenStone/common/component/Impl/FS_RandomImpl.h"

#endif
