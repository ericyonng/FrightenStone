#ifndef THREAD_POST_HELPER_H_
#define THREAD_POST_HELPER_H_

#pragma once
#include <BaseCode/DataCheckMacro.h>

#pragma MEM_ALIGN_BEGIN(1)

//实现线程间数据传递
//携带list容器
//锁
//post时候发送sinal  并wait到 目的线程接收然后退出while循环
//目标线程wait 直到接收到第一个sinal醒来并设置awake位，并将list中的数据迁移 执行数据业务
typedef void(*FunDisplay)(std::string str, bool bNeedPreSym);

NAMESPACE_BEGIN(gtool)

struct  ThreadDataMsg
{
	U64 u64Param;		//若没有填0	//当套接字使用时候需要判断有效性 
	I64 nLen;
	char rData[0];		//可以为PackData.Stream 等的纯数据
};

template<typename T>
class  CThreadDataTransfer
{
public:
	typedef bool(T::*fnRECV_HANDLE_MSG)( const U64 u64Param, const void *pData, const I64 nSize); //pData为堆区创建，但无需再次用智能指针对象持有因为已经被持有了
private:
	CThreadDataTransfer(const CThreadDataTransfer<T>&);	
	const CThreadDataTransfer<T>& operator=(const CThreadDataTransfer<T>&); 
	CThreadDataTransfer(CThreadDataTransfer<T>&&);

public:
	CThreadDataTransfer();
	virtual ~CThreadDataTransfer();
	static CThreadDataTransfer<T>* CreateNew()
	{
		return new CThreadDataTransfer<T>();
	}
	virtual int Release() { Cleans(); delete this; return 0; } 
	virtual void Cleans();

public:	
	void SetRecvObj(const void* pObj) 
	{
		T* p = (T*)pObj;
		ASSERTEX(p);
		if (!m_pObj) m_pObj = p;
		ASSERTEX(p == m_pObj);
	}
	const void *GetRecvObj() { T* p = m_pObj; return p; }
	bool IsMsgQueueEmpty(const void* pObj)
	{
		T* p = (T*)pObj;
		ASSERTEX(p);
		ASSERTEX(p == m_pObj);
		IF_NOT(m_plckGuard)
			return true;
		IF_NOT(m_pMsgList)
			return true;
				
		ASSERTEX(m_plckGuard->Lock());
		auto bEmpty = m_pMsgList->IsEmpty();
		ASSERTEX(m_plckGuard->UnLock());
		return bEmpty;
	}

public:
	static CThreadDataTransfer<T>* CreateNew(fnRECV_HANDLE_MSG fnHandleMsg)
	{
		return new CThreadDataTransfer<T>(fnHandleMsg);
	}

	CThreadDataTransfer(fnRECV_HANDLE_MSG fnHandleMsg) :m_pfnRECV_HANDLE_MSG(fnHandleMsg)
	{
		InitObj();
	}

	void SetfnRecvHandleMsg(fnRECV_HANDLE_MSG fnHandleMsg) { m_pfnRECV_HANDLE_MSG = fnHandleMsg;	}

	//同步
public:
	bool Lock();	//发送给接收者
	bool Send2RecvMsg(const U64 u64Param, const void *pData, const I64 nSize);//线程安全
	bool RecvWaitAndHandleMsg();
	bool Unlock();
	void SetEnableSend(const bool bEnable) { m_bAllowSend = bEnable; }

	bool GetRecvAwake()  const { return m_bRecvAwake; }
	void SetRecvAwake(const bool bAwake = true) { m_bRecvAwake = bAwake; }

	//等待消息处理结束
	bool WaitForFini(bool bBreakLoopFinal = false);

protected:
	bool HandleRecv(const U64 u64Param, const void *pData, const I64 nSize);
	void InitObj();

	//同步
private:
	gtool::CSmartPtr<CDataLocker> m_plckGuard;	//同步

	typedef gtool::TPtrList<CXByte, DEL_WAY_DELETE_MULTI, false> MsgList;		//消息队列
	gtool::CSmartPtr<MsgList>	  m_pMsgList = NULL;

	boost::atomic_bool			m_bRecvAwake{ false };
	boost::atomic_bool			m_bInit{ false };
	boost::atomic_bool			m_bAllowSend{ true };
	I32							m_nLoopMax = LOOP_MAX_DEF;
	boost::atomic<fnRECV_HANDLE_MSG>	m_pfnRECV_HANDLE_MSG{ NULL };
	boost::atomic<T*>			m_pObj{ NULL };
};




