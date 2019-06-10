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
 * filename: AssistObjsDefs.h
 *
 * author  : ericyonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
#ifndef __Base_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__
#define __Base_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__
/**
* @file AssistObjsDefs.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"

#ifdef FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL

// inner use

#endif

class BASE_EXPORT AssistObjsDefs
{
public:
    enum DelMethods
    {
        Delete = 0,
        MultiDelete,
        SelfRelease,
        NoDel,
    };
};

FS_NAMESPACE_BEGIN

// auto free obj
template<typename Type, AssistObjsDefs::DelMethods del>
class AutoDelObj
{
public:
    void Release();
    void Release(Type *&p);
};

template<AssistObjsDefs::DelMethods del>
class AutoDelHelper
{
public:
    template<typename _Type>
    void Release(_Type *&val);
};

FS_NAMESPACE_END

#include "base/common/assist/assistobjs/Defs/AssistObjsDefsImpl.h"
#endif // !__Base_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__