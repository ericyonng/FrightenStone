#ifndef __Base_Common_BaseDefs_Resource_Inner_InnerResource_H__
#define __Base_Common_BaseDefs_Resource_Inner_InnerResource_H__
/**
* @file InnerResource.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/25
* @brief
*/

#pragma once

#ifdef FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL

// #pragma region OpenSSL支持
// #ifdef _DEBUG
// #include "3rd/openssl/staticlib/debug/include/openssl/md5.h"
// #pragma comment(lib, "staticlib\\debug\\lib\\libeay32.lib")
// #pragma comment(lib, "staticlib\\debug\\lib\\ssleay32.lib")
// #else
// #include "3rd/openssl/staticlib/release/include/openssl/md5.h"
// #pragma comment(lib, "staticlib\\release\\lib\\libeay32.lib")
// #pragma comment(lib, "staticlib\\release\\lib\\ssleay32.lib")
// #endif
// #pragma endregion

#pragma region 创建文件夹支持
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
#pragma endregion

#endif

#endif // !__Base_Common_BaseDefs_Resource_Inner_InnerResource_H__
