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
#include "base/common/assist/utils/utils.h"

#include "base/common/net/net.h"

fs::MemleakMonitor *g_MemleakMonitor = fs::MemleakMonitor::GetInstance();


FS_NAMESPACE_BEGIN

Locker MemleakMonitor::_locker;
MemleakMonitor::MemleakMonitor()
    :_printInfoPool(new FS_ThreadPool(0, 1))
{
    
}

MemleakMonitor::~MemleakMonitor()
{
    _printInfoPool->Close();
    Fs_SafeFree(_printInfoPool);
    STLUtil::DelMapContainer(_objNameRefPrintCallback);
    _threadIdRefMemPoolPrintCallback.clear();
}

MemleakMonitor *MemleakMonitor::GetInstance()
{
    _locker.Lock();

    if(!g_MemleakMonitor)
        g_MemleakMonitor = new MemleakMonitor;
    _locker.Unlock();

    return g_MemleakMonitor;
}

void MemleakMonitor::Start()
{
    auto maxBytes = g_SvrCfg->GetMaxAllowObjPoolBytesOccupied();
    for(auto &setInvoke : _objPoolSetMaxAllowOccupiedBytes)
        setInvoke->Invoke(maxBytes);

    g_MemoryPool->SetMaxAllowOccupiedBytes(g_SvrCfg->GetMaxAllowMemoryPoolBytesOccupied());
    _printInfoPool->AddTask(DelegatePlusFactory::Create(this, &MemleakMonitor::_PrintInfoPerSeconds));
}

void MemleakMonitor::Finish()
{
    _printInfoPool->Close();
    Fs_SafeFree(_printInfoPool);
    STLUtil::DelMapContainer(_objNameRefPrintCallback);
    STLUtil::DelListContainer(_objPoolSetMaxAllowOccupiedBytes);
    _threadIdRefMemPoolPrintCallback.clear();
}

