#ifdef __Base_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__
/**
* @file AssistObjsDefsImpl.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

FS_NAMESPACE_BEGIN

template<typename Type, AssistObjsDefs::DelMethods delMethod>
inline void AutoDelObj<Type, delMethod>::Release()
{

}

template<typename Type>
class AutoDelObj<typename Type, AssistObjsDefs::Delete>
{
public:
    inline void Release(Type *&p)
    {
        FsSafeDelete(p);
    }
};

template<typename Type>
class AutoDelObj<typename Type, AssistObjsDefs::MultiDelete>
{
public:
    inline void Release(typename Type *&p)
    {
        FsSafeMultiDelete(p);
    }
};

template<typename Type>
class AutoDelObj<typename Type, AssistObjsDefs::SelfRelease>
{
public:
    inline void Release(Type *&p)
    {
        if(p)
            p->Release();

        p = NULL;
    }
};

template<typename Type>
class AutoDelObj<typename Type, AssistObjsDefs::NoDel>
{
public:
    inline void Release(Type *&p)
    {
        p = NULL;
    }
};

template<>
class AutoDelHelper<AssistObjsDefs::Delete>
{
public:
    template<typename _Type>
    inline void Release(_Type *&val)
    {
        FsSafeDelete(val);
    }
};


template<>
class AutoDelHelper<AssistObjsDefs::MultiDelete>
{
public:
    template<typename _Type>
    inline void Release(_Type *&val)
    {
        FsSafeMultiDelete(val);
    }
};


template<>
class AutoDelHelper<AssistObjsDefs::SelfRelease>
{
public:
    template<typename _Type>
    inline void Release(_Type *&val)
    {
        if(val)
            val->Release();

        val = NULL;
    }
};


template<>
class AutoDelHelper<AssistObjsDefs::NoDel>
{
public:
    template<typename _Type>
    inline void Release(_Type *&val)
    {
        val = NULL;
    }
};

FS_NAMESPACE_END

#endif
