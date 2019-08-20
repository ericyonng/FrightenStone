#include "stdafx.h"
#include "base/common/objpool/Defs/ObjPoolDefs.h"
#include "base/common/log/Log.h"
#include "base/common/memleak/memleak.h"

FS_NAMESPACE_BEGIN

const Int32 ObjPoolDefs::__g_FreeRate = 50;           // 释放内存时判断内存块利用率低于__g_BusyThresholdValue百分数时转为可用区内存

void ObjPoolMethods::PrintObjPoolInfo(const char *objName, size_t nodeCnt, size_t totalObjBlocks, size_t bytesOccupied, size_t memleakObjCnt, size_t memleakBytes)
{
    if(memleakBytes)
    {// 内存泄漏打印内存泄漏
        g_Log->objpool("obj name[%s], pool node cnt[%llu] totalObjBlocks[%llu] pool memory bytes occupied[%llu]"
                       " memleak obj cnt[%llu] memleak bytes[%llu]"
                       , objName
                       , nodeCnt
                       , totalObjBlocks
                       , bytesOccupied
                       , memleakObjCnt
                       , memleakBytes);
    }
    
    // 打印内存占用信息
    g_Log->objpool("[objpool memory info]: obj name[%s], pool node cnt[%llu] totalObjBlocks[%llu] pool memory bytes occupied[%llu]"
               , objName
               , nodeCnt
               , totalObjBlocks
               , bytesOccupied);
}

void ObjPoolMethods::RegisterToMemleakMonitor(const char *objName, IDelegatePlus<size_t, Int64 &> *callback)
{
    // TODO
    if(!g_MemleakMonitor)
        fs::MemleakMonitor::GetInstance();
    g_MemleakMonitor->RegisterObjPoolCallback(objName, callback);
}

void ObjPoolMethods::UnRegisterMemleakDelegate(const char *objName)
{
    // TODO
    if(!g_MemleakMonitor)
        fs::MemleakMonitor::GetInstance();

    g_MemleakMonitor->UnRegisterObjPool(objName);
}

FS_NAMESPACE_END

