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
fs::ObjPoolHelper<fs::FS_List<ObjType>> fs::FS_List<ObjType>::_objPoolHelper(__DEF_OBJ_POOL_OBJ_NUM__);

FS_NAMESPACE_BEGIN

template<typename ObjType>
inline size_t FS_List<ObjType>::GetMemleakNum()
{
    return _objPoolHelper.GetMemleakObjNum();
}

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
    return _end->_obj;
}

template<typename ObjType>
inline const ObjType &FS_List<ObjType>::back() const
{
    return _end->_obj;
}

template<typename ObjType>
inline void FS_List<ObjType>::push_front(ObjType obj)
{
    auto newNode = _NewNode(obj);
    newNode->_nextNode = _head;
    if(_head)
        _head->_preNode = newNode;
    _head = newNode;
    if(!_head->_nextNode)
        _end = _head;
    ++_nodeCnt;
}

template<typename ObjType>
inline void FS_List<ObjType>::push_back(ObjType obj)
{
    auto newNode = _NewNode(obj);
    newNode->_preNode = _end;
    if(_end)
        _end->_nextNode = newNode;
    _end = newNode;
    if(!_end->_preNode)
        _head = _end;

    ++_nodeCnt;
}

template<typename ObjType>
inline void FS_List<ObjType>::pop_front()
{
    auto head = _head;
    if(_head->_nextNode)
        _head->_nextNode->_preNode = NULL;
    else
        _end = NULL;

    _head = _head->_nextNode;
    delete head;
    --_nodeCnt;
}

template<typename ObjType>
inline void FS_List<ObjType>::pop_back()
{
    auto end = _end;
    if(_end->_preNode)
        _end->_preNode->_nextNode = NULL;
    else
    {
        _head = NULL;
    }

    _end = _end->_preNode;
    delete end;
    --_nodeCnt;
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
    return _end;
}

template<typename ObjType>
inline const ListNode<ObjType> *FS_List<ObjType>::end() const
{
    return _end;
}

template<typename ObjType>
void FS_List<ObjType>::erase(ListNode<ObjType> *node)
{
    if(node == _head)
    {
        pop_front();
        return;
    }
    else if(node == _end)
    {
        pop_back();
        return;
    }

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
    if(specifyPosNode->_preNode)
        specifyPosNode->_preNode->_nextNode = newNode;

    // 与指定节点对接
    newNode->_nextNode = specifyPosNode;
    specifyPosNode->_preNode = newNode;

    if(!newNode->_preNode)
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
    while(_head)
    {
        auto node = _head;
        _head = _head->_nextNode;
        delete node;
    }
    _nodeCnt = 0;
}

template<typename ObjType>
inline ListNode<ObjType> *FS_List<ObjType>::_NewNode(ObjType &obj)
{
    return new ListNode<ObjType>(obj);
}

FS_NAMESPACE_END

#endif
