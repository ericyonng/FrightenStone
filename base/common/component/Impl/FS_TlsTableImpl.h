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
 * @file  : FS_TlsTableImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_FS_TlsTable_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_TlsTable::FS_TlsTable()
{
}

template<typename ObjType>
inline ObjType *FS_TlsTable::GetElement(Int32 type)
{
    auto iterElement = _elementTypeRefElements.find(type);
    if(iterElement == _elementTypeRefElements.end())
        return NULL;

    return static_cast<ObjType *>(iterElement->second);
}

template<typename ObjType, typename... Args>
inline ObjType *FS_TlsTable::AllocElement(Int32 type, Args... args)
{
    auto iterElement = _elementTypeRefElements.find(type);
    if(iterElement == _elementTypeRefElements.end())
    {
        ITlsBase *newObj = new ObjType(std::forward<Args>(args)...);
        iterElement = _elementTypeRefElements.insert(std::make_pair(type, newObj)).first;
    }

    return static_cast<ObjType *>(iterElement->second);
}

FS_NAMESPACE_END

#endif