template<typename T>
CThreadDataTransfer<T>::CThreadDataTransfer()
{
	InitObj();
}

template<typename T>
CThreadDataTransfer<T>::~CThreadDataTransfer()
{
}

template<typename T>
void CThreadDataTransfer<T>::Cleans()
{
	CHECK(m_plckGuard);
	ASSERTEX(m_bInit);
	m_bAllowSend = false;
	ASSERTEX(m_plckGuard->Broadcast());

	IF_OK(m_pMsgList)
	{
		ASSERTEX(m_pMsgList->IsEmpty());
		ASSERTEX(WaitForFini(true));
	}

	m_bInit = false;
	IF_OK(m_plckGuard)
	{
		m_plckGuard->Lock();
	}

	IF_OK(m_pMsgList)
	{
		ASSERTEX(m_pMsgList->IsEmpty());
		m_pMsgList->Clear();
	}

	IF_OK(m_plckGuard)
	{
		m_plckGuard->UnLock();
	}

}


template<typename T>
void CThreadDataTransfer<T>::InitObj()
{
	m_plckGuard = CDataLocker::CreateNew(true);
	m_pMsgList = MsgList::CreateNew();
	m_bAllowSend = true;
	m_bInit = true;
}

template<typename T>
bool CThreadDataTransfer<T>::WaitForFini(bool bBreakLoopFinal)
{
	IF_NOT(m_pMsgList)
		return true;

	bool bEmpty = false;
	I32 nLoopCount = 0;
	const auto nLoopMax = m_nLoopMax;
	while (!bEmpty)
	{
		if (bBreakLoopFinal)
		{
			DEAD_LOOP_LOG_BREAK(nLoopCount, nLoopMax);
		}
		else
		{
			DEAD_LOOP_LOG(nLoopCount, nLoopMax);
		}		

		Lock();
		bEmpty = m_pMsgList->IsEmpty();
		Unlock();
		Sleep(1);
	}

	return true;
}

template<typename T>
bool CThreadDataTransfer<T>::Lock()
{
	CHECKF(m_plckGuard);
	return m_plckGuard->Lock();
}

template<typename T>
bool CThreadDataTransfer<T>::Send2RecvMsg(const U64 u64Param, const void *pData, const I64 nSize) //线程安全
{
	CHECKF(m_bAllowSend);
	CHECKF(m_plckGuard);
	CHECKF(m_bInit);
	CHECKF(m_pMsgList);

	//转移数据
	gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> pStream = new CXByte[sizeof(ThreadDataMsg) + nSize];
	CHECKF(pStream);
	ThreadDataMsg *pMsgData = (ThreadDataMsg *)((CXByte *)pStream);
	pMsgData->nLen = nSize;
	if(pData)
		memmove(pMsgData->rData, pData, pMsgData->nLen);

	pMsgData->u64Param = u64Param;

	//数据进入队列
	CHECKF(m_plckGuard->Lock());
	ASSERTEX(m_pMsgList->PushBack(pStream.pop()));
	ASSERTEX(m_plckGuard->SinalOne());
	ASSERTEX(m_plckGuard->UnLock());

	return true;
}


template<typename T>
bool CThreadDataTransfer<T>::RecvWaitAndHandleMsg()
{
	CHECKF(m_plckGuard);
	CHECKF(m_pMsgList);

	CHECKF(m_plckGuard->Lock());

	if (!m_pMsgList->IsEmpty())
	{
		//处理任务
		gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> pStream = (CXByte *)(m_pMsgList->PopFront());
		ThreadDataMsg *pMsgData= (ThreadDataMsg *)((CXByte *)pStream);
		ASSERTEX(m_plckGuard->UnLock());
		CHECKF(pMsgData);
		ASSERTEX(HandleRecv(pMsgData->u64Param, pMsgData->rData, pMsgData->nLen));
	}
	else
	{
		//挂起
		m_bRecvAwake = false;
		ASSERTEX(m_plckGuard->WaitEvent() != WIN_WAIT_EVENT_FAILED);
		m_bRecvAwake = true;

		//处理任务
		ThreadDataMsg *pMsgData = NULL;
		gtool::CSmartPtrC<CXByte, CXByte, DEL_WAY_DELETE_MULTI> pStream = NULL;
		if (!m_pMsgList->IsEmpty())
		{
			pStream = (CXByte *)(m_pMsgList->PopFront());
			pMsgData = (ThreadDataMsg *)((CXByte *)pStream);
		}

		ASSERTEX(m_plckGuard->UnLock());
		if(pMsgData)
			ASSERTEX(HandleRecv(pMsgData->u64Param, pMsgData->rData, pMsgData->nLen));
	}

	return true;
}

