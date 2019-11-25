#ifndef __Base_Common_ObjPool_Defs_FS_ObjPoolDefs_H__
#define __Base_Common_ObjPool_Defs_FS_ObjPoolDefs_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/assist/assistobjs/Impl/Singleton.h"

// 控制对象池分配的内存
FS_NAMESPACE_BEGIN
extern BASE_EXPORT std::atomic<UInt64> g_curObjPoolOccupiedBytes;
FS_NAMESPACE_END

// #define g_CharPool fs::Singleton<fs::ObjPoolHelper<char>>::GetInstance(__DEF_OBJ_POOL_OBJ_NUM__)

#endif
