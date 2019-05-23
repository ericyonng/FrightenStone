#include <BaseCode/Win/WinGtool.h>


#if defined(NEED_WINDOWS_COMMON_API)



NAMESPACE_BEGIN(gtool)

//windows平台毫秒级
time_t InsertCurrentTime(gtool::CFmtIOString& fos)								//时间戳
{
	//获取时间
	SYSTEMTIME tm_now = { 0 };
	GetLocalTime(&tm_now);

	auto nStrLen = sprintf(fos.GetBuffer(), "%hu.%02hu.%02hu %02hu:%02hu:%02hu.%03hu ",
		tm_now.wYear, tm_now.wMonth, tm_now.wDay, tm_now.wHour, tm_now.wMinute, tm_now.wSecond, tm_now.wMilliseconds);
	nStrLen = ((nStrLen < BUFFER_LEN32) ? gtool::Max(nStrLen, 0) : (BUFFER_LEN32 - 1));
	fos.GetBuffer()[nStrLen] = 0;
	fos << fos.GetBuffer();
	fos.GetBuffer()[0] = 0;

	tm	tmMkTime;
	memset(&tmMkTime, 0, sizeof(tm));
	tmMkTime.tm_year = tm_now.wYear - 1900;
	tmMkTime.tm_mon = tm_now.wMonth - 1;
	tmMkTime.tm_mday = tm_now.wDay;
	tmMkTime.tm_hour = tm_now.wHour;
	tmMkTime.tm_min = tm_now.wMinute;
	tmMkTime.tm_sec = tm_now.wSecond;

	return mktime(&tmMkTime);
}



//获取内存信息
void GetMemoryInfo(MEMORYSTATUS& MemInfo)
{
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);
}

msize_t GetMemPoolAlignByte(const msize_t&nByte, msize_t nPoolRestByte)
{
	//使用物理内存剩余大小
	if (nPoolRestByte < 0)
	{
		MEMORYSTATUS rMemInfo;
		gtool::GetMemoryInfo(rMemInfo);
		nPoolRestByte = msize_t(rMemInfo.dwAvailPhys) - PYSICAL_MEM_REMAIN_BYTE;
	}
	CHECKF_NL(nPoolRestByte >= 0);

	//本机内存剩余大小
	msize_t nNeededByte = ((nByte% MEMBLOCK_MINIUM_SIZE) != 0) ? ((nByte / MEMBLOCK_MINIUM_SIZE + 1)*MEMBLOCK_MINIUM_SIZE) : nByte;
	nNeededByte = ((nNeededByte > I64(nPoolRestByte)) ? (I64(nPoolRestByte) / MEMBLOCK_MINIUM_SIZE * MEMBLOCK_MINIUM_SIZE) : nNeededByte);
	return nNeededByte;
}

msize_t GetSysAlignByte(const msize_t&nByte)
{
	//本机内存剩余大小
	msize_t nNeededByte = ((nByte% MEMBLOCK_MINIUM_SIZE) != 0) ? ((nByte / MEMBLOCK_MINIUM_SIZE + 1)*MEMBLOCK_MINIUM_SIZE) : nByte;
	MEMORYSTATUS rMemInfo;
	gtool::GetMemoryInfo(rMemInfo);
	I64 nSysTemMemRest = I64(rMemInfo.dwAvailPhys) - I64(PYSICAL_MEM_REMAIN_BYTE);//必须给系统内存留余量
	CHECKF_NL(nSysTemMemRest >= 0);

	nNeededByte = ((nNeededByte > I64(nSysTemMemRest)) ? (I64(nSysTemMemRest) / MEMBLOCK_MINIUM_SIZE * MEMBLOCK_MINIUM_SIZE) : nNeededByte);
	return nNeededByte;
}

typedef   void(WINAPI*   FunctionGlobalMemoryStatusEx)(LPMEMORYSTATUS);//声明函数原型指针

msize_t GetAvailProcessSize()
{
	HMODULE hModule;
	FunctionGlobalMemoryStatusEx GlobalMemoryStatusEx;
	MEMORYSTATUS status;
	status.dwLength = sizeof(status);
	hModule = LoadLibrary("kernel32.dll");//载入动态链接库kernel32.dll，返回它的句柄
	IF_NOT_NL (hModule)//判断载入dll是否成功
		return -1;

	//在kernel32.dll句柄里查找GlobalMemoryStatusEx函数，返回改函数的指针
	GlobalMemoryStatusEx = (FunctionGlobalMemoryStatusEx)GetProcAddress(hModule, "GlobalMemoryStatusEx");
	IF_NOT_NL (GlobalMemoryStatusEx)//判断函数是否找到
		return -1;

	GlobalMemoryStatusEx(&status);//调用函数取得系统的内存情况
	FreeLibrary(hModule);//释放链接库句柄
	return status.dwAvailVirtual;
}

msize_t GetTotalProcessSize()
{
	HMODULE hModule;
	FunctionGlobalMemoryStatusEx GlobalMemoryStatusEx;
	MEMORYSTATUS status;
	status.dwLength = sizeof(status);
	hModule = LoadLibrary("kernel32.dll");//载入动态链接库kernel32.dll，返回它的句柄
	IF_NOT_NL(hModule)//判断载入dll是否成功
		return -1;

	//在kernel32.dll句柄里查找GlobalMemoryStatusEx函数，返回改函数的指针
	GlobalMemoryStatusEx = (FunctionGlobalMemoryStatusEx)GetProcAddress(hModule, "GlobalMemoryStatusEx");
	IF_NOT_NL(GlobalMemoryStatusEx)//判断函数是否找到
		return -1;

	GlobalMemoryStatusEx(&status);//调用函数取得系统的内存情况
	FreeLibrary(hModule);//释放链接库句柄
	return status.dwTotalVirtual;
}