template<typename T>
bool CThreadDataTransfer<T>::Unlock()
{
	CHECKF(m_plckGuard);
	return m_plckGuard->UnLock();
}

template<typename T>
bool CThreadDataTransfer<T>::HandleRecv(const U64 u64Param, const void *pData, const I64 nSize)
{
	CHECKF(m_pfnRECV_HANDLE_MSG);
	CHECKF(m_pObj);
	T* p = m_pObj;
	fnRECV_HANDLE_MSG pCallBackHandleMsg = m_pfnRECV_HANDLE_MSG;
	return (p->*pCallBackHandleMsg)(u64Param, pData, nSize);
}


NAMESPACE_END(gtool)


template <int i>
struct  ThreadPostHelper
{
	static void SetRecvObj(const void *pObj) { UNUSED_ARG(pObj); }
	static const void *GetRecvObj(){ return (void *)(NULL); }
	static bool Init() { return true; }
	static bool Fini() { return true; }
};

template<int i>
struct  ThreadPostExcludeCommentHelper
{
	enum { realvalue = ThreadPostExcludeCommentHelper<i - 1>::realvalue };
};

template<>
struct   ThreadPostExcludeCommentHelper<0>
{
	enum { realvalue = 0 };
};


#define DECLEAR_THREAD_POST_BEGIN(THREAD_POST_BEGIN, line, indexbase) \
	 enum {THREAD_POST_BEGIN = indexbase};\
	template<>\
struct  ThreadPostExcludeCommentHelper<line>\
{\
	enum {realvalue = indexbase-1};\
};\
NAMESPACE_BEGIN(gtool)\
template<POST_OBJ_TYPE eType>\
struct  InnerPost\
{\
	static bool Post(const void *pObj, const U64 u64Param, const void *pData, const I64 nSize)\
	{\
		return false;\
	}\
\
	static bool IsMsgQueueEmpty(const void *pObj)\
	{\
		return true;\
	}\
\
	static bool WaitAndHandleMsg(const void *pObj)\
	{\
		return false;\
	}\
\
	static bool SetThreadAwake(const void *pObj, bool bAwake)\
	{\
		return false;\
	}\
   static bool WaitForFini(const void *pObj, const bool bBreakLoopFinal)\
	{\
		UNUSED_ARG(pObj); \
		return true;\
	}\
	static void SetEnableSend(const void *pObj, const bool bEnable)\
	{\
		UNUSED_ARG(pObj); \
	}\
};\
NAMESPACE_END(gtool)

#define ADD_THREAD_POST(thread_post_class, fnRecvHandleMsg, POST_TYPE_ENUM, base, line) \
	template<>\
struct  ThreadPostExcludeCommentHelper<line>\
{\
	enum {realvalue = ThreadPostExcludeCommentHelper<line-1>::realvalue + 1};\
};\
	static const int _##thread_post_class##_##fnRecvHandleMsg##_ = ThreadPostExcludeCommentHelper<line>::realvalue;\
typedef gtool::CSingleton<gtool::CThreadDataTransfer<thread_post_class>> _##thread_post_class##_##ThreadDataTransferInstance;\
extern boost::atomic_address _g_pThreadDataTransfer_##thread_post_class##_;\
boost::atomic_address  _g_pThreadDataTransfer_##thread_post_class##_{(void *)(_##thread_post_class##_##ThreadDataTransferInstance::GetInstance())};\
	template<>\
