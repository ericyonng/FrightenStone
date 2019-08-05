#ifndef __Base_Common_ObjPool_Defs_FS_ObjPoolDefs_H__
#define __Base_Common_ObjPool_Defs_FS_ObjPoolDefs_H__
#pragma once
#include "base/common/assist/assistobjs/Impl/Singleton.h"

#define g_CharPool fs::Singleton<fs::ObjPoolHelper<char>>::GetInstance(__DEF_OBJ_POOL_OBJ_NUM__)

#endif
