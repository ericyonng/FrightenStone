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
 * @file  : ForAll.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_BaseDefs_DataType_ForAll_ForAll_H__
#define __Frame_Include_FrightenStone_Common_BaseDefs_DataType_ForAll_ForAll_H__


#pragma once

// defs ...
#include "FrightenStone/common/basedefs/DataType/ForAll/Defs/NetDataType.h"

#pragma region base data type
typedef char Byte8;
typedef unsigned char U8;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
typedef long Long;
typedef unsigned long ULong;
typedef long long Int64;
typedef unsigned long long UInt64;
typedef float Float;
typedef double Double;
#pragma endregion

// cache
#define BUFFER_LEN4         4
#define BUFFER_LEN8         8
#define BUFFER_LEN16        16
#define BUFFER_LEN32        32
#define BUFFER_LEN64        64
#define BUFFER_LEN128       128
#define BUFFER_LEN256       256
#define BUFFER_LEN512       512
#define BUFFER_LEN1024      1024
typedef Byte8 BUFFER8[BUFFER_LEN8];
typedef Byte8 BUFFER16[BUFFER_LEN16];
typedef Byte8 BUFFER32[BUFFER_LEN32];
typedef Byte8 BUFFER64[BUFFER_LEN64];
typedef Byte8 BUFFER128[BUFFER_LEN128];
typedef Byte8 BUFFER256[BUFFER_LEN256];
typedef Byte8 BUFFER512[BUFFER_LEN512];
typedef Byte8 BUFFER1024[BUFFER_LEN1024];

class FileCursorOffsetType
{
public:
    enum FILE_CURSOR_POS
    {
        FILE_CURSOR_POS_SET = 0,    // 起始位置
        FILE_CURSOR_POS_CUR = 1,    // 当前位置
        FILE_CURSOR_POS_END = 2,    // 文件末尾
    };
};

#ifndef _WIN32
typedef void* HANDLE;
#define MAX_PATH  PATH_MAX

#endif

#endif // !__Frame_Include_FrightenStone_Common_BaseDefs_DataType_InnerDataType_InnerDataType_H__
