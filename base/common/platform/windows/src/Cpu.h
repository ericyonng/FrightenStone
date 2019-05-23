#ifndef CPU_H_
#define CPU_H_

#pragma once

#if defined(NEED_WINDOWS_COMMON_API)

NAMESPACE_BEGIN(gtool)

class CCpuInfo
{
	NO_COPY_NO_MOVE(CCpuInfo);

public:
	CCpuInfo() {}
	~CCpuInfo() {}

public:
	bool		Startup();
	MYDOUBLE	CpuUsage();
	bool		IsIdleStatus(MYDOUBLE IdelVal = MEM_POOL_CPU_IDLE_VAL)
	{
		return CpuUsage() < IdelVal;
	}

	//ÄÚ²¿
protected:
	I64			CompareFileTime(FILETIME time1, FILETIME time2);

private:
	FILETIME		m_preidleTime = { 0 };
	FILETIME		m_prekernelTime = { 0 };
	FILETIME		m_preuserTime = { 0 };
	boost::atomic_bool			m_bInit{ false };
};

NAMESPACE_END(gtool)

#else

#endif

#endif


