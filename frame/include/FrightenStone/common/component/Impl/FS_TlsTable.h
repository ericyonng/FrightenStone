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
 * @file  : FS_TlsTable.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */

#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_FS_TlsTable_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_FS_TlsTable_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

struct ITlsBase;

// 请不要过度依赖于线程局部存储
// 请勿频繁创建
// 请保证线程周期内都不会释放,数组类型元素请在外部包装成派生于ITlsBase基类的结构体，再进行存储
class BASE_EXPORT FS_TlsTable
{
public:
    FS_TlsTable();
    ~FS_TlsTable();

public:
    template<typename ObjType>
    ObjType *GetElement(Int32 type);
    template<typename ObjType, typename... Args>
    ObjType *AllocElement(Int32 type, Args... args);
    void Erase(Int32 type);

private:
    std::map<Int32, ITlsBase *> _elementTypeRefElements;
};

FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/FS_TlsTableImpl.h"



#endif