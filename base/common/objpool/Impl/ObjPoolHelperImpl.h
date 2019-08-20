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
 * @file  : ObjPoolHelperImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/22
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_ObjPool_Impl_ObjPoolHelper_H__
#pragma once

FS_NAMESPACE_BEGIN

template<typename ObjType>
inline ObjPoolHelper<ObjType>::ObjPoolHelper(size_t objAmount)
    :_alloctor(new IObjAlloctor<ObjType>(objAmount))
{
    // ´´½¨Î¯ÍÐ
   ObjPoolMethods::RegisterToMemleakMonitor(typeid(ObjType).name()
                                            , DelegatePlusFactory::Create(this, &ObjPoolHelper<ObjType>::PrintObjPool));
//     ObjPoolMethods::RegisterToMemleakMonitor(typeid(ObjType).name()
//                                              , DelegatePlusFactory::Create<size_t, size_t &>(obj, &fs::ObjPoolHelper<ObjType>::PrintMemleak));
}

template<typename ObjType>
inline ObjPoolHelper<ObjType>::~ObjPoolHelper()
{
    ObjPoolMethods::UnRegisterMemleakDelegate(typeid(ObjType).name());

#if __FS_THREAD_SAFE__
    _locker.Lock();
#endif

    Fs_SafeFree(_alloctor);

#if __FS_THREAD_SAFE__
    _locker.Unlock();
#endif
}

template<typename ObjType>
inline size_t ObjPoolHelper<ObjType>::GetMemleakObjNum() const
{
    return _alloctor->GetObjInUse();
}

template<typename ObjType>
inline const char *ObjPoolHelper<ObjType>::GetObjName() const
{
    return typeid(ObjType).name();
}

template<typename ObjType>
inline size_t ObjPoolHelper<ObjType>::GetMemleakBytes() const
{
    return _alloctor->GetObjInUse()*IObjAlloctor<ObjType>::_objBlockSize;
}

template<typename ObjType>
inline size_t ObjPoolHelper<ObjType>::GetPoolBytesOccupied() const
{
    return _alloctor->GetBytesOccupied();
}

template<typename ObjType>
inline size_t ObjPoolHelper<ObjType>::PrintObjPool(Int64 &poolOccupiedBytes)
{
    auto memleakBytes = _alloctor->GetObjInUse()*IObjAlloctor<ObjType>::_objBlockSize;
    poolOccupiedBytes = _alloctor->GetBytesOccupied();
    ObjPoolMethods::PrintObjPoolInfo(typeid(ObjType).name()
                                     , _alloctor->GetNodeCnt()
                                     , _alloctor->GetTotalObjBlocks()
                                     , _alloctor->GetBytesOccupied()
                                     , _alloctor->GetObjInUse()
                                     , memleakBytes);

    return memleakBytes;
}

template<typename ObjType>
inline IObjAlloctor<ObjType> *ObjPoolHelper<ObjType>::operator->()
{
    return _alloctor;
}

template<typename ObjType>
inline const IObjAlloctor<ObjType> *ObjPoolHelper<ObjType>::operator->() const
{
    return _alloctor;
}

FS_NAMESPACE_END

#endif
