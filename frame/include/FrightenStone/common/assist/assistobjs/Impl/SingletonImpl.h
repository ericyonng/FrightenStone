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
#ifdef __Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_Singleton_H__

FS_NAMESPACE_BEGIN

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
Locker Singleton<ObjType, delMethod>::_locker;

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
SmartPtr<ObjType, delMethod> Singleton<ObjType, delMethod>::_pObj = NULL;

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline ObjType *Singleton<ObjType, delMethod>::GetInstance()
{
    _locker.Lock();
    ObjType *pObj = _pObj;
    if(NULL != pObj)
    {
        _locker.Unlock();
        return pObj;
    }

    _pObj = new ObjType;
    pObj = _pObj;
    _locker.Unlock();

    return pObj;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
template<typename... Args>
inline ObjType *Singleton<ObjType, delMethod>::GetInstance(Args... args)
{
    _locker.Lock();
    ObjType *pObj = _pObj;
    if(NULL != pObj)
    {
        _locker.Unlock();
        return pObj;
    }

    _pObj = new ObjType(std::forward<Args>(args)...);
    pObj = _pObj;
    _locker.Unlock();

    return pObj;
}

FS_NAMESPACE_END

#endif // !__Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_Singleton_H__
