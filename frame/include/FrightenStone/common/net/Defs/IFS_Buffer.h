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
 * @file  : IFS_Buffer.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/13
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_IFS_Buffer_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_IFS_Buffer_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/objpool/objpool.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/assist/utils/Impl/StringUtil.h"

FS_NAMESPACE_BEGIN

// buffer不可给其他线程使用，只能在单线程中 避免IMemoryAlloctor未定义出错 IMemoryAlloctor 是外部传入，外部释放
class BASE_EXPORT IFS_Buffer
{
    OBJ_POOL_CREATE_ANCESTOR(IFS_Buffer);
public:
    // 支持内存分配器
    IFS_Buffer(size_t bufferSize, IMemoryAlloctor *memAlloctor);
    virtual ~IFS_Buffer();

public:
    void PopFront(Int64 bytesLen);
    void PushBack(const Byte8 *data, size_t len);
    void RollbackPush(Int64 len);
    bool CanPush(size_t len);
    size_t GetSuitableBytesToPush(size_t len);
    void Clear();
    bool IsEmpty() const;
    bool IsFull() const;
    Int64 GetLength() const;
    const Int64 &GetDataLen() const;
    Int64 GetTotalSize() const;
    Int64 GetRest() const;
    Byte8 *GetData();
    const Byte8 *GetData() const;
    Byte8 *GetStartPush(Int64 *&curPos);
    virtual FS_String ToString() const;

    template<typename ObjType>
    ObjType *CastToBuffer();
    template<typename ObjType>
    const ObjType *CastToBuffer() const;
    
protected:
    Int64 &_GetCurPos();

private:
    void _Init();
    void _Destroy();
    
protected:
    IMemoryAlloctor *_alloctor;
    Int64 _bufferSize;
    Byte8 *_buff;
    Int64 _curPos;     // 当前数据末尾位置
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Defs/IFS_BufferImpl.h"

#endif
