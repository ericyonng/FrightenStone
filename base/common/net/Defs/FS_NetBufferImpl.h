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
 * @file  : FS_NetBufferImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/5
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Net_Defs_FS_NetBuffer_H__
#pragma once

FS_NAMESPACE_BEGIN

inline char *FS_NetBuffer::GetData()
{
    return _buff;
}

inline std::list<FS_NetBuffer *>::iterator &FS_NetBuffer::GetNode()
{
    return _arrayNode;
}

inline FS_NetBufferArray *FS_NetBuffer::GetOwner()
{
    return _owner;
}

inline void FS_NetBuffer::SetNode(const std::list<FS_NetBuffer *>::iterator &iterNode)
{
    _arrayNode = iterNode;
}

inline bool FS_NetBuffer::NeedWrite() const
{
    return _lastPos > 0;
}

inline bool FS_NetBuffer::IsFull() const
{
    return _lastPos >= _buffSize;
}

inline bool FS_NetBuffer::IsEmpty() const
{
    return _lastPos <= 0;
}

inline void FS_NetBuffer::Release()
{
    FsDelete(this);
}

FS_NAMESPACE_END

#endif
