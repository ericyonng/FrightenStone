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
 * @file  : CrashHandleUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/2
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "FrightenStone/common/crashhandle/Impl/CrashHandleUtil.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/assist/utils/utils.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "iostream"
#include "FrightenStone/common/memleak/memleak.h"

#ifndef _WIN32
// linux下堆栈追踪头文件
#include <execinfo.h>
// linux下类型识别接口相关
#include <cxxabi.h>
#endif

#define __USE_FS_DBGHELP__
#include "3rd/3rd.h"

#ifdef _WIN32
#define FS_CFG_APP_DUMPFILE_DUMPTYPES               (MiniDumpWithFullMemory | \
                                                     MiniDumpWithFullMemoryInfo | \
                                                     MiniDumpIgnoreInaccessibleMemory | \
                                                     MiniDumpWithFullAuxiliaryState | \
                                                     MiniDumpWithHandleData | \
                                                     MiniDumpWithThreadInfo)
#endif



FS_NAMESPACE_BEGIN

static FS_String __dumpFileName;
static Locker __g_SehLocker;

#ifdef _WIN32
static void __GetExceptionBackTrace(PCONTEXT ctx, FS_String &backTrace)
{
#if defined(_M_IX86)    // 32位cpu
    DWORD machineType = IMAGE_FILE_MACHINE_I386;
#elif defined(_M_X64)   // 64位cpu
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
#else
    return;
#endif

    STACKFRAME64 stackFrame64;
    ::memset(&stackFrame64, 0, sizeof(STACKFRAME64));
#if defined(_M_IX86)
    stackFrame64.AddrPC.Offset = ctx->Eip;
    stackFrame64.AddrPC.Mode = AddrModeFlat;
    stackFrame64.AddrStack.Offset = ctx->Esp;
    stackFrame64.AddrStack.Mode = AddrModeFlat;
    stackFrame64.AddrFrame.Offset = ctx->Ebp;
    stackFrame64.AddrFrame.Mode = AddrModeFlat;
#elif (_M_X64)
    stackFrame64.AddrPC.Offset = ctx->Rip;
    stackFrame64.AddrPC.Mode = AddrModeFlat;
    stackFrame64.AddrStack.Offset = ctx->Rsp;
    stackFrame64.AddrStack.Mode = AddrModeFlat;
    stackFrame64.AddrFrame.Offset = ctx->Rbp;
    stackFrame64.AddrFrame.Mode = AddrModeFlat;
#endif // _M_IX86

    HANDLE curProc = ::GetCurrentProcess();
    HANDLE curThread = ::GetCurrentThread();
    std::vector<FS_String> backTraces;
    while(true)
    {
        if(!::StackWalk64(machineType,
                          curProc,
                          curThread,
                          &stackFrame64,
                          ctx,
                          NULL,
                          ::SymFunctionTableAccess64,
                          ::SymGetModuleBase64,
                          NULL))
            break;

        if(stackFrame64.AddrFrame.Offset == 0)
            break;

        BYTE symbolBuffer[sizeof(SYMBOL_INFO) + 512];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = 511;

        if(!::SymFromAddr(curProc, stackFrame64.AddrPC.Offset, NULL, symbol))
            break;

        DWORD symDisplacement = 0;
        IMAGEHLP_LINE64 lineInfo = {sizeof(IMAGEHLP_LINE64)};
        if(::SymGetLineFromAddr64(curProc, stackFrame64.AddrPC.Offset, &symDisplacement, &lineInfo))
        {
            backTraces.push_back(FS_String().AppendFormat("0x%x in %s at %s:%d",
                (void *)symbol->Address, symbol->Name, lineInfo.FileName, lineInfo.LineNumber));
        }
        else
        {
            backTraces.push_back(FS_String().AppendFormat("0x%x in %s at %s:%d",
                (void *)symbol->Address, symbol->Name, "", 0));
        }
    }

    for(size_t i = 0; i < backTraces.size(); i++)
        backTrace.AppendFormat("#%d %s\n", backTraces.size() - i - 1, backTraces[i].c_str());
}

