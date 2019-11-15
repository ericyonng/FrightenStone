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
 * @file  : FS_ListImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/30
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_FS_List_Impl_FS_List_H__
#pragma once

template<typename ObjType>
fs::ObjPoolHelper<fs:: FS_List<ObjType>> *fs:: FS_List<ObjType>::_objPoolHelper = fs::Singleton<fs::ObjPoolHelper<fs:: FS_List<ObjType>>, fs::AssistObjsDefs::NoDel>::GetInstance(__DEF_OBJ_POOL_OBJ_NUM__);

template<typename ObjType>
inline size_t fs:: FS_List<ObjType>::GetMemleakNum()
{
    return _objPoolHelper->GetMemleakObjNum();
}

FS_NAMESPACE_BEGIN

// template<typename ObjType>
// inline size_t FS_List<ObjType>::GetMemleakNum()
// {
//     return _objPoolHelper.GetMemleakObjNum();
// }

template<typename ObjType>
inline FS_List<ObjType>::FS_List()
{
}

template<typename ObjType>
inline FS_List<ObjType>::~FS_List()
{
    clear();
}

template<typename ObjType>
inline ObjType &FS_List<ObjType>::front()
{
    return _head->_obj;
}

template<typename ObjType>
inline const ObjType &FS_List<ObjType>::front() const
{
    return _head->_obj;
}

template<typename ObjType>
inline ObjType &FS_List<ObjType>::back()
{
    return _head->_preNode->_obj;
}

template<typename ObjType>
inline const ObjType &FS_List<ObjType>::back() const
{
    return _head->_preNode->_obj;
}

template<typename ObjType>
inline void FS_List<ObjType>::push_front(ObjType obj)
{
    auto newNode = _NewNode(obj);
    if(_head)
    {
        // 首节点换位
        newNode->_nextNode = _head;
        newNode->_preNode = _head->_preNode;

        // 末节点指向首节点
        _head->_preNode->_nextNode = newNode;
        _head->_preNode = newNode;
    }
    else
    {
        newNode->_preNode = newNode;
        newNode->_nextNode = newNode;
    }

    _head = newNode;
    ++_nodeCnt;
}

template<typename ObjType>
inline void FS_List<ObjType>::push_back(ObjType obj)
{
    auto newNode = _NewNode(obj);
    if(_head)
    {
        // 首节点换位
        newNode->_nextNode = _head;
        newNode->_preNode = _head->_preNode;

        // 末节点指向首节点
        _head->_preNode->_nextNode = newNode;
        _head->_preNode = newNode;
    }
    else
    {
        newNode->_preNode = newNode;
        newNode->_nextNode = newNode;
        _head = newNode;
    }

    ++_nodeCnt;
}

template<typename ObjType>
inline void FS_List<ObjType>::pop_front()
{
    erase(_head);
}

template<typename ObjType>
inline void FS_List<ObjType>::pop_back()
{
    erase(_head->_preNode);
}

template<typename ObjType>
inline ListNode<ObjType> *FS_List<ObjType>::begin()
{
    return _head;
}

template<typename ObjType>
inline const ListNode<ObjType> *FS_List<ObjType>::begin() const
{
    return _head;
}

template<typename ObjType>
inline ListNode<ObjType> *FS_List<ObjType>::end()
{
    return _head->_preNode;
}

template<typename ObjType>
inline const ListNode<ObjType> *FS_List<ObjType>::end() const
{
    return _head->_preNode;
}

template<typename ObjType>
inline void FS_List<ObjType>::erase(ListNode<ObjType> *node)
{
    // 前后节点对接
    if(_nodeCnt == 1)
    {
        _head = NULL;
        delete node;
        --_nodeCnt;
        return;
    }
    
    // 是否头
    if(_head == node)
        _head = node->_nextNode;

    node->_preNode->_nextNode = node->_nextNode;
    node->_nextNode->_preNode = node->_preNode;
    delete node;
    --_nodeCnt;
}

template<typename ObjType>
inline void FS_List<ObjType>::insert_before(ObjType newObj, ListNode<ObjType> *specifyPosNode)
{
    auto newNode = _NewNode(newObj);
    // 与旧的前节点对接
    newNode->_preNode = specifyPosNode->_preNode;
    specifyPosNode->_preNode->_nextNode = newNode;

    // 与指定节点对接
    newNode->_nextNode = specifyPosNode;
    specifyPosNode->_preNode = newNode;

    if(specifyPosNode == _head)
        _head = newNode;
    ++_nodeCnt;
}

template<typename ObjType>
inline bool FS_List<ObjType>::empty() const
{
    return _head == NULL;
}

template<typename ObjType>
inline size_t FS_List<ObjType>::size() const
{
    return _nodeCnt;
}

template<typename ObjType>
inline void FS_List<ObjType>::clear()
{
    while(_nodeCnt > 0)
    {
        --_nodeCnt;
        auto node = _head;
        _head = _head->_nextNode;
        delete node;
    }
}

template<typename ObjType>
inline ListNode<ObjType> *FS_List<ObjType>::_NewNode(ObjType &obj)
{
    return new ListNode<ObjType>(obj);
}

FS_NAMESPACE_END

#endif
