#include <TDPoster/ThreadPostApi.h>
#include <TDPoster/ThreadPostDefine.h>

//////////////////////////////////////////////////////////////////////////

NAMESPACE_BEGIN(THREAD_TRANSFER)



//postthread
#undef DECLEAR_POST_THREAD_BEGIN
#define DECLEAR_POST_THREAD_BEGIN()\
bool Post2Thread(const void * pRecvObj, POST_OBJ_TYPE eType, const U64 u64Param, const void *pData, const I64 nSize)\
{\
switch (eType)\
{\



#undef ADD_POST_THREAD_CASE
#define ADD_POST_THREAD_CASE(POST_TYPE_ENUM)\
case POST_TYPE_ENUM:\
{\
	CHECKF_NL(pRecvObj);\
	return gtool::InnerPost<POST_TYPE_ENUM>::Post(pRecvObj, u64Param, pData, nSize);\
}\
break;\

#undef DECLEAR_POST_THREAD_END
#define DECLEAR_POST_THREAD_END()\
	default:\
ASSERT(false);\
break;\
}\
\
return false;\
}




//IsMsgQueueEmpty
#undef DECLEAR_IS_MSG_QUEUE_EMPTY_BEGIN
#define DECLEAR_IS_MSG_QUEUE_EMPTY_BEGIN()\
bool IsMsgQueueEmpty(const void * pRecvObj, POST_OBJ_TYPE eThreadType)\
{\
switch (eThreadType)\
{\

#undef ADD_IS_MSG_QUEUE_EMPTY_CASE
#define ADD_IS_MSG_QUEUE_EMPTY_CASE(POST_TYPE_ENUM)\
case POST_TYPE_ENUM:\
{\
	IF_NOT_NL(pRecvObj)\
		break;\
\
	return gtool::InnerPost<POST_TYPE_ENUM>::IsMsgQueueEmpty(pRecvObj);\
}\
break;\

#undef DECLEAR_IS_MSG_QUEUE_EMPTY_END
#define DECLEAR_IS_MSG_QUEUE_EMPTY_END()\
	default:\
ASSERT(false);\
break;\
}\
\
return true;\
}


//RecverWaitAndHandle
#undef DECLEAR_RECVER_WAIT_AND_HANDLE_BEGIN
#define DECLEAR_RECVER_WAIT_AND_HANDLE_BEGIN()\
bool RecverWaitAndHandle(const void *pRecvObj, POST_OBJ_TYPE eThreadType)\
{\
switch (eThreadType)\
{\

#undef ADD_RECVER_WAIT_AND_HANDLE_CASE
#define ADD_RECVER_WAIT_AND_HANDLE_CASE(POST_TYPE_ENUM)\
case POST_TYPE_ENUM:\
{\
	IF_NOT_NL(pRecvObj)\
		break;\
\
	return gtool::InnerPost<POST_TYPE_ENUM>::WaitAndHandleMsg(pRecvObj);\
}\
break;\

#undef DECLEAR_RECVER_WAIT_AND_HANDLE_END
#define DECLEAR_RECVER_WAIT_AND_HANDLE_END()\
	default:\
ASSERT(false);\
break;\
}\
\
return false;\
}


//SetThreadAwakeStatus
#undef DECLEAR_SET_THREAD_AWAKE_STATUS_BEGIN
#define DECLEAR_SET_THREAD_AWAKE_STATUS_BEGIN()\
bool SetThreadAwakeStatus(const void *pRecvObj, POST_OBJ_TYPE eThreadType, bool bAwake)\
{\
switch (eThreadType)\
{\

#undef ADD_SET_THREAD_AWAKE_STATUS_CASE
#define ADD_SET_THREAD_AWAKE_STATUS_CASE(POST_TYPE_ENUM)\
case POST_TYPE_ENUM:\
{\
	IF_NOT_NL(pRecvObj)\
		break;\
\
	return gtool::InnerPost<POST_TYPE_ENUM>::SetThreadAwake(pRecvObj, bAwake);\
}\
break;\

#undef DECLEAR_SET_THREAD_AWAKE_STATUS_END
#define DECLEAR_SET_THREAD_AWAKE_STATUS_END()\
	default:\
ASSERT(false);\
break;\
}\
\
return false;\
}

