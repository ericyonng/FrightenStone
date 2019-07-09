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
 * @file  : MemoryAlloctor.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/memorypool/Defs/MemoryAlloctor.h"
#include "base/common/memorypool/Defs/MemoryBlock.h"
#include "base/common/log/Log.h"

#define BLOCK_AMOUNT_DEF    1024    // 默认内存块数量

FS_NAMESPACE_BEGIN

IMemoryAlloctor::IMemoryAlloctor()
    :_isInit(false)
    ,_buf(NULL)
    ,_blockAmount(BLOCK_AMOUNT_DEF)
    ,_usableBlockHeader(NULL)
    ,_blockSize(0)
{
    
}

IMemoryAlloctor::~IMemoryAlloctor()
{
    FinishMemory();
}

void *IMemoryAlloctor::AllocMemory(size_t bytesCnt, const Byte8 *objName)
{
//     if(UNLIKELY(!_buf))
//         _InitMemory();

    // 内存不足则使用系统内存分配方案
    MemoryBlock *newBlock = NULL;
    if(_usableBlockHeader == 0)
    {
        newBlock = reinterpret_cast<MemoryBlock *>(::malloc(bytesCnt + sizeof(MemoryBlock)));
        newBlock->_isInPool = false;    // 不在内存池内
        newBlock->_ref = 1;             // 记1次引用
        newBlock->_alloctor = this;     // 分配器
        newBlock->_nextBlock = 0;
        auto len = sprintf(newBlock->_objName, "%s", objName);
        if(len > 0)
            newBlock->_objName[BUFFER_LEN256 > len ? len : BUFFER_LEN256 - 1] = 0;
        else
            newBlock->_objName[0] = 0;
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
        auto len = sprintf(newBlock->_objName, "%s", objName);
        if(len > 0)
            newBlock->_objName[BUFFER_LEN256 > len ? len : BUFFER_LEN256 - 1] = 0;
        else
            newBlock->_objName[0] = 0;

        ASSERT(newBlock->_ref == 0);
        newBlock->_ref = 1;
    }
    if(newBlock)
    {
        newBlock->_objSize = static_cast<Int64>(bytesCnt);
        _usingBlocks.insert(newBlock);
    }

    return ((char*)newBlock) + sizeof(MemoryBlock);   // 从block的下一个地址开始才是真正的申请到的内存
}

void IMemoryAlloctor::FreeMemory(void *ptr)
{
    // 内存块头
    char *ptrToFree = reinterpret_cast<char*>(ptr);
    MemoryBlock *blockHeader = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(ptrToFree - sizeof(MemoryBlock)));

    // 引用计数
    if(--blockHeader->_ref != 0)
        return;

    // 不是内存池内且引用计数为0
    if (!blockHeader->_isInPool)
    {
        ::free(blockHeader);
        blockHeader->_objName[0] = 0;
        _usingBlocks.erase(blockHeader);
        return;
    }

    // 被释放的节点插到可用头节点之前
    blockHeader->_objName[0] = 0;
    blockHeader->_nextBlock = _usableBlockHeader;
    _usableBlockHeader = blockHeader;
    _usingBlocks.erase(blockHeader);
}

void IMemoryAlloctor::InitMemory()
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
    _usableBlockHeader = reinterpret_cast<MemoryBlock*>(_buf);
    _usableBlockHeader->_ref = 0;
    _usableBlockHeader->_alloctor = this;
    _usableBlockHeader->_nextBlock = 0;
    _usableBlockHeader->_isInPool = true;
    _usableBlockHeader->_objName[0] = 0;
    MemoryBlock *temp = _usableBlockHeader;

    // 构建内存块链表
    for(size_t i = 1; i < _blockAmount; ++i)
    {
        char *cache = (_buf + _blockSize * i);
        MemoryBlock *block = reinterpret_cast<MemoryBlock*>(cache);
        block->_ref = 0;
        block->_isInPool = true;
        block->_alloctor = this;
        block->_nextBlock = 0;
        block->_objName[0] = 0;
        temp->_nextBlock = block;
        temp = block;
    }

    _isInit = true;
}

void IMemoryAlloctor::FinishMemory()
{
    if(!_isInit)
        return;

    _isInit = false;

    // 收集并释放泄漏的内存
    std::map<FS_String, std::pair<Int64, Int64>> objNameRefAmountSizePair;
    FS_String objName;
    for(auto &block : _usingBlocks)
    {
        if(block->_ref)
        {
            objName = block->_objName;
            auto iterAmount = objNameRefAmountSizePair.find(objName);
            if(iterAmount == objNameRefAmountSizePair.end())
                iterAmount = objNameRefAmountSizePair.insert(std::make_pair(objName, std::pair<Int64, Int64>())).first;
            iterAmount->second.first += 1;
            iterAmount->second.second = block->_objSize;
            if(!block->_isInPool)
                ::free(block);
        }
    }

    // 使用系统的内存分配释放掉
    while(_usableBlockHeader != 0)
    {
        MemoryBlock *header = _usableBlockHeader;
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
    for(auto &memleakObj : objNameRefAmountSizePair)
        g_Log->memleak("objName[%s], amount[%lld]*size[%lld];", memleakObj.first.c_str(), memleakObj.second.first, memleakObj.second.second);
}

size_t IMemoryAlloctor::_GetFreeBlock()
{
    MemoryBlock *temp = _usableBlockHeader;
    size_t cnt = 0;
    while(temp != 0)
    {
        ++cnt;
        temp = temp->_nextBlock;
    }
    return  cnt;
}

MemoryAlloctor::MemoryAlloctor(size_t blockSize, size_t blockAmount)
{
    _blockAmount = blockAmount;
    _blockSize = blockSize / sizeof(void *) * sizeof(void *) + (blockSize % sizeof(void *) ? sizeof(void *) : 0);
    _blockSize = _blockSize + sizeof(MemoryBlock);
}

MemoryAlloctor::~MemoryAlloctor()
{
}

FS_NAMESPACE_END


