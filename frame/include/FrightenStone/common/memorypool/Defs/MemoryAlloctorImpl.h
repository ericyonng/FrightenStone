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
 * @file  : MemoryAlloctorImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_MemoryPool_Defs_MemoryAlloctor_H__
#pragma once

FS_NAMESPACE_BEGIN
template<typename ObjType>
inline ObjType *IMemoryAlloctor::MixAlloc(size_t bytes)
{
    return reinterpret_cast<ObjType *>(MixAlloc(bytes));
}

template<typename ObjType>
inline ObjType *IMemoryAlloctor::AllocMemory(size_t bytesCnt)
{
    return reinterpret_cast<ObjType *>(AllocMemory(bytesCnt));
}

inline size_t IMemoryAlloctor::GetBlockSize() const
{
    return _blockSize;
}

inline size_t IMemoryAlloctor::GetEffectiveBlockSize() const
{
    return _effectiveBlockSize;
}

inline size_t IMemoryAlloctor::GetOccupiedBytes() const
{
    return _totalOccupiedSize;
}

inline size_t IMemoryAlloctor::GetInUsedBytes() const
{
    return _memBlockInUse * _blockSize;
}

inline void IMemoryAlloctor::Lock()
{
    _locker.Lock();
}

inline void IMemoryAlloctor::Unlock()
{
    _locker.Unlock();
}

inline void MemoryAlloctorWithLimit::_UpdateCanCreateNewNode(size_t addOccupiedBytes)
{
    if(!_canCreateNewNodeForAlloctor)
        return;

    _curAlloctorOccupiedBytes += addOccupiedBytes;
    if(_curAlloctorOccupiedBytes >= _maxAlloctorBytes)
        _canCreateNewNodeForAlloctor = false;
}

FS_NAMESPACE_END

#endif
