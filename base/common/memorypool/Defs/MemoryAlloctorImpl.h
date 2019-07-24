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
 * @file  : MemoryAlloctorImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_MemoryPool_Defs_MemoryAlloctor_H__
#pragma once

FS_NAMESPACE_BEGIN

inline size_t IMemoryAlloctor::GetBlockSize() const
{
    return _blockSize;
}

template<typename ObjType>
inline void *IMemoryAlloctor::AllocMemory(size_t bytesCnt, const Byte8 *objName)
{
    // 内存不足则使用系统内存分配方案
    MemoryBlock<ObjType> *newBlock = NULL;
    if(_usableBlockHeader == 0)
    {
        newBlock = reinterpret_cast<MemoryBlock<ObjType> *>(::malloc(bytesCnt + sizeof(MemoryBlock<ObjType>)));
        newBlock->_isInPool = false;    // 不在内存池内
        newBlock->_ref = 1;             // 记1次引用
        newBlock->_alloctor = this;     // 分配器
        newBlock->_nextBlock = 0;
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

FS_NAMESPACE_END

#endif
