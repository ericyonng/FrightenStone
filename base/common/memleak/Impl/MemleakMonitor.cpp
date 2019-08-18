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

fs::MemleakMonitor *g_MemleakMonitor = NULL;


FS_NAMESPACE_BEGIN

Locker MemleakMonitor::_locker;
MemleakMonitor::MemleakMonitor()
{

}

MemleakMonitor::~MemleakMonitor()
{

}

MemleakMonitor *MemleakMonitor::GetInstance()
{
    _locker.Lock();

    if(!g_MemleakMonitor)
        g_MemleakMonitor = new MemleakMonitor;
    _locker.Unlock();

    return g_MemleakMonitor;
}

void MemleakMonitor::RegisterCallback(const char *name, IDelegatePlus<size_t, Int64 &> *callback)
{
    _locker.Lock();
    auto iterCallbacks = _objNameRefPrintCallback.find(name);
    if(iterCallbacks == _objNameRefPrintCallback.end())
        iterCallbacks = _objNameRefPrintCallback.insert(std::make_pair(name, new std::vector<IDelegatePlus<size_t, Int64 &> *>)).first;
    iterCallbacks->second->push_back(callback);
    _locker.Unlock();
}

void MemleakMonitor::UnRegister(const char *name)
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

void MemleakMonitor::PrintMemleakInfo()
{
    size_t totalMemleakBytes = 0;
    Int64 totalOccupiedBytes = 0;
    for(auto &iterCallbacks : _objNameRefPrintCallback)
    {
        for(auto &callback : *iterCallbacks.second)
        {
            Int64 curOccupied = 0;
            totalMemleakBytes += (*callback)(curOccupied);
            totalOccupiedBytes += curOccupied;
        }
    }

    // 打印内存泄漏
    g_Log->memleak("memleak monitor: total memleak bytes[%llu] total pool occupied bytes[%lld]"
                   , totalMemleakBytes, totalOccupiedBytes);

    // 打印系统信息
    g_Log->sys<MemleakMonitor>(_LOGFMT_("memleak monitor: total memleak bytes[%llu] total pool occupied bytes[%lld]")
               , totalMemleakBytes, totalOccupiedBytes);
}

void MemleakMonitor::PrintMemleakInfo(const char *objName)
{
    size_t totalMemleakBytes = 0;
    Int64 totalOccupiedBytes = 0;
    auto iterCallBacks = _objNameRefPrintCallback.find(objName);
    if(iterCallBacks == _objNameRefPrintCallback.end())
        return;

    for(auto &callback : *iterCallBacks->second)
    {
        Int64 curOccupied = 0;
        totalMemleakBytes += (*callback)(curOccupied);
        totalOccupiedBytes += curOccupied;
    }

    // 打印内存泄漏
    g_Log->memleak("memleak monitor[%s]: memleak bytes[%llu] pool occupied bytes[%lld]"
                   ,objName, totalMemleakBytes, totalOccupiedBytes);

    // 打印系统信息
    g_Log->sys<MemleakMonitor>(_LOGFMT_("memleak monitor[%s]: memleak bytes[%llu] pool occupied bytes[%lld]")
               ,objName, totalMemleakBytes, totalOccupiedBytes);
}
FS_NAMESPACE_END
