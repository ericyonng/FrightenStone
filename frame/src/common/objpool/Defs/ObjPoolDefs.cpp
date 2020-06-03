#include "stdafx.h"
#include "FrightenStone/common/objpool/Defs/ObjPoolDefs.h"
#include <FrightenStone/common/objpool/Defs/FS_ObjPoolDefs.h>
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/memleak/memleak.h"
#include "FrightenStone/common/memoryhelper/MemoryHelper.h"

FS_NAMESPACE_BEGIN

const Int32 ObjPoolDefs::__g_FreeRate = 50;           // 释放内存时判断内存块利用率低于__g_BusyThresholdValue百分数时转为可用区内存
// static std::mutex g_ObjPoolMethodGuard;

void ObjPoolMethods::PrintObjPoolInfo(const char *objName, size_t nodeCnt, size_t totalObjBlocks, size_t objBlockSize, size_t bytesOccupied, size_t memObjInUsingCnt, size_t memInUsingBytes, const char *extStr)
{
    if(memInUsingBytes)
    {// 内存泄漏打印内存泄漏
        g_Log->objpool("obj name[%s], extStr[%s] pool node cnt[%llu] totalObjBlockCnt[%llu] objBlockSize[%llu] pool memory bytes occupiedBytes[%llu]"
                       " memObjInUsingCnt[%llu] memInUsingBytes[%llu]"
                       , objName
                       , extStr
                       , nodeCnt
                       , totalObjBlocks
                       , objBlockSize
                       , bytesOccupied
                       , memObjInUsingCnt
                       , memInUsingBytes);
    }
    
    // 打印内存占用信息
    g_Log->objpool("[objpool memory info cur total occupied bytes[%llu]]: obj name[%s], extStr[%s], pool node cnt[%llu] totalObjBlockCnt[%llu] objBlockSize[%llu] pool memory bytes occupiedBytes[%llu]"
               , (UInt64)(g_curObjPoolOccupiedBytes)
               , objName
               , extStr
               , nodeCnt
               , totalObjBlocks
               , objBlockSize
               , bytesOccupied);
}

void ObjPoolMethods::RegisterToMemleakMonitor(const char *objName, IDelegate<size_t, Int64 &, Int64 &, const char *> *callback)
{
    if (!g_MemleakMonitor)
        g_MemleakMonitor = new MemleakMonitor;
    g_MemleakMonitor->RegisterObjPoolCallback(objName, callback);
}

void ObjPoolMethods::UnRegisterMemleakDelegate(const char *objName)
{
    if (!g_MemleakMonitor)
        g_MemleakMonitor = new MemleakMonitor;
    g_MemleakMonitor->UnRegisterObjPool(objName);
}

void ObjPoolMethods::RegisterModifyAllowMaxBytes(IDelegate<void, UInt64> *callback)
{
    if (!g_MemoryHelper)
        g_MemoryHelper = new MemoryHelper;
    g_MemoryHelper->RegisterObjPoolModifyMaxAllowBytesCallback(callback);
}

FS_NAMESPACE_END