//获取进程路径
bool GetProcPath(
	__in	bool	bCurrentProc,
	__in	MYULONG	ulPid,
	__inout LPTSTR	lpProcPath,
	__in	MYULONG	ulProcPathBufLen
)
{
	bool	bRet = false;

	HMODULE hModule = NULL;
	HANDLE	hProc = NULL;
	DWORD	dwProcPathLen = 0;
	I8		tchProcPathDev[MAX_PATH] = { 0 };
	I8		tchVolNameDev[MAX_PATH] = { 0 };
	I8		tchVolName[MAX_PATH] = { 0 };


	__try
	{
		if (!lpProcPath || !ulProcPathBufLen || (!bCurrentProc && !ulPid))
		{
			//ASSERT(!"[GetProcPath] : input parameter error \n");
			__leave;
		}

		if (bCurrentProc)
		{
			if (!GetModuleFileName(NULL, lpProcPath, ulProcPathBufLen / sizeof(I8)))
			{
				//ASSERT(!"[GetProcPath] : GetModuleFileName failed err[", GetLastError(), "].");
				ASSERT(!"[GetProcPath] : GetModuleFileName failed err[");
				__leave;
			}

			bRet = true;
			__leave;
		}

		hProc = OpenProcess(PROCESS_QUERY_INFORMATION, false, ulPid);
		if (!hProc)
		{
			//ASSERTEX("[GetProcPath] : OpenProcess failed err[", GetLastError(), "].");
			//ASSERT(!"[GetProcPath] : OpenProcess failed err[");
			__leave;
		}

		hModule = LoadLibrary(_T("Kernel32.dll"));
		if (!hModule)
		{
			//ASSERTEX("[GetProcPath] : LoadLibrary failed. err[", GetLastError(), "].");
			ASSERT(!"[GetProcPath] : LoadLibrary failed. err[");
			__leave;
		}

		if (GetProcAddress(hModule, "QueryFullProcessImageNameA"))	//获取QueryFullProcessImageNameA函数
		{
			dwProcPathLen = ulProcPathBufLen / sizeof(I8);
			if (!QueryFullProcessImageName(hProc, 0, lpProcPath, &dwProcPathLen))
			{
				//ASSERTEX("[GetProcPath] : QueryFullProcessImageName failed. err[", GetLastError(), "].");
				ASSERT(!"[GetProcPath] : QueryFullProcessImageName failed. err[");
				__leave;
			}

			bRet = true;
			__leave;
		}

		if (!GetProcessImageFileName(hProc, tchProcPathDev, MAX_PATH))	//获取进程路径
		{
			//ASSERTEX("[GetProcPath] : GetProcessImageFileName failed err [", GetLastError(), "].");
			ASSERT(!"[GetProcPath] : GetProcessImageFileName failed err [");
			__leave;
		}

		_tcscat_s(tchVolName, MAX_PATH, _T("A:"));
		for (; *tchVolName <= _T('Z'); (*tchVolName)++)
		{
			memset(tchVolNameDev, 0, MAX_PATH * sizeof(I8));
			if (!QueryDosDevice(tchVolName, tchVolNameDev, MAX_PATH))	//获取盘符
			{
				if (GetLastError() == 2)
					continue;
				else
				{
					//ASSERT("[GetProcPath] : QueryDosDevice failed. err[", GetLastError(), "].");
					ASSERT(!"[GetProcPath] : QueryDosDevice failed. err[");
					__leave;
				}
			}

			if (_tcsncmp(tchProcPathDev, tchVolNameDev, _tcslen(tchVolNameDev)) == 0)	//进程映象名和
			{
				bRet = true;
				break;
			}
		}

		if (bRet)
		{
			_tcscat_s(lpProcPath, ulProcPathBufLen / sizeof(I8), tchVolName);	//盘符
			_tcscat_s(lpProcPath, ulProcPathBufLen / sizeof(I8), tchProcPathDev + _tcslen(tchVolNameDev));	//盘符后的路径
		}
	}
	__finally
	{
		if (hModule)
			FreeLibrary(hModule);

		if (hProc)
			CloseHandle(hProc);
	}

	return bRet;
}

//遍历进程相关
 HANDLE CreateProcessSnapshot()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return NULL;
	}

	return hSnapshot;
}

I32 GetFirstProcessPid(HANDLE& hSnapshot)
{
	CHECKF_NL(hSnapshot);
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(pe);

	CHECKF_NL(Process32First(hSnapshot, &pe));

	return pe.th32ProcessID;
}

I32 GetNextProcessPid(HANDLE& hSnapshot)
{
	CHECKF_NL(hSnapshot);
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(pe);
	if (!Process32Next(hSnapshot, &pe))
		return 0;

	return pe.th32ProcessID;
}


///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	EnumWindowsArg *pArg = (EnumWindowsArg *)lParam;
	DWORD  dwProcessID = 0;
	// 通过窗口句柄取得进程ID
	::GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (dwProcessID == pArg->dwProcessID)
	{
		pArg->hwndWindow = hwnd;
		// 找到了返回FALSE
		return FALSE;
	}
	// 没找到，继续找，返回TRUE
	return TRUE;
}

///< 通过进程ID获取窗口句柄
HWND GetWindowHwndByPID(DWORD dwProcessID)
{
	HWND hwndRet = NULL;
	EnumWindowsArg ewa;
	ewa.dwProcessID = dwProcessID;
	ewa.hwndWindow = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
	if (ewa.hwndWindow)
	{
		hwndRet = ewa.hwndWindow;
	}
	return hwndRet;
}


NAMESPACE_END(gtool)

#endif
