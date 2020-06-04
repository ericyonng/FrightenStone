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
 * @file  : ObjAlloctor.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_ObjPool_Defs_ObjAlloctor_H__
#define __Frame_Include_FrightenStone_Common_ObjPool_Defs_ObjAlloctor_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include <set>
#include "FrightenStone/common/objpool/Defs/ObjPoolDefs.h"
#include "FrightenStone/common/asyn/asyn.h"

#include "FrightenStone/common/objpool/Defs/FS_ObjPoolDefs.h"
#include <iostream>

FS_NAMESPACE_BEGIN

template<typename ObjType>
class ObjBlock;

template<typename ObjType>
class AlloctorNode;

// 内存分配器基类
template<typename ObjType>
class IObjAlloctor
{
public:
    IObjAlloctor(size_t blockAmountPerNode);
    virtual ~IObjAlloctor();

public:
    #pragma region thread safe api
    void *Alloc();
    void  Free(void *ptr);
    #pragma endregion

    // 线程不安全
    #pragma region no thread safe api
    void InitPool();
    void DestroyPool();
    void *MixAllocNoLocker();
    void MixFreeNoLocker(void *ptr);
    void *AllocNoLocker();
    void FreeNoLocker(void *ptr);

    // 带构造与析构操作 以下接口线程不安全
    template<typename... Args>
    ObjType *New(Args &&... args);
    ObjType *NewWithoutConstruct();
    template<typename... Args>
    ObjType *NewByPtr(void *ptr, Args &&... args);
    void Delete(ObjType *ptr);
    void DeleteWithoutDestructor(ObjType *ptr);
    #pragma endregion

    #pragma region thread safe api
    size_t GetObjInUse();
    size_t GetTotalObjBlocks();    // 当前全部的对象块个数
    size_t GetNodeCnt();
    size_t GetBytesOccupied();
    size_t GetObjBlockSize();
    void Lock();
    void UnLock();
    #pragma endregion

    void SetAllowMaxOccupiedBytes(UInt64 maxBytes);     // 线程不安全

private:
    // 线程不安全接口，多线程时接口外面请加锁
    void _NewNode();
    void _InitNode(AlloctorNode<ObjType> *newNode);
    void *_AllocFromSys();

public:
    static const size_t _objBlockSize;      // 对象块大小

protected:
    void *_curNodeObjs;                     // 当前节点的对象池缓冲
    //std::list<ObjType *> _lastDeleted;      // free对象构成的链表指针指向的内存存储的是上一次释放的对象的地址
    ObjType *_lastDeleted;
    size_t _alloctedInCurNode;              // 当前节点已分配的对象个数
    size_t _nodeCapacity;                   // 当前节点分配的数量
    AlloctorNode<ObjType> *_header;         // 头节点
    AlloctorNode<ObjType> *_lastNode;       // 最新的节点
    size_t _objpoolAllowedMaxOccupiedBytes; // 对象池允许的最大占用

    // 内存泄漏相关
    size_t          _nodeCnt;               // 节点个数
    size_t          _bytesOccupied;         // 对象池占用内存大小 = _objBlockSize * _nodeCnt * node._capacity
    size_t          _objInUse;              // 正在使用的对象
    size_t          _totalBlockCnt;         // 总的块数量
    Locker          _locker;                // 线程安全
};

FS_NAMESPACE_END

#include "FrightenStone/common/objpool/Defs/ObjAlloctorImpl.h"

#endif
