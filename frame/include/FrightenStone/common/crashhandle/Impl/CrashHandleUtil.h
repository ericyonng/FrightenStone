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
 * @file  : CrashHandleUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/2
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_CrashHandle_Impl_CrashHandleUtil_H__
#define __Frame_Include_FrightenStone_Common_CrashHandle_Impl_CrashHandleUtil_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

#ifdef _WIN32

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
typedef struct _EXCEPTION_RECORD EXCEPTION_RECORD, *PEXCEPTION_RECORD;
typedef struct _CONTEXT CONTEXT, *PCONTEXT;

#define GET_EXCEPTION_INFO() ((PEXCEPTION_POINTERS)(GetExceptionInformation()))

#endif

FS_NAMESPACE_BEGIN

class FS_String;
struct LogData;

class BASE_EXPORT CrashHandleUtil
{
public:
    // 初始化crashdump信息 isUseSehExceptionHandler是外部手动加了__try __except的seh
    static int InitCrashHandleParams(bool isUseSehExceptionHandler = false);

    #ifdef _WIN32
    // 配合__except见范例
    static Int32 RecordExceptionInfo(EXCEPTION_POINTERS exceptionInfo);
    #endif

    // 初始化pdb等符号信息 用于打印堆栈信息
    static Int32 InitSymbol();
    // 抓取堆栈快照 主动打印堆栈信息
    static FS_String FS_CaptureStackBackTrace(size_t skipFrames = 0, size_t captureFrames = FS_INFINITE);

protected:
    static void _OnBeforeCrashLogHook(LogData *logData);
    static void _OnAfterCrashLogHook(const LogData *logData);
};

FS_NAMESPACE_END

#endif