//WaitForFini
#undef DECLEAR_WAIT_FOR_FINI_BEGIN
#define DECLEAR_WAIT_FOR_FINI_BEGIN()\
bool WaitForFini(const void * pRecvObj, POST_OBJ_TYPE eThreadType, const bool bBreakLoopFinal)\
{\
switch (eThreadType)\
{\

#undef ADD_WAIT_FOR_FINI_CASE
#define ADD_WAIT_FOR_FINI_CASE(POST_TYPE_ENUM)\
case POST_TYPE_ENUM:\
{\
	IF_NOT_NL(pRecvObj)\
		break;\
\
	return gtool::InnerPost<POST_TYPE_ENUM>::WaitForFini(pRecvObj, bBreakLoopFinal);\
}\
break;\

#undef DECLEAR_WAIT_FOR_FINI_END
#define DECLEAR_WAIT_FOR_FINI_END()\
	default:\
ASSERT(false);\
break;\
}\
\
return true;\
}


//SetEnablePost
#undef DECLEAR_SET_ENABLE_POST_BEGIN
#define DECLEAR_SET_ENABLE_POST_BEGIN()\
void SetEnablePost(const void *pRecvObj, POST_OBJ_TYPE eThreadType, const bool bEnable)\
{\
switch (eThreadType)\
{\

#undef ADD_SET_ENABLE_POST_CASE
#define ADD_SET_ENABLE_POST_CASE(POST_TYPE_ENUM)\
case POST_TYPE_ENUM:\
{\
	IF_NOT_NL(pRecvObj)\
		break;\
\
	return gtool::InnerPost<POST_TYPE_ENUM>::SetEnableSend(pRecvObj, bEnable);\
}\
break;\

#undef DECLEAR_SET_ENABLE_POST_END
#define DECLEAR_SET_ENABLE_POST_END()\
	default:\
ASSERT(false);\
break;\
}\
\
return;\
}
/////////////////////////////////////////////////////////////////////////////////定义

bool InitThreadPostModule()
{
	return ThreadPostHelper<THREAD_POST_NUM_MAX>::Init();
}

bool FiniThreadPostModule()
{
	return ThreadPostHelper<THREAD_POST_NUM_MAX>::Fini();
}

//postthread 定义
DECLEAR_POST_THREAD_BEGIN()
ADD_POST_THREAD_CASE(POST_OBJ_TYPE_MAIN_THREAD)
ADD_POST_THREAD_CASE(POST_OBJ_TYPE_NETWORK_SENDER)
DECLEAR_POST_THREAD_END()

//IsMsgQueueEmpty 定义
DECLEAR_IS_MSG_QUEUE_EMPTY_BEGIN()
ADD_IS_MSG_QUEUE_EMPTY_CASE(POST_OBJ_TYPE_MAIN_THREAD)
ADD_IS_MSG_QUEUE_EMPTY_CASE(POST_OBJ_TYPE_NETWORK_SENDER)
DECLEAR_IS_MSG_QUEUE_EMPTY_END()

//RecverWait 定义
DECLEAR_RECVER_WAIT_AND_HANDLE_BEGIN()
ADD_RECVER_WAIT_AND_HANDLE_CASE(POST_OBJ_TYPE_MAIN_THREAD)
ADD_RECVER_WAIT_AND_HANDLE_CASE(POST_OBJ_TYPE_NETWORK_SENDER)
DECLEAR_RECVER_WAIT_AND_HANDLE_END()

//SetThreadAwakeStatus 定义
DECLEAR_SET_THREAD_AWAKE_STATUS_BEGIN()
ADD_SET_THREAD_AWAKE_STATUS_CASE(POST_OBJ_TYPE_MAIN_THREAD)
ADD_SET_THREAD_AWAKE_STATUS_CASE(POST_OBJ_TYPE_NETWORK_SENDER)
DECLEAR_SET_THREAD_AWAKE_STATUS_END()

//WaitForFini 定义
DECLEAR_WAIT_FOR_FINI_BEGIN()
ADD_WAIT_FOR_FINI_CASE(POST_OBJ_TYPE_MAIN_THREAD)
ADD_WAIT_FOR_FINI_CASE(POST_OBJ_TYPE_NETWORK_SENDER)
DECLEAR_WAIT_FOR_FINI_END()

//SetEnablePost
DECLEAR_SET_ENABLE_POST_BEGIN()
ADD_SET_ENABLE_POST_CASE(POST_OBJ_TYPE_MAIN_THREAD)
ADD_SET_ENABLE_POST_CASE(POST_OBJ_TYPE_NETWORK_SENDER)
DECLEAR_SET_ENABLE_POST_END()

NAMESPACE_END(THREAD_TRANSFER)



