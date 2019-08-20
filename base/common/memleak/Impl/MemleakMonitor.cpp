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
 * @file  : MemleakMonitor.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/07/31
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/memleak/Impl/MemleakMonitor.h"
#include "base/common/log/Log.h"
#include "base/common/component/Impl/FS_ThreadPool.h"

fs::MemleakMonitor *g_MemleakMonitor = NULL;


FS_NAMESPACE_BEGIN

Locker MemleakMonitor::_locker;
MemleakMonitor::MemleakMonitor()
    :_memPoolPrintCallback(NULL)
    ,_printInfoPool(new FS_ThreadPool(0, 1))
{
    _printInfoPool->AddTask(DelegatePlusFactory::Create(this, &MemleakMonitor::_PrintInfoPerSeconds));
}

MemleakMonitor::~MemleakMonitor()
{
    _printInfoPool->Clear();
    Fs_SafeFree(_printInfoPool);
}

MemleakMonitor *MemleakMonitor::GetInstance()
{
    _locker.Lock();

    if(!g_MemleakMonitor)
        g_MemleakMonitor = new MemleakMonitor;
    _locker.Unlock();

    return g_MemleakMonitor;
}

void MemleakMonitor::RegisterObjPoolCallback(const char *name, IDelegatePlus<size_t, Int64 &> *callback)
{
    _locker.Lock();
    auto iterCallbacks = _objNameRefPrintCallback.find(name);
    if(iterCallbacks == _objNameRefPrintCallback.end())
        iterCallbacks = _objNameRefPrintCallback.insert(std::make_pair(name, new std::vector<IDelegatePlus<size_t, Int64 &> *>)).first;
    iterCallbacks->second->push_back(callback);
    _locker.Unlock();
}

void MemleakMonitor::UnRegisterObjPool(const char *name)
{
    _locker.Lock();
    auto iterCallbacks = _objNameRefPrintCallback.find(name);
    if(iterCallbacks == _objNameRefPrintCallback.end())
    {
        _locker.Unlock();
        return;
    }

    for(auto &callback : *iterCallbacks->second)
        callback->Release();

    Fs_SafeFree(iterCallbacks->second);
    _objNameRefPrintCallback.erase(iterCallbacks);
    _locker.Unlock();
}

void MemleakMonitor::RegisterMemPoolPrintCallback(const IDelegatePlus<void> *callback)
{
    _locker.Lock();
    _memPoolPrintCallback = callback;
    _locker.Unlock();
}

void MemleakMonitor::UnRegisterMemPoolPrintCallback()
{
    _locker.Lock();
    _memPoolPrintCallback = NULL;
    _locker.Unlock();
}

void MemleakMonitor::PrintObjPoolInfo() const
{
    size_t totalPoolInUsedBytes = 0;
    Int64 totalOccupiedBytes = 0;
    for(auto &iterCallbacks : _objNameRefPrintCallback)
    {
        for(auto &callback : *iterCallbacks.second)
        {
            Int64 curOccupied = 0;
            totalPoolInUsedBytes += (*callback)(curOccupied);
            totalOccupiedBytes += curOccupied;
        }
    }

    // 打印内存泄漏
    g_Log->objpool("objpool: total total pool in used bytes[%llu] total pool occupied bytes[%lld]"
                   , totalPoolInUsedBytes, totalOccupiedBytes);
    g_Log->objpool("=========================================================");

    // 打印系统信息
//     g_Log->sys<MemleakMonitor>(_LOGFMT_("objpool: total total pool in used bytes[%llu] total pool occupied bytes[%lld]")
//                , totalPoolInUsedBytes, totalOccupiedBytes);
}

void MemleakMonitor::PrintObjPoolInfo(const char *objName) const
{
    size_t totalPoolInUsedBytes = 0;
    Int64 totalOccupiedBytes = 0;
    auto iterCallBacks = _objNameRefPrintCallback.find(objName);
    if(iterCallBacks == _objNameRefPrintCallback.end())
        return;

    for(auto &callback : *iterCallBacks->second)
    {
        Int64 curOccupied = 0;
        totalPoolInUsedBytes += (*callback)(curOccupied);
        totalOccupiedBytes += curOccupied;
    }

    // 打印内存泄漏
    g_Log->objpool("objpool objname[%s]:  total total pool in used bytes[%llu] total pool occupied bytes[%lld]"
                   ,objName, totalPoolInUsedBytes, totalOccupiedBytes);

    // 打印系统信息
//     g_Log->sys<MemleakMonitor>(_LOGFMT_("objpool objname[%s]:  total total pool in used bytes[%llu] total pool occupied bytes[%lld]")
//                ,objName, totalPoolInUsedBytes, totalOccupiedBytes);
}


void MemleakMonitor::PrintPoolAll() const
{
    // 打印对象池
    PrintObjPoolInfo();
    if(_memPoolPrintCallback)
        (*_memPoolPrintCallback)();
}

void MemleakMonitor::_PrintInfoPerSeconds(const FS_ThreadPool *pool)
{
    while(!pool->IsClearingPool())
    {
        Sleep(1000);
        _locker.Lock();
        PrintPoolAll();
        _locker.Unlock();
    }
}

FS_NAMESPACE_END
