#include<BaseCode/Win/Cpu.h>

#if defined(NEED_WINDOWS_COMMON_API)

NAMESPACE_BEGIN(gtool)
bool		CCpuInfo::Startup()
{
	CHECKF_NL(GetSystemTimes(&m_preidleTime, &m_prekernelTime, &m_preuserTime));

	m_bInit = true;
	return true;
}

I64	CCpuInfo::CompareFileTime(FILETIME time1, FILETIME time2)
{
	I64 a = (I64(time1.dwHighDateTime) << I64(32)) | I64(time1.dwLowDateTime);
	I64 b = (I64(time2.dwHighDateTime) << I64(32)) | I64(time2.dwLowDateTime);

	return   abs(b - a);
}


MYDOUBLE CCpuInfo::CpuUsage()
{
	IF_NOT_NL(m_bInit)
		return (std::numeric_limits<MYDOUBLE>::min)();

	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	IF_NOT_NL(GetSystemTimes(&idleTime, &kernelTime, &userTime))
		return 0.0;

	I64 i64IdleTime = CompareFileTime(m_preidleTime, idleTime);			//空闲时间
	I64 i64KernelTime = CompareFileTime(m_prekernelTime, kernelTime);		//内核时间
	I64 i64UserTime = CompareFileTime(m_preuserTime, userTime);			//用户时间

																		//总时间
	if (i64KernelTime + i64UserTime == 0)
		return (std::numeric_limits<MYDOUBLE>::min)();

	//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
	double lfCpuUsage = MYDOUBLE(i64KernelTime + i64UserTime - i64IdleTime) * 100 / (i64KernelTime + i64UserTime);

	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime;
	return lfCpuUsage;
}

NAMESPACE_END(gtool)

#else
//非windows代码

#endif