void MemleakMonitor::RegisterObjPoolCallback(const char *name, IDelegate<size_t, Int64 &, Int64 &, const char *> *callback)
{
    _locker.Lock();
    auto iterCallbacks = _objNameRefPrintCallback.find(name);
    if(iterCallbacks == _objNameRefPrintCallback.end())
        iterCallbacks = _objNameRefPrintCallback.insert(std::make_pair(name, new std::vector<IDelegate<size_t, Int64 &, Int64 &, const char *> *>)).first;
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

void MemleakMonitor::RegisterMemPoolPrintCallback(Int32 threadId, const IDelegate<void> *callback)
{
    _locker.Lock();
    _threadIdRefMemPoolPrintCallback.insert(std::make_pair(threadId, callback));
    _locker.Unlock();
}

void MemleakMonitor::UnRegisterMemPoolPrintCallback(Int32 threadId)
{
    _locker.Lock();
    _threadIdRefMemPoolPrintCallback.erase(threadId);
    _locker.Unlock();
}

void MemleakMonitor::RegisterObjPoolModifyMaxAllowBytesCallback(IDelegate<void, UInt64> *callback)
{
    _locker.Lock();
    _objPoolSetMaxAllowOccupiedBytes.push_back(callback);
    _locker.Unlock();
}

void MemleakMonitor::PrintObjPoolInfo() const
{
    size_t totalPoolInUsedBytes = 0;
    Int64 totalOccupiedBytes = 0;
    Int64 totalObjInUsedCnt = 0;
    const char *extStr = "";
    for(auto &iterCallbacks : _objNameRefPrintCallback)
    {
        for(auto &callback : *iterCallbacks.second)
        {
            Int64 curOccupied = 0;
            Int64 objInUsed = 0;
            totalPoolInUsedBytes += callback->Invoke(curOccupied, objInUsed, extStr);
            totalOccupiedBytes += curOccupied;
            totalObjInUsedCnt += objInUsed;
        }
    }

    if(g_curObjPoolOccupiedBytes != totalOccupiedBytes)
        g_curObjPoolOccupiedBytes = totalOccupiedBytes;

    // 打印内存泄漏
    g_Log->objpool("objpool[curObjPoolOccupiedBytes[%llu]]: total total pool in used bytes[%llu] totalObjInUsedCnt[%lld] total pool occupied bytes[%lld]"
                   ,(UInt64)(g_curObjPoolOccupiedBytes),  totalPoolInUsedBytes, totalObjInUsedCnt, totalOccupiedBytes);
    g_Log->objpool("=========================================================");

    // 打印系统信息
//     g_Log->sys<MemleakMonitor>(_LOGFMT_("objpool: total total pool in used bytes[%llu] total pool occupied bytes[%lld]")
//                , totalPoolInUsedBytes, totalOccupiedBytes);
}

void MemleakMonitor::PrintObjPoolInfo(const char *objName) const
{
    size_t totalPoolInUsedBytes = 0;
    Int64 totalOccupiedBytes = 0;
    Int64 totalObjInUsedCnt = 0;
    auto iterCallBacks = _objNameRefPrintCallback.find(objName);
    if(iterCallBacks == _objNameRefPrintCallback.end())
        return;

    const char *extStr = "";
    for(auto &callback : *iterCallBacks->second)
    {
        Int64 curOccupied = 0;
        Int64 objInUsed = 0;
        totalPoolInUsedBytes += callback->Invoke(curOccupied, objInUsed, extStr);
        totalOccupiedBytes += curOccupied;
        totalObjInUsedCnt += objInUsed;
    }

    // 打印内存泄漏
    g_Log->objpool("objpool objname[%s]:  total total pool in used bytes[%llu] totalObjInUsedCnt[%lld] total pool occupied bytes[%lld]"
                   ,objName, totalPoolInUsedBytes, totalObjInUsedCnt, totalOccupiedBytes);

    // 打印系统信息
//     g_Log->sys<MemleakMonitor>(_LOGFMT_("objpool objname[%s]:  total total pool in used bytes[%llu] total pool occupied bytes[%lld]")
//                ,objName, totalPoolInUsedBytes, totalOccupiedBytes);
}


void MemleakMonitor::PrintSysMemoryInfo() const
{
    // 系统内存情况
    g_Log->mempool("TotalPhysMemSize[%llu];AvailPhysMemSize[%llu] mem use rate[MemoryLoad:[%lu]]"
                   , SystemUtil::GetTotalPhysMemSize()
                   , SystemUtil::GetAvailPhysMemSize()
                   , SystemUtil::GetMemoryLoad());

    ProcessMemInfo memInfo = {};
    g_Log->mempool("process occupied memory info:");
    if(SystemUtil::GetProcessMemInfo(SystemUtil::GetCurProcessHandle(), memInfo))
    {
        g_Log->mempool("max historysetsize in bytes[%llu], cur set size in bytes[%llu];\n"
                       "\tmaxHistoryPagedPoolUsage in bytes[%llu],cur paged pool usage in bytes[%llu];\n"
                       "\tmax history non paged pool usage in bytes[%llu], cur non paged pool usage in bytes[%llu];\n"
                       "\tmax history page file usage in bytes[%llu], cur page file usage in bytes[%llu];\n"
                       "\tcur process allocating memory usage in bytes[%llu];"
                       , memInfo._maxHistorySetSize, memInfo._curSetSize
                       , memInfo._maxHistoryPagedPoolUsage, memInfo._pagedPoolUsage
                       , memInfo._maxHistoryNonPagedPoolUsage, memInfo._curNonPagedPoolUsage
                       , memInfo._maxHistoryPageFileUsage, memInfo._curPageFileUsage
                       , memInfo._processAllocMemoryUsage);
    }
    else
    {
        g_Log->mempool("fail get process mem info");
    }
}

void MemleakMonitor::PrintPoolAll() const
{
    // 系统内存信息
    PrintSysMemoryInfo();
    // 内存池信息
    for(auto &iterCallback : _threadIdRefMemPoolPrintCallback)
    {
        auto callback = iterCallback.second;
        callback->Invoke();
    }

    // 打印对象池
    PrintObjPoolInfo();
}

void MemleakMonitor::_PrintInfoPerSeconds(FS_ThreadPool *pool)
{
    while(pool->IsPoolWorking())
    {
        Sleep(1000);
        _locker.Lock();
        PrintPoolAll();
        _locker.Unlock();
    }
}

FS_NAMESPACE_END
