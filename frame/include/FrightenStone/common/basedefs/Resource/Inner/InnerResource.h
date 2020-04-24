/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
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
 * @file  : InnerResource.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *    只能放第三方资源，或者标准库，不能将自己的资源包含在内
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_BaseDefs_Resource_Inner_InnerResource_H__
#define __Frame_Include_FrightenStone_Common_BaseDefs_Resource_Inner_InnerResource_H__


#pragma once

#ifdef __FRIGHTEN_STONE_FRAME_DLL__

// #pragma region OpenSSL
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

#pragma region create folder
#ifdef _WIN32
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
#endif
#pragma endregion

#endif

#endif // !__Frame_Include_FrightenStone_Common_BaseDefs_Resource_Inner_InnerResource_H__
