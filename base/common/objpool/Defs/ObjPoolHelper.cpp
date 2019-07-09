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
 * @file  : ObjPoolHelper.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/objpool/Defs/ObjPoolHelper.h"
#include "base/common/objpool/Interface/IObjPool.h"

FS_NAMESPACE_BEGIN

ObjPoolHelper::ObjPoolHelper(size_t objSize, size_t objAmount, const Byte8 *objName)
    :_objName{0}
    ,_objPool(NULL)
{
    // objName
    auto len = sprintf(_objName, "%s", objName);
    if(len < 0)
        _objName[0] = 0;
    else
    {
        _objName[len >= BUFFER_LEN256 ? (BUFFER_LEN256 - 1) : len] = 0;
    }

    // ¶ÔÏó³Ø
    _objPool = IObjPool::GetInstance(objSize, objAmount);
    _objPool->InitPool();
}

ObjPoolHelper::~ObjPoolHelper()
{
    if(_objPool)
        _objPool->FinishPool();
    Fs_SafeFree(_objPool);
}

void *ObjPoolHelper::Alloc(size_t bytes)
{
    _objPool->Lock();
    auto ptr = _objPool->Alloc(bytes, _objName);
    _objPool->Unlock();
    return ptr;
}

void ObjPoolHelper::Free(void *ptr)
{
    _objPool->Lock();
    _objPool->Free(ptr);
    _objPool->Unlock();
}

void ObjPoolHelper::AddRef(void *ptr)
{
    _objPool->Lock();
    _objPool->AddRef(ptr);
    _objPool->Unlock();
}

FS_NAMESPACE_END
