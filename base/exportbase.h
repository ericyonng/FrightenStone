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
 * @file  : exportbase.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __FRIGHTEN_STONE_BASE_EXPORT_BASE_H__
#define __FRIGHTEN_STONE_BASE_EXPORT_BASE_H__

#pragma once
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _WIN32


// 导出定义
#ifndef BASE_EXPORT
    #ifdef  FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL
        #ifdef _WIN32
            #define BASE_EXPORT _declspec(dllexport)
        #else
            #define BASE_EXPORT
        #endif
    #else
        #ifdef _WIN32
            #define BASE_EXPORT _declspec(dllimport)
        #else
            #define BASE_EXPORT 
        #endif
    #endif
#endif

#pragma region Warning Disable
#pragma warning(disable:4251)               // 模版类造成的警告
#ifdef _WIN32
#pragma warning (disable:4819)              // 屏蔽文件中存在汉字时的编码警告
#endif
#define D_SCL_SECURE_NO_WARNINGS            // disable warning C4996
#pragma endregion

#include <base/common/common.h>

#endif
