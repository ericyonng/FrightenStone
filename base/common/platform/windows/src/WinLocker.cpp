#include "stdafx.h"
#include <BaseCode/MyBaseMacro.h>
#include "BaseCode/Win/WinLocker.h"
#include "BaseCode/MyDataType.h"
#include <exception>
#include <thread>
#include <BaseCode/CheckNoLog.h>
#include <BaseCode/enumtype.h>
#include <iostream>
// #include <process.h>
// #include<afxmt.h>

///////////////////////////////////////////////////////////////////////////////////

#if defined(NEED_WINDOWS_COMMON_API)


#ifndef ENTER_SECTION_CIRCULATION_COUNT
#define ENTER_SECTION_CIRCULATION_COUNT 4000		//是否允许进入关键段的轮询次数
#endif
//////////////////////////////////////////////////////////////////////////////////


CDataLocker* CDataLocker::CreateNew(bool bCreateEvent)
{
	return new CDataLocker(bCreateEvent);
}

int CDataLocker::Release()
{
	try
	{
		ASSERT(CloseEvent());
		delete this;
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
	}
	catch (...)
	{
		ASSERT(!"CDataLocker::Release()");
	}

	return 0;
}

//共享资源锁
//构造函数
CDataLocker::CDataLocker(bool bCreateEvent)
{
	//设置变量
	m_nLockCount = 0;
	m_hEvent = NULL;
	if (bCreateEvent)
		ASSERT(CreateEventDef());

	return;
}

//析构函数
CDataLocker::~CDataLocker()
{
	try
	{
		//解除锁定
		while (m_nLockCount > 0)
		{
			IF_NOT_NL(LoopUnLock())  break;
		}

		ASSERT(CloseEvent());
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
	}
	catch (...)
	{
		ASSERT(!"CDataLocker::~CDataLocker()");
	}
}

//锁定函数
bool CDataLocker::LoopLock()
{
	bool bRet = false;
	try
	{
		m_nLockCount++;
		bRet = m_CSLockerObj.Lock();

		if (bRet == false)
		{
			m_nLockCount--;
		}
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
		return false;
	}
	catch (...)
	{
		ASSERT(!"CDataLocker::LoopLock()");
		return false;

	}
	return bRet;
}

//解锁函数
bool CDataLocker::LoopUnLock()
{
	auto bRet = true;
	try
	{
		//效验状态
		if (m_nLockCount <= 0)
		{
			return RES_UN_DO;
		}

		//设置变量
		bRet = m_CSLockerObj.Unlock();

		if (bRet == true)
		{
			m_nLockCount--;
		}
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
		return false;
	}
	catch (...)
	{
		ASSERT(!"CDataLocker::LoopUnLock()");
		return false;
	}

	return bRet;
}

bool CDataLocker::Lock()
{
	//m_nLockCount++;
	return  m_CSLockerObj.Lock();
}

bool CDataLocker::UnLock()
{
	//ASSERT(m_nLockCount >= 0);
	//m_nLockCount--;
	return m_CSLockerObj.Unlock();
}

bool CDataLocker::CreateEventDef()
{
	m_hEvent = CreateEvent(NULL, true, false, NULL);
	CHECKF_NL(m_hEvent);

	return true;
}

WIN_WAIT_EVENT CDataLocker::WaitEvent(const unsigned long ulMilliseconds/*=INFINITE*/)
{
	I64 i64WaitRet = WAIT_OBJECT_0;
	I64 nOldWaitVal = m_nWaitCount;
	I64 nOldWakeVal = m_nWaitCount;
	bool bSucWait = false;
	DOTRY_B_NL
	{
		while (!m_bSinal)
		{
			IF_NOT_NL(m_hEvent)
			{
				m_bSinal = false;
				return WIN_WAIT_EVENT_FAILED;
			}			

			IF_NOT_NL(UnLock())
			{
				m_bSinal = false;
				return WIN_WAIT_EVENT_FAILED;
			}

			//	std::cout << std::endl << "Waitbegin:" << std::this_thread::get_id() << std::endl;
			void *pEvent = m_hEvent;
			m_nWaitCount++;
			bSucWait = true;
			i64WaitRet = WaitForMultipleObjects(1, &pEvent, true, ulMilliseconds);


			ASSERT(Lock());
			ASSERT(ResetEvent(m_hEvent));

			nOldWaitVal = m_nWaitCount;
			m_nWaitCount--;
			nOldWakeVal = m_nWaitCount;

			ASSERT(m_nWaitCount >= 0);

			IF_NOT_NL(i64WaitRet != WAIT_FAILED)
			{
				m_bSinal = false;
				return WIN_WAIT_EVENT_FAILED;
			}

			//没有被人唤醒
			if(!m_bSinal&&ulMilliseconds!= INFINITE)
				break;
		}	

		m_bSinal = false;
	}
	DOCATCH_ANY_NL("WaitEvent CRASH!")
	{
		i64WaitRet = WIN_WAIT_EVENT_FAILED;
		m_bSinal = false;
		if (bSucWait &&(nOldWaitVal >= nOldWakeVal))
			m_nWaitCount--;

		return WIN_WAIT_EVENT_FAILED;
	}
	DOCATCH_ANY_NL_END

		if ((I64(WAIT_OBJECT_0) <= i64WaitRet)
			&& (i64WaitRet <= I64(MAXIMUM_WAIT_OBJECTS + WAIT_OBJECT_0)))
		{
			return WIN_WAIT_EVENT_SUC;
		}
		else if (i64WaitRet == WAIT_TIMEOUT)
		{
			return WIN_WAIT_EVENT_TIMEOUT;
		}

	ASSERT(!"i64WaitRet");

	return WIN_WAIT_EVENT_FAILED;
}

