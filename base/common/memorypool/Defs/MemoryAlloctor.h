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
 * @file  : MemoryAlloctor.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief : 需要导出内存泄漏日志
 *          alloctor结构：             | - block
 *                          node    -  | - block
 *                                     | - ...
 *
 *                          newNode...
 *
 *                                     | - block
 *                          freeList - | - block
 *                                     | - ...
 *
 *          alloc 优先从_lastDeleted链表取，内存不足创建一个节点
 *          
 *          free 时将block串成链表
 * 
 */
#ifndef __Base_Common_MemoryPool_Defs_MemoryAlloctor_H__
#define __Base_Common_MemoryPool_Defs_MemoryAlloctor_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include <set>
#include "base/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN

struct MemBlocksNode;
class MemoryBlock;
class FS_String;

// 内存分配器基类
class BASE_EXPORT IMemoryAlloctor
{
    friend class MemoryPoolMgr;
public:
    IMemoryAlloctor(const bool &canCreateNewNode);
    virtual ~IMemoryAlloctor();

public:
    void *AllocMemory(size_t bytesCnt);
    void  FreeMemory(void *ptr);
    size_t GetBlockSize() const;
    size_t GetEffectiveBlockSize() const;
    size_t GetOccupiedBytes() const;
    size_t GetInUsedBytes() const;
    void PrintMemInfo() const;

public:
    void InitMemory();
    void FinishMemory();

protected:
    void _NewNode();
    void _InitNode(MemBlocksNode *newNode);

protected:
    bool            _isFinish;
    char            *_curBuf;               // 整个已申请的内存地址
    MemoryBlock     *_usableBlockHeader;    // 结点 next方向是可用的未分配的内存块，分配时候给_usableBlockHeader节点，释放时候要释放的节点插在_usableBlockHeader之前当作可用节点头
    MemoryBlock     *_lastDeleted;          // free链表
    Int64           _memBlockInUse;         // 正在使用的内存块个数

    /* 内存节点 */
    MemBlocksNode *_header;                 // 头节点
    MemBlocksNode *_lastNode;               // 当前最新节点
    Int64 _curNodeCnt;                      // 节点个数

    // 最新的节点
    size_t          _blockAmount;           // 内存块总数量
    size_t          _blockSize;             // 内存块大小
    size_t          _effectiveBlockSize;    // 有效内存块大小（扣除MemoryBlock的内存大小）

    IDelegatePlus<void, size_t> *_updateMemPoolOccupied = NULL;
    const bool &_canCreateNewNode;
};

class MemoryAlloctor : public IMemoryAlloctor
{
public:
    MemoryAlloctor(size_t blockSize, size_t blockAmount, IDelegatePlus<void, size_t> *updateMemPoolOccupied, const bool &canCreateNewNode);
    virtual ~MemoryAlloctor();
};

FS_NAMESPACE_END

#include "base/common/memorypool/Defs/MemoryAlloctorImpl.h"

#endif