static LONG WINAPI __AppCrashHandler(::EXCEPTION_POINTERS *exception)
{
    HANDLE dmpFile = ::CreateFileA(__dumpFileName.c_str(),
                                   GENERIC_WRITE,
                                   FILE_SHARE_READ,
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if(UNLIKELY(dmpFile == INVALID_HANDLE_VALUE))
        return EXCEPTION_CONTINUE_SEARCH;

    ::MINIDUMP_EXCEPTION_INFORMATION dmpInfo;
    EXCEPTION_POINTERS cache = *exception;
    dmpInfo.ExceptionPointers = &cache;
    dmpInfo.ThreadId = ::GetCurrentThreadId();
    dmpInfo.ClientPointers = TRUE;

    ::MiniDumpWriteDump(::GetCurrentProcess(),
                        ::GetCurrentProcessId(),
                        dmpFile,
                        (MINIDUMP_TYPE)FS_CFG_APP_DUMPFILE_DUMPTYPES,
                        &dmpInfo,
                        NULL,
                        NULL);

    ::CloseHandle(dmpFile);

    FS_String errMsg;
    errMsg << "Unhandled exception!\n";
    errMsg.AppendFormat("Mini dump file path:%s\n", __dumpFileName.c_str());

    FS_String backTrace;
    __GetExceptionBackTrace(cache.ContextRecord, backTrace);
    errMsg.AppendFormat("\nStack BackTrace:\n%s", backTrace.c_str());
    g_Log->crash("\n[ ******** crash info ******** ]%s\n[ ******** End ******** ]\n", errMsg.c_str());
    g_Log->FlushAllFile();
    g_Log->FinishModule();

    return EXCEPTION_EXECUTE_HANDLER;
}


static BOOL __PreventSetUnhandledExceptionFilter()
{
    HMODULE kernel32 = ::LoadLibraryA("kernel32.dll");
    if(kernel32 == NULL)
        return FALSE;

    void *orgEntry = (void *)::GetProcAddress(kernel32, "SetUnhandledExceptionFilter");
    if(orgEntry == NULL)
        return FALSE;

#ifdef _M_IX86
    // Code for x86:
    // 33 C0    xor eax, eax
    // C2 04 00 ret 4
    unsigned char execute[] = {0x33, 0xc0, 0xc2, 0x04, 0x00};
#elif _M_X64
    // Code for x64
    // 33 c0    xor eax, eax
    // c3       ret
    unsigned char execute[] = {0x33, 0xc0, 0xc3};
#else
#error "Unsupported architecture(on windows platform)!"
#endif

    SIZE_T bytesWritten = 0;
    return ::WriteProcessMemory(GetCurrentProcess(),
                                orgEntry,
                                execute,
                                sizeof(execute),
                                &bytesWritten);
}
#endif

int CrashHandleUtil::InitCrashHandleParams(bool isUseSehExceptionHandler)
{
#ifndef _WIN32
    return StatusDefs::Success;
#else

    FS_String procPath;
    SystemUtil::GetProgramPath(true, procPath);
    auto fileName = FS_DirectoryUtil::GetFileNameInPath(procPath);
    fileName.strip(" ");
    auto fileNameExt = fileName.Split('.');

    Time nowTime;
    nowTime.FlushTime();
    fileName = fileNameExt[0];
    fileName.AppendFormat("_%d%02d%02d_%02d%02d%02d_%06d_%s"
                          , nowTime.GetLocalYear()
                          , nowTime.GetLocalMonth()
                          , nowTime.GetLocalDay()
                          , nowTime.GetLocalHour()
                          , nowTime.GetLocalMinute()
                          , nowTime.GetLocalSecond()
                          , nowTime.GetLocalMilliSecond() * 1000 + nowTime.GetLocalMicroSecond()
                          , fileNameExt[1].c_str());

    if(fileNameExt[1] != ".dmp")
        fileName += ".dmp";

    __dumpFileName = fileName;

    if(!isUseSehExceptionHandler)
        ::SetUnhandledExceptionFilter(fs::__AppCrashHandler);

#ifndef _DEBUG
    if(!isUseSehExceptionHandler)
        __PreventSetUnhandledExceptionFilter();
#endif // Release

    const Int32 initSymbolRet = InitSymbol();
    ASSERT(initSymbolRet == StatusDefs::Success);
    if(initSymbolRet != StatusDefs::Success)
        return initSymbolRet;

    // 设置crashlog hook
    g_Log->InstallBeforeLogHookFunc(LogLevel::Crash, &CrashHandleUtil::_OnBeforeCrashLogHook);
    auto afterDelegate = g_Log->InstallLogHookFunc(LogLevel::Crash, &CrashHandleUtil::_OnAfterCrashLogHook);

    return StatusDefs::Success;
#endif
}

#ifdef _WIN32
Int32 CrashHandleUtil::RecordExceptionInfo(EXCEPTION_POINTERS exceptionInfo)
{
    __g_SehLocker.Lock();
    const auto st = __AppCrashHandler(&exceptionInfo);
    __g_SehLocker.Unlock();
    return st;
}
#endif

Int32 CrashHandleUtil::InitSymbol()
{
#ifdef _WIN32
    // 初始化pdb符号
    if(!::SymInitialize(::GetCurrentProcess(), NULL, TRUE))
    {
        const UInt64 err = GetLastError();
        g_Log->e<CrashHandleUtil>(_LOGFMT_("SymInitialize fail error[%llu]"), err);
        return StatusDefs::CrashHandleUtil_SymInitializeFail;
    }

    return StatusDefs::Success;
#else // Non-Win32
    return StatusDefs::Success;
#endif
}

FS_String CrashHandleUtil::FS_CaptureStackBackTrace(size_t skipFrames /*= 0*/, size_t captureFrames /*= FS_INFINITE*/)
{
    // 快照帧数
    if(captureFrames == FS_INFINITE)
        captureFrames = SYMBOL_MAX_CAPTURE_FRAMES;
    else
        captureFrames = std::min<size_t>(captureFrames, SYMBOL_MAX_CAPTURE_FRAMES);

    // 初始化堆栈结构
    void *stackArray[SYMBOL_MAX_CAPTURE_FRAMES] = {0};
    void **stack = stackArray;

#ifdef _WIN32
    // name是个变长数组
    SYMBOL_INFO *win32Symboal = reinterpret_cast<SYMBOL_INFO *>(new char[sizeof(::SYMBOL_INFO) + (SYMBOL_MAX_SYMBOL_NAME + 1) * sizeof(char)]);
    memset(win32Symboal, 0, sizeof(SYMBOL_INFO));
    win32Symboal->SizeOfStruct = sizeof(::SYMBOL_INFO);
    win32Symboal->MaxNameLen = SYMBOL_MAX_SYMBOL_NAME;
    IMAGEHLP_LINE64 win32ImgHelpLine64;
    ::memset(&win32ImgHelpLine64, 0, sizeof(win32ImgHelpLine64));
    win32ImgHelpLine64.SizeOfStruct = sizeof(win32ImgHelpLine64);

    // 抓取快照 取得各个展开的函数地址
    DWORD displacement;
    HANDLE curProc = ::GetCurrentProcess();
    SYMBOL_INFO *symbol = win32Symboal;
    const WORD frames = ::CaptureStackBackTrace(static_cast<DWORD>(skipFrames) + 1,
                                                static_cast<DWORD>(captureFrames),
                                                stack,
                                                NULL);

    // 构建快照字符串信息
    FS_String backTrace;
    for(WORD frame = 0; frame != frames; frame++)
    {
        ::SymFromAddr(curProc, (DWORD64)stack[frame], 0, symbol);
        IMAGEHLP_LINE64 &imgHelpLine64 = win32ImgHelpLine64;
        if(::SymGetLineFromAddr64(curProc, symbol->Address, &displacement, &imgHelpLine64))
        {
            backTrace.AppendFormat("#%d 0x%x in %s at %s:%d", frames - frame - 1, (void *)symbol->Address,
                                    symbol->Name, imgHelpLine64.FileName, imgHelpLine64.LineNumber);
        }
        else
        {
            backTrace.AppendFormat("#%d 0x%x in %s at %s:%d", frames - frame - 1,
                (void *)symbol->Address, symbol->Name, "", 0);
        }

        if(frame != frames - 1)
            backTrace.AppendBitData("\n", 1);
    }

    if(win32Symboal)
        delete[]win32Symboal;
#else // Non-Win32
    FS_String backTrace;
    char rtti[__FS_RTTI_BUF_SIZE__] = {};
    const int frames = ::backtrace(stack, captureFrames + skipFrames);
    char **strs = ::backtrace_symbols(stack, frames);
    if(LIKELY(strs))
    {
        for(int i = skipFrames; i < frames; i++)
        {
            backTrace.AppendFormat("#%d ", frames - i - 1);

            char *parenthesisEnd = NULL;
            char *parenthesisBeg = strchr(strs[i], '(');
            if(parenthesisBeg)
            {
                parenthesisBeg += 1;
                parenthesisEnd = strchr(parenthesisBeg, ')');
            }

            if(parenthesisEnd &&
               parenthesisBeg != parenthesisEnd)
            {
                char *addrOffsetBeg = strchr(parenthesisBeg, '+');
                if(addrOffsetBeg == NULL)
                    addrOffsetBeg = parenthesisEnd;

                const char oldAddrOffsetBegCh = *addrOffsetBeg;
                *addrOffsetBeg = '\0';

                int status = 0;
                size_t length = sizeof(rtti);
                abi::__cxa_demangle(parenthesisBeg, rtti, &length, &status);
                *addrOffsetBeg = oldAddrOffsetBegCh;
                if(status == 0)
                {
                    backTrace.AppendBitData(strs[i], parenthesisBeg - strs[i]);
                    backTrace << rtti;
                    backTrace << addrOffsetBeg;
                }
                else
                {
                    backTrace.AppendFormat("%s", strs[i]);
                }
            }
            else
            {
                backTrace.AppendFormat("%s", strs[i]);
            }

            if(i != frames - 1)
                backTrace.AppendBitData("\n", 1);
        }

        free(strs);
    }
#endif // LLBC_TARGET_PLATFORM_WIN32

    return backTrace;
}

void CrashHandleUtil::_OnBeforeCrashLogHook(LogData *logData)
{
    // 打印堆栈信息
//     auto backStrace = FS_CaptureStackBackTrace();
//     logData->_logToWrite.Format("\n[ ******* crash info ******* ]\n%s\n[ ******* End ******* ]\n"
//                                 , backStrace.c_str());
}

void CrashHandleUtil::_OnAfterCrashLogHook(const LogData *logData)
{
#ifdef _WIN32
    // 内存泄漏信息
    MemleakMonitor::GetInstance()->PrintObjPoolInfo();

    // 系统内存情况
    g_Log->sys<CrashHandleUtil>(_LOGFMT_("TotalPhysMemSize[%llu];AvailPhysMemSize[%llu] mem use rate[MemoryLoad:[%llu]]")
               , SystemUtil::GetTotalPhysMemSize(), SystemUtil::GetAvailPhysMemSize(), SystemUtil::GetMemoryLoad());

    // 弹窗堆栈信息
    FS_String path;
    SystemUtil::GetProgramPath(true, path);
    auto fileName = FS_DirectoryUtil::GetFileNameInPath(path);
    SystemUtil::MessageBoxPopup(fileName, logData->_logToWrite);
#endif

// #ifdef _DEBUG
//     // 弹窗堆栈信息
//     FS_String path;
//     SystemUtil::GetProgramPath(true, path);
//     auto fileName = FS_DirectoryUtil::GetFileNameInPath(path);
//     ::MessageBoxA(NULL, logData->_logToWrite.c_str(), fileName.c_str(), MB_ICONERROR | MB_OK);
// #else
//     Sleep(1000);
// #endif
}

FS_NAMESPACE_END
