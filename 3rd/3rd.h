/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * filename: 3rd.h
 *
 * author  : Eric Yonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
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
#ifdef _WIN32
    // include header
    #ifdef _DEBUG
        #include "3rd/openssl/staticlib/debug/include/openssl/md5.h"
        #include "3rd/openssl/staticlib/debug/include/openssl/aes.h"
    #else
        #include "3rd/openssl/staticlib/release/include/openssl/md5.h"
        #include "3rd/openssl/staticlib/release/include/openssl/aes.h"
    #endif

    // lib static lib
    #ifdef _DEBUG
        #pragma comment(lib, "staticlib\\debug\\lib\\libeay32.lib")
        #pragma comment(lib, "staticlib\\debug\\lib\\ssleay32.lib")
    #else
        #pragma comment(lib, "staticlib\\release\\lib\\libeay32.lib")
        #pragma comment(lib, "staticlib\\release\\lib\\ssleay32.lib")
    #endif

#else// linux
#include "openssl/md5.h"
#include "openssl/aes.h"
#endif

#pragma endregion
#endif

// 使用utf8_string
#ifdef __USE_FS_TINY_UTF8__
#include "3rd/tiny-utf8/include/tinyutf8.hpp"
#endif

// 使用dbghelp
#ifdef __USE_FS_DBGHELP__
#ifdef _WIN32
    #include <DbgHelp.h>
        #ifdef _DEBUG
            #pragma comment(lib, "DbgHelp\\debug\\DbgHelp.Lib")
        #else
            #pragma comment(lib, "DbgHelp\\release\\DbgHelp.Lib")
        #endif
    #endif
#endif
#endif

