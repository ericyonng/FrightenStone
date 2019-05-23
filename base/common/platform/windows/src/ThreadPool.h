#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#pragma once

// #include "BaseCode/MyDataType.h"
// #include <BaseCode/SmartPtr.hpp>
// #include <BaseCode/TPtrList.hpp>

//任务基类
class CTaskBase;
class CDataLocker;


#if defined(NEED_WINDOWS_COMMON_API)

//广播状态
// #define CAST_UN_DO		0											//无动作
// #define CASTED_ON		1											//通知
// #define CASTED_OFF		2											//关闭

//线程处理函数参数
#define		TIME_SLEEP_PER_TASK			10		//ms 线程空闲睡眠时间
#define		NO_TASK_BREAK_COUNT			10		//轮休无任务跳出次数

//多线程支持
#include <process.h> 
//#include<afxmt.h> 临界区


//线程池
class  CThreadPool
{
	NO_COPY_NEED_RVALUE(CThreadPool);

	//类型定义
public:
	typedef gtool::TPtrList<CTaskBase, DEL_WAY_RELEASE, boost::is_class<CTaskBase>::value> TASK_LIST;
	typedef char STR_BUF[1024];
#define POOL_DEF_CACHE_SIZE (128*1024)
#define POOL_DEF_CACHE_RE_PAUSE_PERCENTAGE	2000			//万分比

	//实例化接口
protected:
	//构造函数
	CThreadPool(int nMinNum, int nMaxNum);
	//构造函数
	CThreadPool();
	//析构函数
	virtual ~CThreadPool();
public:
	static CThreadPool* CreateNew(int nMinNum, int nMaxNum) { return new CThreadPool(nMinNum, nMaxNum); }
	static CThreadPool* CreateNew() { return new CThreadPool(); }
	//释放对象
	virtual int Release() { ClearPool(); delete this; return 0; }

	//线程池参数
protected:
	boost::atomic<I32>									m_nMinNum{ 0 };						//最小线程数
	boost::atomic<I32>									m_nMaxNum{ 0 };						//最大线程数
	boost::atomic<I32>									m_nCurNum{ 0 };						//当前总的线程数
	boost::atomic<I32>									m_nWaitNum{0};						//等待线程数
	boost::atomic_bool									m_bIsDestroy{ false };					//线程池是否销毁

																						//任务
protected:
	boost::atomic_bool									m_bIsStopAdd{ false };					//停止添加任务
	volatile	bool									m_bEnableCache=false;					//使用缓冲不立即执行机制
	volatile	U64										m_nCacheUsed=0;					//缓冲使用情况
	volatile	U64										m_nCacheTopLimit=0;				//最大缓冲尺寸
	volatile	U16										m_nRePauseCachePercentage=0;		//执行后再次停止缓冲比例 万分比
	boost::atomic_bool									m_bClearPool{ false };
	gtool::CSmartPtr<CDataLocker>							m_pCSynLockerObj=std::move(gtool::CSmartPtr<CDataLocker>());				//多线程任务队列同步
	mutable gtool::CSmartPtr<TASK_LIST>								m_plistTasklist=std::move(gtool::CSmartPtr<TASK_LIST>(NULL));				//任务队列
																								//list<stThreadPara *>								m_listThreadHandle;				//线程资源标识

	//回收
protected:
	//CDataLocker *										m_pRecycleThreadObj;			//线程回收同步
	void ClearPool();


	//线程操作
public:
	//只能增加无法减少
	void SetThreadNumLimit(const I32& nMin, const I32& nMax);
	//创建线程 需要加锁保护
	bool CreateThread(int nThreadNum);
	//线程处理函数
	static unsigned __stdcall ThreadHandler(void* pThreadData);

	//任务
public:
	//结束线程池
	void Destroy();
	//添加任务
	bool AddTask(CTaskBase &rTask, int iThreadPerNum = 1, const bool bCreateNewThread=false);
	//停止添加任务
	int StopAdd();
	//使能添加任务
	int EnAbleAdd();
	void DeductCacheByte(U64 u64Byte);
	void AddCacheByte(U64 u64Byte);
	const U64 GetCurCacheUsed();
	const U64 GetCacheTopLimit();
	void SetCacheTopLimit(U64 u64Byte);
	void SetEnableCache(bool bEnable = false);
	bool GetEnableCache();
	U16 GetRePauseCachePercentage();

	//辅助
protected:
	int CheckTask(bool &bDestroy, int &iThreadPerNum);

	//同步对象
public:
	//线程同步类对象
	CDataLocker *GetSynLockerObj() { return m_pCSynLockerObj; }
	TASK_LIST* GetTaskList() const;

	bool Lock();
	bool UnLock();
};


#endif


#endif
