#ifndef __FRIGHTEN_STONE_BASE_EXPORT_BASE_H__
#define __FRIGHTEN_STONE_BASE_EXPORT_BASE_H__
/**
* @file exportbase.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief 导出接口文件
*/

#pragma once

//导出定义
#ifndef BASE_EXPORT
    #ifdef  FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL
        #define BASE_EXPORT _declspec(dllexport)
    #else
        #define BASE_EXPORT _declspec(dllimport)
    #endif
#endif

//定义文件名
#ifdef _DEBUG
    #define FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL_NAME                "baseD.dll"    //
#else
    #define FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL_NAME                "base.dll"    //
#endif

#pragma region Warning Disable
#pragma warning(disable:4251)               // 
#define D_SCL_SECURE_NO_WARNINGS            // disable warning C4996
#pragma endregion

#include <base/common/common.h>
#ifndef FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL
    #ifdef _DEBUG
        #pragma comment(lib, "baseD.lib")
    #else
        #pragma comment(lib, "base.lib")
    #endif
#endif

#endif
