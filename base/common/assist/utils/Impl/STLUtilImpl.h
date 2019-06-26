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
 * @file  : STLUtilImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Assist_Utils_Impl_STLUtil_H__
/**
* @file STLUtilImpl.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/08
* @brief
*/

#pragma once

FS_NAMESPACE_BEGIN

template<typename _Type, AssistObjsDefs::DelMethods DelType>
inline void STLUtil::DelMapContainer(_Type &container)
{
    AutoDelHelper<DelType> delObj;
    for(auto iterElement = container.begin(); iterElement != container.end(); ++iterElement)
        delObj.Release(iterElement->second);
}

template<typename _Type, AssistObjsDefs::DelMethods DelType>
inline void STLUtil::DelSetContainer(_Type &container)
{
    AutoDelHelper<DelType> delObj;
    for(auto iterElement = container.begin(); iterElement != container.end(); ++iterElement)
    {
        auto element = *iterElement;
        delObj.Release(element);
    }
}

template<typename _Type, AssistObjsDefs::DelMethods DelType>
inline void STLUtil::DelListContainer(_Type &container)
{
    AutoDelHelper<DelType> delObj;
    for(auto iterElement = container.begin(); iterElement != container.end(); ++iterElement)
    {
        auto element = *iterElement;
        delObj.Release(element);
    }
}

template<typename _Type, AssistObjsDefs::DelMethods DelType>
inline void STLUtil::DelArray(_Type &container)
{
    const Int32 sizeContainer = ARRAY_ELEM_COUNT(container);
    for(Int32 i = 0; i < sizeContainer; ++i)
        Fs_SafeFree(container[i]);
}

FS_NAMESPACE_END

#endif

