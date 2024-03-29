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
 * @file  : ThreadTlsTableMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/component/Impl/FS_TlsTable.h"
#include "FrightenStone/common/component/Impl/ThreadTlsTableMgr.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"

FS_NAMESPACE_BEGIN

ThreadTlsTableMgr::~ThreadTlsTableMgr()
{
    STLUtil::DelMapContainer(_threadIdRefTlsTable);
}

FS_TlsTable *ThreadTlsTableMgr::GetThisThreadTableNoLock()
{
    const UInt64 threadId = SystemUtil::GetCurrentThreadId();
    auto iterTable = _threadIdRefTlsTable.find(threadId);
    if(iterTable == _threadIdRefTlsTable.end())
        return NULL;

    return iterTable->second;
}

void ThreadTlsTableMgr::FreeThisThreadTable()
{
    const UInt64 threadId = SystemUtil::GetCurrentThreadId();

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

FS_TlsTable *ThreadTlsTableMgr::_GetThisThreadTable()
{
    const UInt64 threadId = SystemUtil::GetCurrentThreadId();
    auto iterTable = _threadIdRefTlsTable.find(threadId);
    if(iterTable == _threadIdRefTlsTable.end())
        return NULL;

    return iterTable->second;
}

FS_TlsTable *ThreadTlsTableMgr::_CreateThisThreadTable()
{
    const UInt64 threadId = SystemUtil::GetCurrentThreadId();
    auto iterTable = _threadIdRefTlsTable.find(threadId);
    if(iterTable == _threadIdRefTlsTable.end())
        iterTable = _threadIdRefTlsTable.insert(std::make_pair(threadId, new FS_TlsTable())).first;

    return iterTable->second;
}

FS_NAMESPACE_END
