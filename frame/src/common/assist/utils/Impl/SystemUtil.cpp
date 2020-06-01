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
 * @file  : SystemUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/6/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/assist/utils/Defs/SystemUtilDefs.h"
#include "FrightenStone/common/assist/utils/Impl/FS_DirectoryUtil.h"
#include "FrightenStone/common/assist/utils/Impl/FS_FileUtil.h"

#pragma region windows
#ifdef _WIN32
#include "tchar.h"
#include "Psapi.h"
#include "tlhelp32.h"
#include "process.h"
#include "sysinfoapi.h"
#else
// linux:
#include <sys/types.h>
#include <signal.h>
#endif
#pragma endregion

#ifdef _WIN32
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

#endif

FS_NAMESPACE_BEGIN

#ifdef _WIN32
#pragma region windowsdefines
typedef struct
{
    HWND    hwndWindow;     // 窗口句柄
    DWORD   dwProcessID;    // 进程ID
}EnumWindowsArg;
#pragma endregion

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
#endif

Int32 SystemUtil::GetProgramPath(bool isCurrentProcess, FS_String &processPath, UInt64 pid)
{
#ifdef _WIN32
    HMODULE hModule = NULL;
    HANDLE hProc = NULL;

    do
    {
        if(UNLIKELY(!isCurrentProcess && !pid))
            return StatusDefs::ParamError;

        // 若是当前进程
        Byte8   pathName[MAX_PATH] = {0};
        if(isCurrentProcess)
        {
            if(UNLIKELY(!GetModuleFileName(NULL, pathName, MAX_PATH)))
                return StatusDefs::SystemUtil_GetModuleFileNameFailed;

            processPath.AppendBitData(pathName, MAX_PATH);
            break;
        }

        hProc = OpenProcess(PROCESS_QUERY_INFORMATION, false, static_cast<DWORD>(pid));
        if(UNLIKELY(!hProc))
            return StatusDefs::SystemUtil_OpenProcessQueryInfomationFailed;

        hModule = LoadLibrary(TEXT("Kernel32.dll"));
        if(UNLIKELY(!hModule))
            return StatusDefs::SystemUtil_LoadKernel32LibraryFailed;

        // 获取QueryFullProcessImageNameA函数
        if(GetProcAddress(hModule, "QueryFullProcessImageNameA"))
        {
            DWORD dwProcPathLen = MAX_PATH / sizeof(Byte8);
            if(!QueryFullProcessImageName(hProc, 0, pathName, &dwProcPathLen))
                return StatusDefs::SystemUtil_QueryFullProcessImageNameFailed;

            processPath.AppendBitData(pathName, MAX_PATH);
            break;
        }

        // 获取进程带驱动器名的路径（驱动器名：\\Device\\HardwareVolume1）
        if(!GetProcessImageFileName(hProc, pathName, MAX_PATH))
            return StatusDefs::SystemUtil_GetProcessImageFileNameFailed;

        // 遍历确认驱动器名对应的盘符名
        Byte8   volNameDev[MAX_PATH] = {0};
        Byte8   volName[MAX_PATH] = {0};
        _tcscat_s(volName, MAX_PATH, TEXT("A:"));
        bool isFound = false;
        for(; *volName <= _T('Z'); (*volName)++)
        {
            // 获取盘符
            if(!QueryDosDevice(volName, volNameDev, MAX_PATH))
            {
                auto lastError = GetLastError();
                if(lastError == 2)
                    continue;

                return StatusDefs::SystemUtil_QueryDosDeviceError;
            }

            // 确认是否驱动器名一样
            if(_tcsncmp(pathName, volNameDev, _tcslen(volNameDev)) == 0)
            {
                isFound = true;
                break;
            }
        }

        if(!isFound)
            return StatusDefs::SystemUtil_GetDriveError;

        processPath.AppendBitData(volName, _tcslen(volName));
        processPath.AppendBitData(pathName + _tcslen(volNameDev), _tcslen(pathName) - _tcslen(volNameDev));
    } while(0);

    if(hModule)
        FreeLibrary(hModule);

    if(hProc)
        CloseHandle(hProc);

    return StatusDefs::Success;
#else

    ssize_t ret = -1;
    char buf[PATH_MAX + 1];
    if(isCurrentProcess)
    {
        if((ret = readlink("/proc/self/exe", buf, PATH_MAX)) == -1)
            return StatusDefs::SystemUtil_GetProcNameFail;
    }
    else
    {
        BUFFER64 proc = {};
        sprintf(proc, "/proc/%llu/exe", pid);
        if((ret = readlink(proc, buf, PATH_MAX)) == -1)
            return StatusDefs::SystemUtil_GetProcNameFail;
    }

    buf[ret] = '\0';
    processPath = buf;
    return StatusDefs::Success;
#endif
}

