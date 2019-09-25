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
 * @file  : FS_ArgsExpander.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_FS_ArgsExpander_H__
#define __Base_Common_Component_Impl_FS_ArgsExpander_H__


#pragma once
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

template<typename ObjType>
class BASE_EXPORT FS_ArgsExpander
{
public:
    template<typename... Args>
    static void Expand(Args&&... args);
};

template<typename ObjType>
class BASE_EXPORT FS_RecursiveArgsExpander
{
public:
    template<typename Arg0, typename... Args>
    static void Expand(Arg0&& arg0, Args&&... rest);

    template<typename Arg0>
    static void Expand(Arg0&& arg0);
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_ArgsExpanderImpl.h"
#endif
