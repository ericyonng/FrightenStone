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
 * @file  : SystemUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/10
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_SystemUtil_H__
#define __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_SystemUtil_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN
class FS_String;
struct ProcessMemInfo;

class BASE_EXPORT SystemUtil
{
public:
    /* 进程线程 */
    // 获取程序目录
    static Int32 GetProgramPath(bool isCurrentProcess, FS_String &processPath, UInt64 pid = 0);
    // 获取当前进程名
    static FS_String GetCurProgramName();
    // 获取线程id
    static UInt64 GetCurrentThreadId();
    // 获取进程id
    static Int32 GetCurProcessId();
    // 结束进程
    static Int32 CloseProcess(Int32 processId, ULong *lastError = NULL);

    #ifdef _WIN32
    // 获取可用的内存大小
    static UInt64 GetAvailPhysMemSize();
    // 获取内存大小
    static UInt64 GetTotalPhysMemSize();
    // 内存使用率
    static UInt64 GetMemoryLoad();
    // 进程占用内存信息
    static bool GetProcessMemInfo(HANDLE processHandle, ProcessMemInfo &info);

    // 创建进程快照（遍历进程相关）
    static HANDLE CreateProcessSnapshot();
    // 获取第一个进程id
    static UInt64 GetFirstProcessPid(HANDLE &hSnapshot);
    // 获取下一个进程id
    static UInt64 GetNextProcessPid(HANDLE &hSnapshot);
    // 获取进程句柄
    static HANDLE GetCurProcessHandle();
    // 通过进程ID获取窗口句柄
    static HWND GetWindowHwndByPID(DWORD dwProcessID);
    // 将窗口设置顶层
    static void BringWindowsToTop(HWND curWin);
    // 弹窗
    static void MessageBoxPopup(const FS_String &title, const FS_String &content);
    // 获取当前调用线程所在的cpu编号信息
    static void GetCallingThreadCpuInfo(UInt16 &cpuGroup, Byte8 &cpuNumber);
    // 遍历进程判断某进程是否在进程列表
    static bool IsProcessExist(const FS_String &processName);
    #else
    // ...linux
    #endif
    
        /* 控制台 */
    static void LockConsole();
    static void UnlockConsole();
    // 设置控制台颜色
    static Int32 SetConsoleColor(Int32 color);
    // 获取控制台颜色
    static Int32 GetConsoleColor();
    // 输出给控制台
    static void OutputToConsole(const FS_String &outStr);

    /* 杂项 */
    // 大小端判断
    static bool IsLittleEndian();
    // 休眠
    static void Sleep(UInt64 milliSec, UInt64 microSec = 0);
};

FS_NAMESPACE_END

#include "FrightenStone/common/assist/utils/Impl/SystemUtilImpl.h"

#endif
