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
 * @file  : FS_RandomImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/25
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_FS_Random_H__
#pragma once

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
inline FS_Random<RandValType, RanmdomGenType, DisType>::FS_Random()
{
}

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
inline FS_Random<RandValType, RanmdomGenType, DisType>::FS_Random(RandValType minVal, RandValType maxVal, RandValType srandVal)
    :_distributor(minVal, maxVal)
    ,_randomSource(srandVal)
{
}

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
inline FS_Random<RandValType, RanmdomGenType, DisType>::~FS_Random()
{
}

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
inline FS_Random<RandValType, RanmdomGenType, DisType>::operator typename RandValType()() const
{
     return static_cast<RandValType>(_distributor._generator(_randomSource._generator));
}

#pragma region 随机数分布器/随机数发生源

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_SMALLINT>
{
    std::tr1::uniform_int<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_INT>
{
    std::tr1::uniform_int_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_REAL>
{
    std::tr1::uniform_real_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_BERNOULLI>
{
    std::tr1::bernoulli_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_GEOMETRIC>
{
    std::tr1::geometric_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_EXPONENTIAL>
{
    std::tr1::exponential_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_NORMAL>
{
    std::tr1::normal_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_LOGNORMAL>
{
    std::tr1::lognormal_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_BINOMIAL>
{
    std::tr1::binomial_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_CAUCHY>
{
    std::tr1::cauchy_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct Distributor<RandValType, RAND_DIS_TYPE_DISCRETE>
{
    std::tr1::discrete_distribution<RandValType> _generator;
    Distributor(const RandValType minVal = RAND_DIS_NUM_SCOPE_MIN, const RandValType maxVal = ((std::numeric_limits<RandValType>::max)()&MAX_I64_NUM))
        :_generator(minVal, maxVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct RandomSource<RandValType, RAND_GEN_ALGORITHM_TYPE_MT19937>
{
    std::tr1::mt19937<RandValType> _generator;
    RandomSource(const RandValType srandVal = static_cast<RandValType>(std::chrono::system_clock().now().time_since_epoch().count()))
        :_generator(srandVal)
    {

    }
};

template<typename RandValType, FS_RandomDefs::RAND_GEN_ALGORITHM_TYPE RanmdomGenType, FS_RandomDefs::RAND_DIS_TYPE DisType>
FS_Random<RandValType, RanmdomGenType, DisType>::struct RandomSource<RandValType, RAND_GEN_ALGORITHM_TYPE_MT19937_64>
{
    std::tr1::mt19937_64<RandValType> _generator;
    RandomSource(const RandValType srandVal = static_cast<RandValType>(std::chrono::system_clock().now().time_since_epoch().count()))
        :_generator(srandVal)
    {

    }
};

#pragma endregion

#endif // __Base_Common_Component_Impl_FS_Random_H__

