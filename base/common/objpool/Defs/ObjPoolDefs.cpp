#include "stdafx.h"
#include "base/common/objpool/Defs/ObjPoolDefs.h"

FS_NAMESPACE_BEGIN

const Int32 ObjPoolDefs::__g_BusyThresholdValue = 70;           // 释放内存时判断内存块利用率低于__g_BusyThresholdValue百分数时转为可用区内存

FS_NAMESPACE_END