FS_String SystemUtil::GetCurProgramName()
{
    FS_String path;
    SystemUtil::GetProgramPath(true, path);
    return FS_DirectoryUtil::GetFileNameInPath(path);
}

FS_String SystemUtil::GetApplicationName()
{
    return FS_FileUtil::ExtractFileWithoutExtension(SystemUtil::GetCurProgramName().RemoveZeroTail());
}

UInt64 SystemUtil::GetCurrentThreadId()
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#else
    return ::pthread_self();
#endif
}

Int32 SystemUtil::GetCurProcessId()
{
#ifdef _WIN32
    return ::_getpid();
#else
    return ::getpid();
#endif
}

Int32 SystemUtil::CloseProcess(Int32 processId, ULong *lastError)
{
#ifdef _WIN32
    if(!TerminateProcess(OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, false, processId), 0))
    {
        if(lastError)
            *lastError = GetLastError();

        return StatusDefs::Failed;
    }
#else
    auto ret = kill(processId, SIGKILL);
    if(ret != 0)
    {
        perror("kill process fail");
        return StatusDefs::Failed;
    }
#endif

    return StatusDefs::Success;
}

#ifdef _WIN32

UInt64 SystemUtil::GetAvailPhysMemSize()
{
#ifdef _WIN32
    MEMORYSTATUSEX status;
    auto ret = GetMemoryStatus(status);
    if(ret != StatusDefs::Success)
        return 0;

    return status.ullAvailPhys;

#else
    // TODO:Linux
#endif
}

UInt64 SystemUtil::GetTotalPhysMemSize()
{
#ifdef _WIN32
    MEMORYSTATUSEX status;
    auto ret = GetMemoryStatus(status);
    if(ret != StatusDefs::Success)
        return 0;

    return status.ullTotalPhys;
#else
    // TODO:Linux
#endif
}

UInt64 SystemUtil::GetMemoryLoad()
{
#ifdef _WIN32
    MEMORYSTATUSEX status;
    auto ret = GetMemoryStatus(status);
    if(ret != StatusDefs::Success)
        return 0;

    return status.dwMemoryLoad;
#else
    // TODO:Linux
#endif
}
// 进程占用内存信息
bool SystemUtil::GetProcessMemInfo(HANDLE processHandle, ProcessMemInfo &info)
{
    PROCESS_MEMORY_COUNTERS_EX processInfo = {};
    if(!GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS *)&processInfo, sizeof(PROCESS_MEMORY_COUNTERS_EX)))
        return false;

    info._maxHistorySetSize = processInfo.PeakWorkingSetSize;
    info._curSetSize = processInfo.WorkingSetSize;
    info._maxHistoryPagedPoolUsage = processInfo.QuotaPeakPagedPoolUsage;
    info._pagedPoolUsage = processInfo.QuotaPagedPoolUsage;
    info._maxHistoryNonPagedPoolUsage = processInfo.QuotaPeakNonPagedPoolUsage;
    info._curNonPagedPoolUsage = processInfo.QuotaNonPagedPoolUsage;
    info._curPageFileUsage = processInfo.PagefileUsage;
    info._maxHistoryPageFileUsage = processInfo.PeakPagefileUsage;
    info._processAllocMemoryUsage = processInfo.PrivateUsage;
    return true;
}

HANDLE SystemUtil::CreateProcessSnapshot()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(INVALID_HANDLE_VALUE == hSnapshot)
        return NULL;

    return hSnapshot;
}

