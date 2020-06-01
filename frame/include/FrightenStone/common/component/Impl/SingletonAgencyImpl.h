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
 * @file  : SingletonAgencyImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/08/10
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Component_Impl_SingletonAgency_H__
#pragma once

FS_NAMESPACE_BEGIN

template<typename ObjType>
inline SingletonAgency<ObjType>::SingletonAgency()
{
}

template<typename ObjType>
inline SingletonAgency<ObjType>::~SingletonAgency()
{
}

template<typename ObjType>
inline ObjType *SingletonAgency<ObjType>::operator->()
{
    static ObjType *obj = Singleton<ObjType>::GetInstance();
    return obj;
}

template<typename ObjType>
inline const ObjType *SingletonAgency<ObjType>::operator->() const
{
    static ObjType *obj = Singleton<ObjType>::GetInstance();
    return obj;
}

FS_NAMESPACE_END

#endif
