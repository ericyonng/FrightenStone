#ifndef __Base_Common_BaseDefs_DataType_InnerDataType_Defs_LockerHandle_H_
#define __Base_Common_BaseDefs_DataType_InnerDataType_Defs_LockerHandle_H_
/**
* @file LockerHandle.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/25
* @brief
*/
#pragma once

#ifdef _WIN32
#include "windows.h"
typedef CRITICAL_SECTION LockerHandle;
#endif

#endif // !__Base_Common_BaseDefs_DataType_InnerDataType_Defs_LockerHandle_H_