UInt64 SystemUtil::GetFirstProcessPid(HANDLE &hSnapshot)
{
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(pe);

    Process32First(hSnapshot, &pe);
    return pe.th32ProcessID;
}

UInt64 SystemUtil::GetNextProcessPid(HANDLE &hSnapshot)
{
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(pe);
    if(!Process32Next(hSnapshot, &pe))
        return 0;

    return pe.th32ProcessID;
}

// 获取进程句柄
HANDLE SystemUtil::GetCurProcessHandle()
{
    return ::GetCurrentProcess();
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

void SystemUtil::BringWindowsToTop(HWND curWin)
{
    ::BringWindowToTop(curWin);
    ::ShowWindow(curWin, SW_SHOW);
}

// 弹窗
void SystemUtil::MessageBoxPopup(const FS_String &title, const FS_String &content)
{
#ifdef _WIN32
    auto hwnd = GetWindowHwndByPID(GetCurProcessId());
    ::MessageBoxA(hwnd, content.c_str(), title.c_str(), MB_ABORTRETRYIGNORE);
#endif
}

void SystemUtil::GetCallingThreadCpuInfo(UInt16 &cpuGroup, Byte8 &cpuNumber)
{
    PROCESSOR_NUMBER processorInfo = {};
    GetCurrentProcessorNumberEx(&processorInfo);
    cpuGroup = processorInfo.Group;
    cpuNumber = processorInfo.Number;
}

bool SystemUtil::IsProcessExist(const FS_String &processName)
{
#ifdef _WIN32
    // 遍历进程
    auto hProcModule = CreateProcessSnapshot();
    auto pid = GetFirstProcessPid(hProcModule);
    bool isFirst = true;
    fs::FS_String pachCache;
    for(; isFirst ? isFirst : (pid > 0); pid = GetNextProcessPid(hProcModule))
    {
        isFirst = false;
        pachCache.Clear();
        if(GetProgramPath(false, pachCache, pid) != StatusDefs::Success)
            continue;

        auto iterExist = pachCache.GetRaw().find(processName.GetRaw());
        if(iterExist != std::string::npos)
            return true;
    }

    return false;
#else
    // TODO:linux
    return false;
#endif
}

#else
#endif

void SystemUtil::LockConsole()
{
    __g_consoleLock.Lock();
}

void SystemUtil::UnlockConsole()
{
    __g_consoleLock.Unlock();
}

Int32 SystemUtil::SetConsoleColor(Int32 color)
{
#ifdef _WIN32
    HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if(::SetConsoleTextAttribute(handle, color) == 0)
    {
        ASSERT(!"SetConsoleTextAttribute failed");
        __g_consoleLock.Unlock();
        return StatusDefs::Failed;
    }
#endif

    return StatusDefs::Success;
}

Int32 SystemUtil::GetConsoleColor()
{
#ifdef _WIN32
    HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if(::GetConsoleScreenBufferInfo(handle, &info) == 0)
    {
        ASSERT(!"GetConsoleScreenBufferInfo failed");
        __g_consoleLock.Unlock();
        return StatusDefs::Error;
    }

    return info.wAttributes;
#endif

    return 0;
}

void SystemUtil::OutputToConsole(const FS_String &outStr)
{
    printf("%s", outStr.c_str());
}

bool SystemUtil::IsLittleEndian()
{
    // 若是小端字节序，从低位开始存储，则顺序为l,?,?,b, 则endian_test.l的第一个字节为'l',若是大端则是'b'
    static union {
        char c[4];
        unsigned long l;
    } endian_test = {{'l', '?', '?', 'b'}};

    return static_cast<char>(endian_test.l) != 'b';
}

void SystemUtil::Sleep(UInt64 milliSec, UInt64 microSec /*= 0*/)
{
    std::chrono::microseconds t(milliSec*Time::_microSecondPerMilliSecond + microSec);
    std::this_thread::sleep_for(t);
}

FS_String SystemUtil::GetErrString(Int32 err)
{
#ifndef _WIN32
    return strerror(err);
#else
    return "";
#endif
}

FS_NAMESPACE_END



