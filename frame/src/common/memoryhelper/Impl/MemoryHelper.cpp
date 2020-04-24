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
 * @file  : MemoryHelper.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/01
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/memoryhelper/Impl/MemoryHelper.h"
#include "FrightenStone/common/assist/assistobjs/Impl/Singleton.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/memorypool/memorypool.h"

fs::MemoryHelper *g_MemoryHelper = NULL;

FS_NAMESPACE_BEGIN
Locker MemoryHelper::_locker;
MemoryHelper::~MemoryHelper()
{
    Finish();
}

MemoryHelper *MemoryHelper::GetInstance()
{
    if(!g_MemoryHelper)
        g_MemoryHelper = Singleton<MemoryHelper>::GetInstance();

    return g_MemoryHelper;
}

void MemoryHelper::Start(UInt64 maxObjPoolBytesLimit, UInt64 maxGlobalMemoryPoolBytesLimit)
{
    if(_isStart)
        return;

    _maxObjPoolBytesLimit = maxObjPoolBytesLimit;
    for(auto &callback : _objPoolSetMaxAllowOccupiedBytes)
        callback->Invoke(maxObjPoolBytesLimit);

    g_MemoryPool->SetMaxAllowOccupiedBytes(maxGlobalMemoryPoolBytesLimit);
    _isStart = true;
}

void MemoryHelper::Finish()
{
    if(!_isStart)
        return;

    _isStart = false;
    _locker.Lock();
    STLUtil::DelListContainer(_objPoolSetMaxAllowOccupiedBytes);
    _locker.Unlock();
}

void MemoryHelper::RegisterObjPoolModifyMaxAllowBytesCallback(IDelegate<void, UInt64> *callback)
{
    if(_isStart)
    {
        callback->Invoke(_maxObjPoolBytesLimit);
        FS_Release(callback);
        return;
    }

    _locker.Lock();
    _objPoolSetMaxAllowOccupiedBytes.push_back(callback);
    _locker.Unlock();
}

FS_NAMESPACE_END