bool CDataLocker::SinalOne()	//线程安全的
{
	bool bRet = false;
	DOTRY_B_NL
	{
		CHECKF_NL(m_hEvent);
	m_bSinal = true;
	bRet = SetEvent(m_hEvent);
	if (!bRet)
		m_bSinal = false;
	}
		DOCATCH_ANY_NL("SinalOne CRASH!")
	{
		return false;
	}
	DOCATCH_ANY_NL_END

		return bRet;
}

bool CDataLocker::Broadcast()
{
	while(m_nWaitCount > 0)
	{
		IF_NOT_NL(Lock())
			continue;
		ASSERT(SinalOne());
		ASSERT(UnLock());
	}

	return true;
}

// bool CDataLocker::BroadcastSinal()	//无效
// {
// 	DOTRY_B_NL
// 	{
// 		CHECKF_NL(m_hEvent);
// 		ASSERT(Lock() == RES_SUCCESS);
// 		bool bRet= PulseEvent(m_hEvent);
// 		ASSERT(UnLock() == RES_SUCCESS);
// 		
// 		return	bRet;
// 	}
// 	DOCATCH_ANY_NL("BroadcastSinal CRASH!")
// 	{
// 		return false;
// 	}
// 	DOCATCH_ANY_NL_END
// 
// 	return true;
// }

bool CDataLocker::CloseEvent()
{
	DOTRY_B_NL
	{
		if (m_hEvent == NULL) return true;
	CHECKF_NL(CloseHandle(m_hEvent));
	m_hEvent = NULL;
	}
		DOCATCH_ANY_NL("CloseEvent CRASH!")
	{
		return false;
	}
	DOCATCH_ANY_NL_END

		return true;
}

bool CDataLocker::Islock()
{
	return m_CSLockerObj.Islock();
}

void CDataLocker::YieldScheduler()
{
	m_CSLockerObj.YieldScheduler();
}

//锁定次数
int CDataLocker::GetLockCount()
{

	int nCount = 0;
	try
	{
		nCount = m_nLockCount;
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
	}
	catch (...)
	{
		ASSERT(!"CDataLocker::GetLockCount()");
	}

	return nCount;
}

CCriticalSectionProtect* CCriticalSectionProtect::CreateNew()
{
	return new CCriticalSectionProtect;
}

int CCriticalSectionProtect::Release()
{
	try
	{
		delete this;
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
	}
	catch (...)
	{
		ASSERT(!"CCriticalSectionProtect::Release()");
	}
	return 0;
}

CCriticalSectionProtect::CCriticalSectionProtect()
{
	ASSERT(Init());
}

CCriticalSectionProtect::~CCriticalSectionProtect()
{
	try
	{
		DeleteCriticalSection(&m_CritSec);
		m_bInit = false;
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
		m_bInit = false;
	}
	catch (...)
	{
		ASSERT(!"CCriticalSectionProtect::~CCriticalSectionProtect()");
		m_bInit = false;
	}
}

bool CCriticalSectionProtect::Lock()
{
	try
	{
		if (!IsInit())
		{
			ASSERT(Init());
		}
		EnterCriticalSection(&m_CritSec);
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
		return false;
	}
	catch (...)
	{
		ASSERT(!"CCriticalSectionProtect::Lock()");
		return false;
	}

	return true;
}

bool CCriticalSectionProtect::Unlock()
{
	try
	{
		LeaveCriticalSection(&m_CritSec);
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
		return false;
	}
	catch (...)
	{
		ASSERT(!"CCriticalSectionProtect::Unlock()");
		return false;
	}

	return true;
}

bool CCriticalSectionProtect::TryLock()
{
	try
	{
		if (!IsInit())
		{
			ASSERT(Init());
		}
		return TryEnterCriticalSection(&m_CritSec);
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
		return false;
	}
	catch (...)
	{
		ASSERT(!"CCriticalSectionProtect::Lock()");
		return false;
	}

	return true;
}

bool CCriticalSectionProtect::Islock()
{
	HANDLE hCurThd = ULongToHandle(GetCurrentThreadId());
	return m_CritSec.OwningThread != NULL && m_CritSec.OwningThread != hCurThd;
}

bool CCriticalSectionProtect::IsInit()
{
	return m_bInit;
}

bool CCriticalSectionProtect::Init()
{
	try
	{
		DWORD dwSipnCount = DWORD(1 << (gtool::GetDataTypeBitWidth<DWORD>() - 1)) | (DWORD)(ENTER_SECTION_CIRCULATION_COUNT);	//最高位置1使内核对象进程加载时创建，避免内存不足时候创建失败
	//	DWORD dwSipnCount = DWORD(1 << (gtool::GetDataTypeStrWidth<DWORD>() - 1)) | (DWORD)(ENTER_SECTION_CIRCULATION_COUNT);	//最高位置1使内核对象进程加载时创建，避免内存不足时候创建失败

		ASSERT(InitializeCriticalSectionAndSpinCount(&m_CritSec, dwSipnCount));	//设置循环锁减少加锁时付出的上下文切换代价（上千个时钟周期）
																									//InitializeCriticalSection(&m_CritSec);																//InitializeCriticalSection(&m_CritSec);
		m_bInit = true;
	}
	catch (std::exception &e)
	{
		ASSERT(!(e.what()));
		return false;
	}
	catch (...)
	{
		ASSERT(!"CCriticalSectionProtect::CCriticalSectionProtect()");
		return false;
	}

	return true;
}

void CCriticalSectionProtect::YieldScheduler()
{
	std::this_thread::yield();
}

#endif

