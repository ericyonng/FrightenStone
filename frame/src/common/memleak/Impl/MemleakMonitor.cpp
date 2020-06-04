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
 * @file  : MemleakMonitor.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/07/31
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/memleak/Impl/MemleakMonitor.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/assist/utils/utils.h"

#include "FrightenStone/common/net/net.h"

// static std::mutex g_MemleakMonitorGuard;
//std::atomic<fs::MemleakMonitor *> g_MemleakMonitor = { NULL };

FS_NAMESPACE_BEGIN

MemleakMonitor::MemleakMonitor()
    :_printInfoPool(new FS_ThreadPool(0, 1))
    ,_printInfoIntervalMilliSec(1)
{
    
}

MemleakMonitor::~MemleakMonitor()
{
    _printInfoPool->Close();
    Fs_SafeFree(_printInfoPool);
    STLUtil::DelMapContainer(_objNameRefPrintCallback);
    _threadIdRefMemPoolPrintCallbacks.clear();
}

MemleakMonitor *MemleakMonitor::GetInstance()
{
    static std::mutex g_mutex;
    std::lock_guard<std::mutex> lck(g_mutex);
    static MemleakMonitor *monitor = new MemleakMonitor;

    return monitor;
}

Int32 MemleakMonitor::BeforeStart(UInt32 printIntervalSeconds)
{
    // TODO:monitor是通用的对象不可与服务端配置耦合
//     for(auto &setInvoke : _objPoolSetMaxAllowOccupiedBytes)
//         setInvoke->Invoke(maxAllowObjPoolBytes);
// 
//     g_MemoryPool->SetMaxAllowOccupiedBytes(maxAllowMemoryPoolBytes);
    _printInfoIntervalMilliSec = printIntervalSeconds * static_cast<UInt32>(Time::_millisecondPerSecond);
    return StatusDefs::Success;
}

void MemleakMonitor::Start()
{
    _printInfoPool->AddTask(DelegatePlusFactory::Create(this, &MemleakMonitor::_PrintInfoPerSeconds));
}

void MemleakMonitor::Finish()
{
    _printInfoPool->Close();
    Fs_SafeFree(_printInfoPool);
    STLUtil::DelMapContainer(_objNameRefPrintCallback);
    STLUtil::DelListContainer(_objPoolSetMaxAllowOccupiedBytes);
    _threadIdRefMemPoolPrintCallbacks.clear();
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

void MemleakMonitor::RegisterMemPoolPrintCallback(UInt64 threadId, const IDelegate<void> *callback)
{
    _locker.Lock();
    auto iterCallbacks = _threadIdRefMemPoolPrintCallbacks.find(threadId);
    if(iterCallbacks == _threadIdRefMemPoolPrintCallbacks.end())
        iterCallbacks = _threadIdRefMemPoolPrintCallbacks.insert(std::make_pair(threadId, std::set<const IDelegate<void> *>())).first;
    iterCallbacks->second.insert(callback);
    _locker.Unlock();
}

void MemleakMonitor::UnRegisterMemPoolPrintCallback(UInt64 threadId, const IDelegate<void> *callback)
{
    _locker.Lock();
    auto iterCallbacks = _threadIdRefMemPoolPrintCallbacks.find(threadId);
    if(iterCallbacks != _threadIdRefMemPoolPrintCallbacks.end())
        iterCallbacks->second.erase(callback);
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
#ifdef _WIN32

    // 系统内存情况
    g_Log->mempool("TotalPhysMemSize[%llu];AvailPhysMemSize[%llu] mem use rate[MemoryLoad:[%llu]]"
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
#else
    // TODO:linux
#endif

}

void MemleakMonitor::PrintPoolAll() const
{
    // 系统内存信息
    PrintSysMemoryInfo();
    // 内存池信息
    for(auto &iterCallback : _threadIdRefMemPoolPrintCallbacks)
    {
        auto &callbacks = iterCallback.second;
        for(auto &callback : callbacks)
            callback->Invoke();
    }

    // 打印对象池
    PrintObjPoolInfo();
}

void MemleakMonitor::_PrintInfoPerSeconds(FS_ThreadPool *pool)
{
    while(pool->IsPoolWorking())
    {
        SystemUtil::Sleep(_printInfoIntervalMilliSec);
        _locker.Lock();
        PrintPoolAll();
        _locker.Unlock();
    }
}

FS_NAMESPACE_END
