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

FS_NAMESPACE_BEGIN

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
inline typename RandValType FS_Random<RandValType, RanmdomGenType, DisType>::operator ()()
{
     return  static_cast<RandValType>(_distributor._generator(_randomSource._generator));
}

FS_NAMESPACE_END

#pragma endregion

#endif // __Base_Common_Component_Impl_FS_Random_H__

