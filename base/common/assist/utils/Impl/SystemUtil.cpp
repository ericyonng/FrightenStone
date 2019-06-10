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
 * @file  : SystemUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/assist/utils/Impl/SystemUtil.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_String.h"

#pragma region windows
#ifdef _WIN32
#include "tchar.h"
#include "Psapi.h"
#include "tlhelp32.h"
#include "process.h"
#include "sysinfoapi.h"
#endif
#pragma endregion

#pragma region defines
// 获取内存状态函数函数原型指针
// typedef   void(WINAPI *__GlobalMemoryStatusExFunc)(LPMEMORYSTATUSEX);

static Int32 GetMemoryStatus(MEMORYSTATUSEX &status)
{
    // 载入动态链接库kernel32.dll，返回它的句柄
//     HMODULE hModule;
//     hModule = LoadLibrary("kernel32.dll");
//     if(UNLIKELY(!hModule))
//         return StatusDefs::SystemUtil_GetKernel32HandleFailed;

    // 在kernel32.dll句柄里查找GlobalMemoryStatusEx函数，获取函数指针
//     __GlobalMemoryStatusExFunc globalMemoryStatusEx = (__GlobalMemoryStatusExFunc)GetProcAddress(hModule, "GlobalMemoryStatusEx");
//     if(UNLIKELY(!globalMemoryStatusEx))
//         return StatusDefs::SystemUtil_GetGlobalMemoryStatusExFuncFailed;

//      globalMemoryStatusEx(&status);
// 
//     // 释放链接库句柄
//     FreeLibrary(hModule);
    // 调用函数取得系统的内存情况
    status.dwLength = sizeof(status);
    if(!GlobalMemoryStatusEx(&status))
        return StatusDefs::SystemUtil_GetGlobalMemoryStatusExFailed;


    return StatusDefs::Success;
}
#pragma endregion

FS_NAMESPACE_BEGIN

#pragma region windowsdefines
typedef struct
{
    HWND    hwndWindow;     // 窗口句柄
    DWORD   dwProcessID;    // 进程ID
}EnumWindowsArg;
#pragma endregion

UInt64 SystemUtil::GetAvailPhysMemSize()
{
    MEMORYSTATUSEX status;
    auto ret = GetMemoryStatus(status);
    if(ret != StatusDefs::Success)
        return 0;

    return status.ullAvailPhys;
}

UInt64 SystemUtil::GetTotalPhysMemSize()
{
    MEMORYSTATUSEX status;
    auto ret = GetMemoryStatus(status);
    if(ret != StatusDefs::Success)
        return 0;

    return status.ullTotalPhys;
}

ULong SystemUtil::GetMemoryLoad()
{
    MEMORYSTATUSEX status;
    auto ret = GetMemoryStatus(status);
    if(ret != StatusDefs::Success)
        return 0;

    return status.dwMemoryLoad;
}

