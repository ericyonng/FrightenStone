/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : ObjAlloctorImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_ObjPool_Defs_ObjAlloctor_H__
#pragma once

FS_NAMESPACE_BEGIN

// objtype的blocksize必须是void *尺寸的整数倍
template<typename ObjType>
const size_t IObjAlloctor<ObjType>::_objBlockSize = __FS_MEMORY_ALIGN__(sizeof(ObjType) + sizeof(ObjBlock<ObjType>));

template<typename ObjType>
inline IObjAlloctor<ObjType>::IObjAlloctor(size_t blockAmount)
    :_curNodeObjs(NULL)
    ,_alloctedInCurNode(0)
    ,_nodeCapacity(blockAmount)
    ,_header(NULL)
    ,_lastNode(NULL)
    ,_nodeCnt(0)
    ,_bytesOccupied(0)
    ,_objInUse(0)
    ,_lastDeleted(NULL)
    ,_objpoolAllowedMaxOccupiedBytes(__OBJ_POOL_MAX_ALLOW_BYTES_DEF__)
    ,_totalBlockCnt(0)
{

}

template<typename ObjType>
inline IObjAlloctor<ObjType>::~IObjAlloctor()
{
    DestroyPool();
}

template<typename ObjType>
inline void *IObjAlloctor<ObjType>::Alloc()
{
    _locker.Lock();
    auto ptr = MixAllocNoLocker();
    _locker.Unlock();
    return ptr;
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::Free(void *ptr)
{
    _locker.Lock();
    // free的对象构成链表用于下次分配
    MixFreeNoLocker(ptr);
    _locker.Unlock();
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::InitPool()
{
    if(_header)
        return;

    // 初始化节点
    _header = new AlloctorNode<ObjType>(_nodeCapacity);
    _lastNode = _header;

    // lastnode在构造中初始化
    _curNodeObjs = _lastNode->_objs;
    _alloctedInCurNode = 0;
    ++_nodeCnt;
    _totalBlockCnt += _lastNode->_blockCnt;
    _bytesOccupied += _lastNode->_nodeSize;

    _InitNode(_lastNode);
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::DestroyPool()
{
    if(UNLIKELY(!_header))
        return;

    auto *node = _header->_nextNode;
    while(node)
    {
        auto *nextNode = node->_nextNode;
        delete node;
        node = nextNode;
    }
    Fs_SafeFree(_header);
}

template<typename ObjType>
inline void *IObjAlloctor<ObjType>::MixAllocNoLocker()
{
    auto ptr = AllocNoLocker();
    if(ptr)
        return ptr;

    return _AllocFromSys();
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::MixFreeNoLocker(void *ptr)
{
    ObjBlock<ObjType> *ptrHeader = reinterpret_cast<ObjBlock<ObjType> *>(reinterpret_cast<char *>(reinterpret_cast<char *>(ptr) - sizeof(ObjBlock<ObjType>)));
//     std::cout << "ptrHeader=" << ptrHeader << std::endl;
//     std::cout << "ptr" << ptr << std::endl;
//     std::cout << "header" << header << std::endl;
//     std::cout << "sizeof(ObjBlock<ObjType>)" << sizeof(ObjBlock<ObjType>) << std::endl;
//     std::cout << "isInPool" << ptrHeader->_isInPool << std::endl;
    if(ptrHeader->_isInPool)
    {
        FreeNoLocker(ptr);
        return;
    }

    ::free(ptrHeader);
}

template<typename ObjType>
inline void *IObjAlloctor<ObjType>::AllocNoLocker()
{
    if(_lastDeleted)
    {
        ObjType *ptr = _lastDeleted;
        _lastDeleted = *((ObjType **)(_lastDeleted));
        ++_objInUse;
        return ptr;
    }

    // 分配新节点
    if(_alloctedInCurNode >= _lastNode->_blockCnt)
    {
        // 即将增加的内存大于允许占用的内存
        if((g_curObjPoolOccupiedBytes + 2 * _objBlockSize*_nodeCapacity) > _objpoolAllowedMaxOccupiedBytes)
            return NULL;

        _NewNode();
    }

    // 内存池中分配对象
    auto ptr = reinterpret_cast<char *>(_curNodeObjs) + _alloctedInCurNode * _objBlockSize;
    ++_alloctedInCurNode;
    ++_objInUse;

    // ptr是头部开始信息
    return ptr + sizeof(ObjBlock<ObjType>);
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::FreeNoLocker(void *ptr)
{
    // free的对象构成链表用于下次分配
    *((ObjType **)ptr) = _lastDeleted;
    _lastDeleted = reinterpret_cast<ObjType *>(ptr);
    --_objInUse;
}

template<typename ObjType>
template<typename... Args>
inline ObjType *IObjAlloctor<ObjType>::New(Args &&... args)
{
    return ::new(MixAllocNoLocker())ObjType(std::forward<Args>(args)...);
}

template<typename ObjType>
inline ObjType *IObjAlloctor<ObjType>::NewWithoutConstruct()
{
    return  reinterpret_cast<ObjType *>(MixAllocNoLocker());
}

template<typename ObjType>
template<typename... Args>
inline ObjType *IObjAlloctor<ObjType>::NewByPtr(void *ptr, Args &&... args)
{
    return ::new(ptr)ObjType(std::forward<Args>(args)...);
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::Delete(ObjType *ptr)
{
    // 先析构后释放
    ptr->~ObjType();

    MixFreeNoLocker(ptr);
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::DeleteWithoutDestructor(ObjType *ptr)
{
    MixFreeNoLocker(ptr);
}

template<typename ObjType>
inline size_t IObjAlloctor<ObjType>::GetObjInUse()
{
    _locker.Lock();
    const size_t inUse = _objInUse;
    _locker.Unlock();
    return inUse;
}

template<typename ObjType>
inline size_t IObjAlloctor<ObjType>::GetTotalObjBlocks()
{
    _locker.Lock();
    const size_t cnt = _totalBlockCnt;
    _locker.Unlock();
    return cnt;
}

template<typename ObjType>
inline size_t IObjAlloctor<ObjType>::GetNodeCnt()
{
    _locker.Lock();
    const size_t cnt = _nodeCnt;
    _locker.Unlock();
    return cnt;
}

template<typename ObjType>
inline size_t IObjAlloctor<ObjType>::GetBytesOccupied()
{
    _locker.Lock();
    const size_t bytes = _bytesOccupied;
    _locker.Unlock();
    return bytes;
}

template<typename ObjType>
inline size_t IObjAlloctor<ObjType>::GetObjBlockSize()
{
    return _objBlockSize;
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::Lock()
{
    _locker.Lock();
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::Unlock()
{
    _locker.Unlock();
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::SetAllowMaxOccupiedBytes(UInt64 maxBytes)
{
    _objpoolAllowedMaxOccupiedBytes = maxBytes;
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::_NewNode()
{
    // 构成链表
    auto *newNode = new AlloctorNode<ObjType>(_nodeCapacity *= 2);

    // lastnode在构造中初始化
    _lastNode->_nextNode = newNode;
    _lastNode = newNode;
    _curNodeObjs = newNode->_objs;
    _alloctedInCurNode = 0;
    ++_nodeCnt;
    _bytesOccupied += newNode->_nodeSize;
    _totalBlockCnt += newNode->_blockCnt;
    _InitNode(newNode);
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::_InitNode(AlloctorNode<ObjType> *newNode)
{
    auto curBuff = reinterpret_cast<char *>(newNode->_objs);

    /**
    *   申请内存
    */
    // memset(_curBuf, 0, newNode->_nodeSize);

    // 构建内存块链表
    for(size_t i = 0; i < newNode->_blockCnt; ++i)
    {
        char *cache = (curBuff + _objBlockSize * i);
        ObjBlock<ObjType> *block = reinterpret_cast<ObjBlock<ObjType> *>(cache);
        block->_isInPool = true;
    }
}

template<typename ObjType>
inline void * IObjAlloctor<ObjType>::_AllocFromSys()
{
    char *ptr = reinterpret_cast<char *>(::malloc(_objBlockSize));
    reinterpret_cast<ObjBlock<ObjType> *>(ptr)->_isInPool = false;
    return ptr + sizeof(ObjBlock<ObjType>);
}

FS_NAMESPACE_END

#endif
