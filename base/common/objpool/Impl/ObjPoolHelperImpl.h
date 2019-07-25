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
 * @file  : ObjPoolHelperImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/22
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_ObjPool_Impl_ObjPoolHelper_H__
#pragma once

FS_NAMESPACE_BEGIN

template<typename ObjType>
inline ObjPoolHelper<ObjType>::ObjPoolHelper(size_t objAmount)
    : _objAmount(objAmount)
    ,_objAlloctorHeader(NULL)
{
    _Init();
}

template<typename ObjType>
inline ObjPoolHelper<ObjType>::~ObjPoolHelper()
{
    if(_objAlloctorHeader)
    {
#if __FS_THREAD_SAFE__
        _Lock();
#endif

        _Finish();

#if __FS_THREAD_SAFE__
        _Unlock();
#endif
    }
}

template<typename ObjType>
inline void *ObjPoolHelper<ObjType>::Alloc()
{
#if __FS_THREAD_SAFE__
    _Lock();
#endif

    // 从头节点分配内存
    auto ptr = _objAlloctorHeader->_curAlloctor->Alloc();

    // 头结点若内存满则创建新分配器并插入节点头部
    if(_objAlloctorHeader->_curAlloctor->IsEmpty())
        _SwitchToHeader(_NewAlloctorNode());

#if __FS_THREAD_SAFE__
    _Unlock();
#endif
    return  ptr;
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::Free(void *ptr)
{
#if __FS_THREAD_SAFE__
     _Lock();
#endif

     // 通过地址找到所在节点
     auto node = _PtrToAlloctorNode(ptr);

     // 通过节点的分配器释放内存
     node->_curAlloctor->Free(ptr);

     // 内存分配器内存充裕就直接放到头节点
     if(node->_curAlloctor->NotBusy())
         _SwitchToHeader(node);

#if __FS_THREAD_SAFE__
    _Unlock();
#endif
}

template<typename T>
inline void ObjPoolHelper<T>::AddRef(void *ptr)
{
#if __FS_THREAD_SAFE__
    _Lock();
#endif

    // 通过地址找到所在节点
    _PtrToAlloctorNode(ptr)->_curAlloctor->AddRef(ptr);

#if __FS_THREAD_SAFE__
    _Unlock();
#endif
}

template<typename ObjType>
inline size_t ObjPoolHelper<ObjType>::GetMemleakObjNum() const
{
    auto header = _objAlloctorHeader;
    size_t cnt = 0;
    do 
    {
        cnt += header->_curAlloctor->GetObjInUse();
        header = header->_nextNode;
    } while (header);

    return cnt;
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_Init()
{
    if(_objAlloctorHeader)
        return;

    _objAlloctorHeader = _NewAlloctorNode();
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_Finish()
{
    while(_objAlloctorHeader)
    {
        _objAlloctorHeader->_curAlloctor->FinishMemory();
        Fs_SafeFree(_objAlloctorHeader->_curAlloctor);
        auto curNode = _objAlloctorHeader;

        if(curNode->_preNode)
            curNode->_preNode->_nextNode = curNode->_nextNode;

        if(curNode->_nextNode)
            curNode->_nextNode->_preNode = curNode->_preNode;

        _objAlloctorHeader = curNode->_nextNode;
        Fs_SafeFree(curNode);
    }
    _objAlloctorHeader = NULL;
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_Lock()
{
    _locker.Lock();
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_Unlock()
{
    _locker.Lock();
}

template<typename ObjType>
inline AlloctorNode<ObjType> *ObjPoolHelper<ObjType>::_NewAlloctorNode()
{
    // 初始化新的分配器
    auto newAlloctor = new IObjAlloctor<ObjType>(new AlloctorNode<ObjType>, _objAmount);
    newAlloctor->_curNode->_curAlloctor = newAlloctor;
    newAlloctor->InitMemory();
    return newAlloctor->_curNode;
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_SwitchToHeader(AlloctorNode<ObjType> *node)
{
    // 已经是第一个节点
    if(_objAlloctorHeader == node)
        return;

    // 从前后节点中间脱离（判断前后节点是否为空，都为空即本节点为头节点）
    if(node->_preNode)
        node->_preNode->_nextNode = node->_nextNode;

    if(node->_nextNode)
        node->_nextNode->_preNode = node->_preNode;

    // 与头结点对接
    _ReplaceHeader(node);
}

template<typename ObjType>
inline void ObjPoolHelper<ObjType>::_ReplaceHeader(AlloctorNode<ObjType> *node)
{
    _objAlloctorHeader->_preNode = node;
    node->_nextNode = _objAlloctorHeader;
    _objAlloctorHeader = node;
}

template<typename ObjType>
inline AlloctorNode<ObjType> *ObjPoolHelper<ObjType>::_PtrToAlloctorNode(void *ptr)
{
    // 内存块头
    char *ptrToFree = reinterpret_cast<char *>(ptr);
    ObjBlock<ObjType> *blockHeader = reinterpret_cast<ObjBlock<ObjType> *>(reinterpret_cast<char*>(ptrToFree - sizeof(ObjBlock<ObjType>)));
    return blockHeader->_alloctor->GetNode();
}

FS_NAMESPACE_END

#endif
