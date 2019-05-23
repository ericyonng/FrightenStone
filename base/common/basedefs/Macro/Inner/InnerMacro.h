#ifndef __Base_Common_BaseDefs_Macro_MacroDefs_Inner_InnerMacro_H__
#define __Base_Common_BaseDefs_Macro_MacroDefs_Inner_InnerMacro_H__
/**
* @file InnerMacro.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/25
* @brief
*/

#pragma once

// defs ...
#ifdef _WIN32
#undef SPINNING_COUNT
#define SPINNING_COUNT 8000
#endif

#define THREAD_POOL_WAIT_FOR_COMPLETED_TIME 5     // 5ms
#endif // !__Base_Common_BaseDefs_Macro_MacroDefs_Inner_InnerMacro_H__
