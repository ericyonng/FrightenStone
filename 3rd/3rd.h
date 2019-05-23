#ifndef __3rd_3rd_H__
#define __3rd_3rd_H__
#pragma once

#ifdef __USE_FS_3RD_JSON___
#pragma region Json
#include "3rd/json/single_include/nlohmann/json.hpp"
#pragma endregion
#endif

#ifdef __USE_FS_3RD_OPENSSL__
#pragma region openssl
// include header
#ifdef _DEBUG
#include "3rd/openssl/staticlib/debug/include/openssl/md5.h"
#else
#include "3rd/openssl/staticlib/release/include/openssl/md5.h"
#endif

// lib static lib
#ifdef _DEBUG
#pragma comment(lib, "staticlib\\debug\\lib\\libeay32.lib")
#pragma comment(lib, "staticlib\\debug\\lib\\ssleay32.lib")
#else
#pragma comment(lib, "staticlib\\release\\lib\\libeay32.lib")
#pragma comment(lib, "staticlib\\release\\lib\\ssleay32.lib")
#endif
#pragma endregion
#endif

#endif

