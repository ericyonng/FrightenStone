#ifdef __Base_Common_Assist_AssistObjs_Impl_Singleton_H__

FS_NAMESPACE_BEGIN

template<typename T>
Locker Singleton<T>::_locker;

template<typename T>
std::auto_ptr<T> Singleton<T>::_pObj{NULL};

template<typename T>
inline T *Singleton<T>::GetInstance()
{
    // 判断指针是否存在存在则返回不存在则创建
    _locker.Lock();
    T *pObj = _pObj.get();
    if(NULL != pObj)
    {
        _locker.Unlock();
        return pObj;
    }

    _pObj = std::auto_ptr<T>(new T);
    pObj = _pObj.get();
    _locker.Unlock();

    return pObj;
}
FS_NAMESPACE_END

#endif // !__Base_Common_Assist_AssistObjs_Impl_Singleton_H__
