#ifndef WIN_LOCKER_H_
#define WIN_LOCKER_H_

#pragma once
//#include <windows.h>
//#include <BaseCode/myclassmacro.h>
//#include<BaseCode/SmartPtr.hpp>
enum WIN_WAIT_EVENT;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(NEED_WINDOWS_COMMON_API)

//同一线程内多次调用lock（循环或者递归调用）不会被锁定，锁只限制不同线程间，关键段获取锁只区分不同线程要unlock需要线程获取锁
//需要在全局区
class  CCriticalSectionProtect
{
	NO_COPY(CCriticalSectionProtect);

public:
	static CCriticalSectionProtect* CreateNew();
	//释放对象
	virtual int Release();
	CCriticalSectionProtect();

	virtual ~CCriticalSectionProtect();

	bool Lock();						
	bool Unlock();
	bool TryLock();
	bool Islock();
	bool IsInit();
	bool Init();
	void YieldScheduler();

private:
	typename CRITICAL_SECTION    m_CritSec;
	boost::atomic<bool>			m_bInit{ false };
};

//数据锁
class CDataLocker
{
	NO_COPY(CDataLocker);
	
	//变量定义
protected:
	boost::atomic<I64>								m_nLockCount{ 0 };				//锁定计数
	boost::atomic<I64>								m_nWaitCount{ 0 };				//挂起数量
	CCriticalSectionProtect							m_CSLockerObj;					//临界区
	//void *											m_hEvent;						//事件句柄
	boost::atomic_address							m_hEvent;
	boost::atomic_bool								m_bSinal{ false };				//是否发送信号
			
	//函数定义
public:
	static CDataLocker* CreateNew(bool bCreateEvent = false);
	//释放对象
	virtual int	Release();
	//构造函数
	CDataLocker(bool bCreateEvent=false);
	//析构函数
	virtual ~CDataLocker();

	//操作函数
public:
	//锁定函数
	bool LoopLock();
	//解锁函数 
	bool LoopUnLock();

	//非循环计数
public:
	bool Lock();
	bool UnLock();

	//事件等待
public:

	//匿名事件，手动，初始化不触发事件
	bool CreateEventDef();	
	WIN_WAIT_EVENT WaitEvent(const unsigned long ulMilliseconds=INFINITE); //传入微妙 线程不安全需要锁
	bool SinalOne();	//线程安全的
	bool Broadcast();	//唤醒所有线程
	bool BroadcastSinal()=delete;	//无效
	bool CloseEvent();

	//锁状态
public:
	bool Islock();
	void YieldScheduler();

	//状态函数
public:
	//锁定次数
	int GetLockCount();
};

#endif

#endif
