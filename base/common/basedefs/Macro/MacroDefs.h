#ifndef __Base_Common_BaseDefs_Macro_MacroDefs_H__
#define __Base_Common_BaseDefs_Macro_MacroDefs_H__
/**
* @file MacroDefs.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

#ifdef FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL

// 内部模块使用
#include "base/common/basedefs/Macro/Inner/InnerMacro.h"

#endif

// 外部模块使用
#include "base/common/basedefs/Macro/Export/ExportMacro.h"

// 两者都可使用
#include "base/common/basedefs/Macro/ForAll/ForAllMacro.h"

#endif
