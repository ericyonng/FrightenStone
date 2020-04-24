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
 * filename: AssistObjsDefsImpl.h
 *
 * author  : ericyonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__
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
class AutoDelObj<Type, AssistObjsDefs::Delete>
{
public:
    inline void Release(Type *&p)
    {
        FsSafeDelete(p);
    }
};

template<typename Type>
class AutoDelObj<Type, AssistObjsDefs::MultiDelete>
{
public:
    inline void Release(Type *&p)
    {
        Fs_SafeMultiDelete(p);
    }
};

template<typename Type>
class AutoDelObj<Type, AssistObjsDefs::SelfRelease>
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
class AutoDelObj<Type, AssistObjsDefs::NoDel>
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
        Fs_SafeMultiDelete(val);
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
