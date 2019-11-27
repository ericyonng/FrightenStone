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
 * @file  : STLUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_STLUtil_H__
#define __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_STLUtil_H__


#pragma once

#include<FrightenStone/exportbase.h>
#include "FrightenStone/common/basedefs/Macro/ForAll/ForAllMacro.h"
#include "FrightenStone/common/assist/assistobjs/assistobjs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT STLUtil
{
public:
    // del container
    template<typename _Type, AssistObjsDefs::DelMethods DelType = AssistObjsDefs::Delete>
    static void DelMapContainer(_Type &container);

    template<typename _Type, AssistObjsDefs::DelMethods DelType = AssistObjsDefs::Delete>
    static void DelSetContainer(_Type &container);

    template<typename _Type, AssistObjsDefs::DelMethods DelType = AssistObjsDefs::Delete>
    static void DelListContainer(_Type &container);

    template<typename _Type, AssistObjsDefs::DelMethods DelType = AssistObjsDefs::Delete>
    static void DelVectorContainer(_Type &container);

    template<typename _Type, AssistObjsDefs::DelMethods DelType = AssistObjsDefs::Delete>
    static void DelArray(_Type &container);
};

FS_NAMESPACE_END

#include "FrightenStone/common/assist/utils/Impl/STLUtilImpl.h"
#endif // !__Frame_Include_FrightenStone_Common_Assist_Utils_Impl_STLUtil_H__
