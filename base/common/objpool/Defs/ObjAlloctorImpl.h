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
 * @file  : ObjAlloctorImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_ObjPool_Defs_ObjAlloctor_H__
#pragma once

#ifndef BLOCK_AMOUNT_DEF
#define BLOCK_AMOUNT_DEF    10240    // 默认内存块数量
#endif

FS_NAMESPACE_BEGIN

template<typename ObjType>
inline IObjAlloctor<ObjType>::IObjAlloctor(AlloctorNode<ObjType> *curNode, size_t blockAmount)
    :_isInit(false)
    ,_buf(NULL)
    ,_curNode(curNode)
    ,_blockAmount(blockAmount)
    ,_freeBlockLeft(blockAmount)
    ,_usableBlockHeader(NULL)
    ,_blockSize(sizeof(ObjType))
    ,_objInUse(0)
{
    _blockSize =_blockSize / sizeof(void *) * sizeof(void *) + (_blockSize % sizeof(void *) ? sizeof(void *) : 0);
    _blockSize = _blockSize + sizeof(ObjBlock<ObjType>);
}

template<typename ObjType>
inline IObjAlloctor<ObjType>::~IObjAlloctor()
{
    FinishMemory();
}

template<typename ObjType>
inline void *IObjAlloctor<ObjType>::Alloc()
{
    // 内存不足则使用系统内存分配方案
    ObjBlock<ObjType> *newBlock = NULL;
    if(_usableBlockHeader == 0)
    {
        newBlock = reinterpret_cast<ObjBlock<ObjType> *>(::malloc(_blockSize));
        newBlock->_isInPool = false;    // 不在内存池内
        newBlock->_ref = 1;             // 记1次引用
        newBlock->_alloctor = this;     // 分配器
        newBlock->_nextBlock = 0;
        newBlock->_objSize = sizeof(ObjType);
    }
    else
    {
        /**
        *   get one node from free list
        */
        newBlock = _usableBlockHeader;
        _usableBlockHeader = _usableBlockHeader->_nextBlock;
        newBlock->_alloctor =   this;
        newBlock->_isInPool = true;
        --_freeBlockLeft;

        ASSERT(newBlock->_ref == 0);
        newBlock->_ref = 1;
    }

    ++_objInUse;
//     if(newBlock)
//         _inUsings.insert(newBlock);

    return ((char*)newBlock) + sizeof(ObjBlock<ObjType>);   // 从block的下一个地址开始才是真正的申请到的内存
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::Free(void *ptr)
{
    // 内存块头
    char *ptrToFree = reinterpret_cast<char *>(ptr);
    ObjBlock<ObjType> *blockHeader = reinterpret_cast<ObjBlock<ObjType> *>(reinterpret_cast<char*>(ptrToFree - sizeof(ObjBlock<ObjType>)));

    // 引用计数
    if(--blockHeader->_ref != 0)
        return;

    // 不是内存池内且引用计数为0
    if (!blockHeader->_isInPool)
    {
        ::free(blockHeader);
        --_objInUse;
        // _inUsings.erase(blockHeader);
        return;
    }

    // 被释放的节点插到可用头节点之前
    blockHeader->_nextBlock = _usableBlockHeader;
    _usableBlockHeader = blockHeader;
    ++_freeBlockLeft;
    --_objInUse;

    // 释放后从正在使用中移除
    // _inUsings.erase(blockHeader);
}

template<typename ObjType>
inline bool IObjAlloctor<ObjType>::NotBusy()
{
    return (_freeBlockLeft * 100 / _blockAmount) >= ObjPoolDefs::__g_FreeRate;
}

template<typename ObjType>
inline bool IObjAlloctor<ObjType>::IsEmpty() const
{
    return !_usableBlockHeader;
}

template<typename ObjType>
inline size_t IObjAlloctor<ObjType>::GetObjInUse() const
{
    return _objInUse;
}

template<typename ObjType>
inline AlloctorNode<ObjType> *IObjAlloctor<ObjType>::GetNode()
{
    return _curNode;
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::InitMemory()
{
    if(_isInit)
        return;

    ASSERT(_buf == 0);
    if(_buf)
        return;

    /**
    *   计算需要申请的内存大小，其中包含内存头数据大小
    */
    size_t	bufSize = _blockSize * _blockAmount;

    /**
    *   申请内存
    */
    _buf = reinterpret_cast<char*>(::malloc(bufSize));
    memset(_buf, 0, bufSize);

    /**
    *   链表头和尾部指向同一位置
    */
    _usableBlockHeader = reinterpret_cast<ObjBlock<ObjType> *>(_buf);
    _usableBlockHeader->_ref = 0;
    _usableBlockHeader->_alloctor = this;
    _usableBlockHeader->_nextBlock = 0;
    _usableBlockHeader->_isInPool = true;
    _usableBlockHeader->_objSize = sizeof(ObjType);
    ObjBlock<ObjType> *temp = _usableBlockHeader;

    // 构建内存块链表
    for(size_t i = 1; i < _blockAmount; ++i)
    {
        char *cache = (_buf + _blockSize * i);
        ObjBlock<ObjType> *block = reinterpret_cast<ObjBlock<ObjType> *>(cache);
        block->_ref = 0;
        block->_isInPool = true;
        block->_alloctor = this;
        block->_nextBlock = 0;
        block->_objSize = sizeof(ObjType);
        temp->_nextBlock = block;
        temp = block;
    }

    _isInit = true;
}

template<typename ObjType>
inline void IObjAlloctor<ObjType>::FinishMemory()
{
    if(!_isInit)
        return;

    _isInit = false;

    // 收集并释放泄漏的内存
    std::pair<Int64, Int64> amountRefSizePair;
//     for(auto &block : _inUsings)
//     {
//         if(block->_ref)
//         {
//             amountRefSizePair.second += 1;
//             amountRefSizePair.first += _blockSize;
//             if(!block->_isInPool)
//                 ::free(block);
//         }
//     }

    // 使用系统的内存分配释放掉
    while(_usableBlockHeader != 0)
    {
        auto *header = _usableBlockHeader;
        _usableBlockHeader = _usableBlockHeader->_nextBlock;

        if(!header->_isInPool)
            ::free(header);
    }
    _usableBlockHeader = 0;

    // 释放本系统内存
    if(_buf)
    {
        ::free(_buf);
        _buf = NULL;
    }

    // 内存泄漏打印
//     if(amountRefSizePair.first)
//     {
//         g_Log->memleak("objName[%s], amount[%lld]*size[%lld];"
//                        , typeid(ObjType).name(), amountRefSizePair.first, amountRefSizePair.second);
//     }
}

FS_NAMESPACE_END

#endif
