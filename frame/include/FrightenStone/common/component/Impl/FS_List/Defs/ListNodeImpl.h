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
 * @file  : ListNodeImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/30
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Component_Impl_FS_List_Defs_ListNode_H__
#pragma once

template<typename ObjType>
fs::ObjPoolHelper<fs::ListNode<ObjType>> *fs::ListNode<ObjType>::_objPoolHelper = fs::Singleton<fs::ObjPoolHelper<fs::ListNode<ObjType>>, fs::AssistObjsDefs::NoDel>::GetInstance(__DEF_OBJ_POOL_OBJ_NUM__);

template<typename ObjType>
inline size_t fs::ListNode<ObjType>::GetMemleakNum()
{
    return _objPoolHelper->GetMemleakObjNum();
}

FS_NAMESPACE_BEGIN

// template<typename ObjType>
// inline size_t ListNode<ObjType>::GetMemleakNum()
// {
//     return _objPoolHelper.GetMemleakObjNum();
// }

template<typename ObjType>
inline ListNode<ObjType>::ListNode(ObjType obj)
    :_obj(obj)
{
}

FS_NAMESPACE_END

#endif
