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
 * @file  : AlloctorListImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/07/25
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_ObjPool_Defs_AlloctorNode_H__
#pragma once

FS_NAMESPACE_BEGIN

template<typename ObjType>
const size_t AlloctorNode<ObjType>::_objBlockSize = __FS_MEMORY_ALIGN__(sizeof(ObjType)) + sizeof(ObjBlock<ObjType>);

template<typename ObjType>
inline AlloctorNode<ObjType>::AlloctorNode(size_t capacity)
    : _objs(::malloc(capacity*_objBlockSize))
    ,_nodeSize(capacity*_objBlockSize)
    ,_nextNode(NULL)
{
    g_curObjPoolOccupiedBytes += _nodeSize;
}

template<typename ObjType>
inline AlloctorNode<ObjType>::~AlloctorNode()
{
    if(_objs)
    {
        g_curObjPoolOccupiedBytes -= _nodeSize;
        ::free(_objs);
    }
}

FS_NAMESPACE_END

#endif

