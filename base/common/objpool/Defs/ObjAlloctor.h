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
 * @file  : ObjAlloctor.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_ObjPool_Defs_ObjAlloctor_H__
#define __Base_Common_ObjPool_Defs_ObjAlloctor_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include <set>
#include "base/common/objpool/Defs/ObjPoolDefs.h"
#include "base/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

template<typename ObjType>
class ObjBlock;

template<typename ObjType>
class AlloctorNode;

// 内存分配器基类
template<typename ObjType>
class IObjAlloctor
{
    template<typename ObjType>
    friend class ObjPoolHelper;
public:
    IObjAlloctor(size_t blockAmountPerNode);
    virtual ~IObjAlloctor();

public:
    // 依据__FS_THREAD_SAFE__开关开启线程安全
    void *Alloc();
    void  Free(void *ptr);

    // 线程不安全
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

    size_t GetObjInUse();
    size_t GetTotalObjBlocks();    // 当前全部的对象块个数
    size_t GetNodeCnt();
    size_t GetBytesOccupied();
    size_t GetObjBlockSize();
    void Lock();
    void UnLock();

private:
    // 线程不安全接口，多线程时接口外面请加锁
    void _NewNode();

protected:
    void *_curNodeObjs;                     // 当前节点的对象池缓冲
    //std::list<ObjType *> _lastDeleted;      // free对象构成的链表指针指向的内存存储的是上一次释放的对象的地址
    ObjType *_lastDeleted;
    size_t _alloctedInCurNode;              // 当前节点已分配的对象个数
    const size_t _nodeCapacity;             // 每个节点对象个数
    AlloctorNode<ObjType> *_header;         // 头节点
    AlloctorNode<ObjType> *_lastNode;       // 最新的节点
    static const size_t _objBlockSize;      // 对象块大小

    // 内存泄漏相关
    size_t          _nodeCnt;               // 节点个数
    size_t          _bytesOccupied;         // 对象池占用内存大小 = _objBlockSize * _nodeCnt * node._capacity
    size_t          _objInUse;              // 正在使用的对象
    Locker          _locker;                // 线程安全
};

FS_NAMESPACE_END

#include "base/common/objpool/Defs/ObjAlloctorImpl.h"

#endif
