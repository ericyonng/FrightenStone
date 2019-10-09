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
 * @file  : FS_TlsTable.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/assist/utils/Defs/FS_TlsTable.h"
#include "base/common/assist/utils/Defs/ITlsBase.h"
#include "base/common/assist/utils/Impl/STLUtil.h"
#include "base/common/assist/utils/Impl/SystemUtil.h"

FS_NAMESPACE_BEGIN

FS_TlsTable::~FS_TlsTable()
{
    STLUtil::DelMapContainer(_elementTypeRefElements);
}

void FS_TlsTable::Erase(Int32 type)
{
    auto iterElement = _elementTypeRefElements.find(type);
    if(iterElement == _elementTypeRefElements.end())
        return;

    iterElement->second->Release();
    _elementTypeRefElements.erase(iterElement);
}

ThreadTlsTableMgr::~ThreadTlsTableMgr()
{
    STLUtil::DelMapContainer(_threadIdRefTlsTable);
}

FS_TlsTable *ThreadTlsTableMgr::GetThisThreadTable()
{
    const Int32 threadId = static_cast<Int32>(SystemUtil::GetCurrentThreadId());
    auto iterTable = _threadIdRefTlsTable.find(threadId);
    if(iterTable == _threadIdRefTlsTable.end())
        return NULL;

    return iterTable->second;
}

FS_TlsTable *ThreadTlsTableMgr::CreateThisThreadTable()
{
    const Int32 threadId = static_cast<Int32>(SystemUtil::GetCurrentThreadId());

    _guard.Lock();
    auto iterTable = _threadIdRefTlsTable.find(threadId);
    if(iterTable == _threadIdRefTlsTable.end())
        iterTable = _threadIdRefTlsTable.insert(std::make_pair(threadId, new FS_TlsTable())).first;
    FS_TlsTable *newTable = iterTable->second;
    _guard.Unlock();

    return newTable;
}

void ThreadTlsTableMgr::FreeThisThreadTable()
{
    const Int32 threadId = static_cast<Int32>(SystemUtil::GetCurrentThreadId());

    _guard.Lock();
    auto iterTable = _threadIdRefTlsTable.find(threadId);
    if(iterTable == _threadIdRefTlsTable.end())
    {
        _guard.Unlock();
        return;
    }
    FS_TlsTable *tlsTable = iterTable->second;
    _threadIdRefTlsTable.erase(iterTable);
    _guard.Unlock();

    Fs_SafeFree(tlsTable);
}

FS_NAMESPACE_END
