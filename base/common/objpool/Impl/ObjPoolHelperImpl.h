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
    : _objAmount(objAmount)
    ,_alloctor(new IObjAlloctor<ObjType>(objAmount))
{
}

template<typename ObjType>
inline ObjPoolHelper<ObjType>::~ObjPoolHelper()
{
#if __FS_THREAD_SAFE__
    _Lock();
#endif

    Fs_SafeFree(_alloctor);

#if __FS_THREAD_SAFE__
    _Unlock();
#endif
}

template<typename ObjType>
inline void *ObjPoolHelper<ObjType>::Alloc()
{
#if __FS_THREAD_SAFE__
    _Lock();
#endif

    auto ptr = _alloctor->Alloc();

#if __FS_THREAD_SAFE__
    _Unlock();
#endif

    return ptr;
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::Free(void *ptr)
{
#if __FS_THREAD_SAFE__
     _Lock();
#endif

     _alloctor->Free(ptr);

#if __FS_THREAD_SAFE__
    _Unlock();
#endif
}

template<typename ObjType>
inline size_t ObjPoolHelper<ObjType>::GetMemleakObjNum() const
{
#if __FS_THREAD_SAFE__
    _Lock();
#endif

    auto cnt = _alloctor->GetObjInUse();

#if __FS_THREAD_SAFE__
    _Unlock();
#endif

    return cnt;
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_Lock()
{
    _locker.Lock();
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_Unlock()
{
    _locker.Lock();
}

FS_NAMESPACE_END

#endif
