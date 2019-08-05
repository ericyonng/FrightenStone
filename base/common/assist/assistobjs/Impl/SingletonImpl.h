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
 * filename: SingletonImpl.h
 *
 * author  : ericyonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
#ifdef __Base_Common_Assist_AssistObjs_Impl_Singleton_H__

FS_NAMESPACE_BEGIN

template<typename T>
Locker Singleton<T>::_locker;

template<typename T>
SmartPtr<T> Singleton<T>::_pObj = NULL;

template<typename T>
inline T *Singleton<T>::GetInstance()
{
    _locker.Lock();
    T *pObj = _pObj;
    if(NULL != pObj)
    {
        _locker.Unlock();
        return pObj;
    }

    _pObj = new T;
    pObj = _pObj;
    _locker.Unlock();

    return pObj;
}

template<typename T>
template<typename... Args>
inline T *Singleton<T>::GetInstance(Args &&... args)
{
    _locker.Lock();
    T *pObj = _pObj;
    if(NULL != pObj)
    {
        _locker.Unlock();
        return pObj;
    }

    _pObj = new T(std::forward<Args>(args)...);
    pObj = _pObj;
    _locker.Unlock();

    return pObj;
}

FS_NAMESPACE_END

#endif // !__Base_Common_Assist_AssistObjs_Impl_Singleton_H__
