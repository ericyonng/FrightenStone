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
 * @file  : FS_List.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/30
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_FS_List_Impl_FS_List_H__
#define __Base_Common_Component_Impl_FS_List_Impl_FS_List_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/memorypool/memorypool.h"
#include "base/common/component/Impl/FS_List/Defs/ListNode.h"

FS_NAMESPACE_BEGIN

template<typename ObjType>
class FS_List
{
    OBJ_POOL_CREATE_DEF(FS_List<ObjType>);
public:
    FS_List();
    ~FS_List();

    /* 值操作 */
public:
    ObjType &front();
    const ObjType &front() const;
    ObjType &back();
    const ObjType &back() const;
    void push_front(ObjType obj);
    void push_back(ObjType obj);
    void pop_front();
    void pop_back();

    /* 节点迭代器操作 */
public:
    ListNode<ObjType> *begin();
    const ListNode<ObjType> *begin() const;
    ListNode<ObjType> *end();
    const ListNode<ObjType> *end() const;
    void erase(ListNode<ObjType> * node);
    void insert_before(ObjType newObj, ListNode<ObjType> *specifyPosNode);

    /* 杂项 */
public:
    bool empty() const;
    size_t size() const;
    void clear();

private:
    ListNode<ObjType> *_NewNode(ObjType &obj);

private:
    ListNode<ObjType> *_head = NULL;
    size_t _nodeCnt = 0;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_List/Impl/FS_ListImpl.h"

#endif
