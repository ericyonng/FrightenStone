#ifndef CRASH_DUMP_INL_
#define CRASH_DUMP_INL_

#pragma once
#include <BaseCode/CrashDumper.h>

#ifndef _DEBUG
#pragma comment(lib, "BaseCode/lib/Release/CrashDump.lib")
#else
#pragma comment(lib, "BaseCode/lib/Debug/CrashDumpD.lib")
#endif

#endif