struct ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>\
{\
	static gtool::CLitUnmanagedPtr<gtool::CThreadDataTransfer<thread_post_class>> m_pPoster;\
	static boost::atomic_bool m_bIsInit;\
	static bool Init() \
	{ \
		if (_##thread_post_class##_##fnRecvHandleMsg##_ > base)\
			CHECKF(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_-1>::Init()); \
		void *gpPostPtr = _g_pThreadDataTransfer_##thread_post_class##_;\
		m_pPoster =(gtool::CThreadDataTransfer<thread_post_class> *)gpPostPtr;\
		CHECKF(m_pPoster);\
		m_pPoster->SetfnRecvHandleMsg(&thread_post_class::fnRecvHandleMsg);\
		m_bIsInit = true;\
		return true;\
	} \
	static bool Fini()\
	{\
		if(!m_bIsInit)\
			return true;\
		m_bIsInit = false;\
		if (_##thread_post_class##_##fnRecvHandleMsg##_ > base)\
				ASSERTEX(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_-1>::Fini()); \
		IF_OK(m_pPoster)\
			m_pPoster->Cleans();\
		m_pPoster = NULL;\
		return true;\
	}\
	static void SetRecvObj(const void *pObj) \
	{\
		CHECK(m_bIsInit);\
		CHECK(m_pPoster);\
		m_pPoster->SetRecvObj(pObj);\
		ASSERTEX(GetRecvObj() == pObj);\
	}\
	static const void *GetRecvObj()\
	{\
		CHECKF(m_bIsInit);\
		CHECKF(m_pPoster);\
		return m_pPoster->GetRecvObj();\
	}\
};\
gtool::CLitUnmanagedPtr<gtool::CThreadDataTransfer<thread_post_class>> ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster = NULL;\
boost::atomic_bool ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_bIsInit{false};\
NAMESPACE_BEGIN(gtool)\
template<>\
struct InnerPost<POST_TYPE_ENUM>\
{\
	static bool Post(const void *pObj, const U64 u64Param, const void *pData, const I64 nSize)\
	{\
		CHECKF(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_bIsInit);\
		CHECKF(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster);\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::SetRecvObj(pObj);\
		CHECKF(pObj == ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::GetRecvObj());\
		return ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster->Send2RecvMsg(u64Param, pData, nSize);\
	}\
\
	static bool IsMsgQueueEmpty(const void *pObj)\
	{\
		IF_NOT(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_bIsInit)\
			return true;\
\
		IF_NOT(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster)\
			return true;\
\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::SetRecvObj(pObj);\
		IF_NOT(pObj == ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::GetRecvObj())\
			return true;\
\
		return ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster->IsMsgQueueEmpty(pObj);\
	}\
\
	static bool WaitAndHandleMsg(const void *pObj)\
	{\
		CHECKF(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_bIsInit);\
		CHECKF(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster);\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::SetRecvObj(pObj);\
		CHECKF(pObj == ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::GetRecvObj());\
		auto bSuc = ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster->RecvWaitAndHandleMsg();\
		ASSERTEX(bSuc);\
		return bSuc;\
	}\
\
	static bool SetThreadAwake(const void *pObj, bool bAwake)\
	{\
		CHECKF(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_bIsInit);\
		CHECKF(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster);\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::SetRecvObj(pObj);\
		CHECKF(pObj == ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::GetRecvObj());\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster->SetRecvAwake(bAwake);\
\
		return true;\
	}\
	static bool WaitForFini(const void *pObj, const bool bBreakLoopFinal)\
	{\
		IF_NOT(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_bIsInit) \
			return true;\
\
		IF_NOT(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster) \
				return true;\
\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::SetRecvObj(pObj); \
		IF_NOT(pObj == ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::GetRecvObj())\
			return true;\
\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster->WaitForFini(bBreakLoopFinal);\
\
		return true;\
	}\
	static void SetEnableSend(const void *pObj, const bool bEnable)\
	{\
		IF_NOT(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_bIsInit) \
			return;\
\
		IF_NOT(ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster) \
				return;\
\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::SetRecvObj(pObj); \
		IF_NOT(pObj == ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::GetRecvObj())\
			return;\
\
		ThreadPostHelper<_##thread_post_class##_##fnRecvHandleMsg##_>::m_pPoster->SetEnableSend(bEnable);\
\
	}\
};\
NAMESPACE_END(gtool)


#define DECLEAR_THREAD_POST_END(THREAD_POST_NUMBER, line) \
	 enum {THREAD_POST_NUMBER = ThreadPostExcludeCommentHelper<line>::realvalue};

#define DECLEAR_GENERAL_THREAD_POST_BEGIN() DECLEAR_THREAD_POST_BEGIN(THREAD_POST_BEGIN,  __LINE__, 0)
#define GENERAL_ADDTHREAD_POST(thread_post_class,  fnRecvHandleMsg, POST_TYPE_ENUM) ADD_THREAD_POST(thread_post_class, fnRecvHandleMsg,POST_TYPE_ENUM, THREAD_POST_BEGIN, __LINE__)	//继承于thread_post_class 就可以接收fnRecvMsg消息
#define DECLEAR_GENERAL_THREAD_POST_END() DECLEAR_THREAD_POST_END(THREAD_POST_NUM_MAX, __LINE__)

#pragma MEM_ALIGN_END(1)

#endif