bool SystemUtil::GetProgramPath(bool isCurrentProcess, ULong pid, FS_String &processPath)
{
    bool ret = false;
    HMODULE hModule = NULL;
    HANDLE  hProc = NULL;
    DWORD   dwProcPathLen = 0;
    Byte8   procPathDev[MAX_PATH] = {0};
    Byte8   volNameDev[MAX_PATH] = { 0};
    Byte8   volName[MAX_PATH] = {0};
    Byte8   pathName[MAX_PATH] = {0};

    __try
    {
        if(!isCurrentProcess && !pid)
            __leave;

        if(isCurrentProcess)
        {
            if(!GetModuleFileName(NULL, pathName, MAX_PATH))
                __leave;

            processPath << pathName;
            ret = true;
            __leave;
        }

        hProc = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
        if(!hProc)
            __leave;

        hModule = LoadLibrary(TEXT("Kernel32.dll"));
        if(!hModule)
            __leave;

        // 获取QueryFullProcessImageNameA函数
        if(GetProcAddress(hModule, "QueryFullProcessImageNameA"))
        {
            dwProcPathLen = MAX_PATH / sizeof(Byte8);
            if(!QueryFullProcessImageName(hProc, 0, pathName, &dwProcPathLen))
                __leave;

            processPath << pathName;
            ret = true;
            __leave;
        }

        // 获取进程路径
        if(!GetProcessImageFileName(hProc, procPathDev, MAX_PATH))
            __leave;

        //盘符
        _tcscat_s(volName, MAX_PATH, TEXT("A:"));
        for(; *volName <= _T('Z'); (*volName)++)
        {
            // 获取盘符
            memset(volNameDev, 0, MAX_PATH * sizeof(Byte8));
            if(!QueryDosDevice(volName, volNameDev, MAX_PATH))
            {
                if(GetLastError() == 2)
                    continue;

                __leave;
            }

            // 进程映象名和
            if(_tcsncmp(procPathDev, volNameDev, _tcslen(volNameDev)) == 0)
            {
                ret = true;
                break;
            }
        }

        if(ret)
        {
            // 盘符
            _tcscat_s(pathName, MAX_PATH / sizeof(Byte8), volName);
            // 盘符后的路径
            _tcscat_s(pathName, MAX_PATH / sizeof(Byte8), procPathDev + _tcslen(volNameDev));
            processPath << pathName;
        }
    }
    __finally
    {
        if(hModule)
            FreeLibrary(hModule);

        if(hProc)
            CloseHandle(hProc);
    }

    return ret;
}

HANDLE SystemUtil::CreateProcessSnapshot()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(INVALID_HANDLE_VALUE == hSnapshot)
    {
        return NULL;
    }

    return hSnapshot;
}

ULong SystemUtil::GetFirstProcessPid(HANDLE &hSnapshot)
{
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(pe);

    Process32First(hSnapshot, &pe);
    return pe.th32ProcessID;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    auto *pArg = (EnumWindowsArg *)lParam;

    // 通过窗口句柄取得进程ID
    DWORD  dwProcessID = 0;
    ::GetWindowThreadProcessId(hwnd, &dwProcessID);
    if(dwProcessID == pArg->dwProcessID)
    {
        pArg->hwndWindow = hwnd;
        // 找到了返回FALSE
        return false;
    }

    // 没找到，继续找，返回TRUE
    return true;
}

HWND SystemUtil::GetWindowHwndByPID(DWORD dwProcessID)
{
    HWND hwndRet = NULL;
    EnumWindowsArg ewa;
    ewa.dwProcessID = dwProcessID;
    ewa.hwndWindow = NULL;
    EnumWindows(fs::EnumWindowsProc, (LPARAM)&ewa);
    if(ewa.hwndWindow)
    {
        hwndRet = ewa.hwndWindow;
    }
    return hwndRet;
}

Int32 SystemUtil::GetProcessId()
{
    return _getpid();
}

void SystemUtil::BringWindowsToTop(HWND curWin)
{
    ::BringWindowToTop(curWin);
    ::ShowWindow(curWin, SW_SHOW);
}

bool SystemUtil::IsProcessExist(const FS_String &processName)
{
    // 遍历进程
    auto hProcModule = CreateProcessSnapshot();
    auto pid = GetFirstProcessPid(hProcModule);
    bool isFirst = true;
    fs::FS_String pachCache;
    for(; isFirst ? isFirst : (pid > 0); pid = GetNextProcessPid(hProcModule))
    {
        isFirst = false;
        pachCache.Clear();
        if(!fs::SystemUtil::GetProgramPath(false, pid, pachCache))
            continue;

        auto iterExist = pachCache.GetRaw().find(processName.GetRaw());
        if(iterExist != std::string::npos)
            return true;
    }

    return false;
}

ULong SystemUtil::GetNextProcessPid(HANDLE &hSnapshot)
{
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(pe);
    if(!Process32Next(hSnapshot, &pe))
        return 0;

    return pe.th32ProcessID;
}

FS_NAMESPACE_END
