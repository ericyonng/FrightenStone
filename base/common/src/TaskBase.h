#ifndef BASE_TASK_H_
#define BASE_TASK_H_
#pragma once
//#include <BaseCode/BaseCodeApi.h>
// #include <BaseCode/MyBaseMacro.h>
// #include <BaseCode/myclassmacro.h>
// #include <BaseCode/MyDataType.h>

//任务基类
class CTaskBase
{
	NO_COPY(CTaskBase);
public:
	//任务执行体
	virtual int Run() = 0;
	//释放
	virtual int Release() { delete this; return 0; }
	//设置参数
	virtual void SetArg(void *pArg) { m_pArg = pArg; }
	//获取参数
	virtual void *GetArg() { return m_pArg; }
	//当前任务缓冲大小
	virtual U64 GetCacheSize() { return 0; }

	//函数定义
protected:
	//构造函数
	CTaskBase() { m_pArg = NULL; }
	//析构函数
	virtual ~CTaskBase() {}

protected:
	void *	m_pArg=NULL;
};


//任务类映射表模块
// #undef DECLEAR_BASE_TASK_RUN_MAP_BEGIN
// #define DECLEAR_BASE_TASK_RUN_MAP_BEGIN(TaskMissonName, line) \
// 	namespace __FILE__##TaskMissonName##TASK_SPACE##__{ \
// 			template<int i>\
// 			struct stTaskPtrInfo\
// 			{\
// 				static long tasknum;\
// 				stTaskPtrInfo(long ltasknum) :{ tasknum=ltasknum;}\
// 			};\
// 	template<int i>\
// 	long stTaskPtrInfo<i>::tasknum=0;\
// 	static const stTaskPtrInfo<0> _##TaskMissonName##_##line##_(-1);\
// 	const stTaskPtrInfo<0> g_TaskMapArray[]={\
// 
// #undef DECLEAR_TASK_MAP_ADD
// #define DECLEAR_TASK_MAP_ADD(ClassName)	\
// 					(stTaskPtrInfo<0>(stTaskPtrInfo<0>::tasknum+1),
// #undef DECLEAR_BASE_TASK_RUN_MAP_END
// #define DECLEAR_BASE_TASK_RUN_MAP_END(TaskMissonName)	\
// 			(stTaskPtrInfo<0 >(stTaskPtrInfo<0>::tasknum, NULL)) };} using namespace __FILE__##TaskMissonName##TASK_SPACE##__;
// 
// #undef GET_TASK_MAP_ARRAY_SIZE
// #define GET_TASK_MAP_ARRAY_SIZE(TaskMissonName)	(__FILE__##TaskMissonName##TASK_SPACE##__::stTaskPtrInfo<0 >::tasknum+1)
// #undef G_TASK_MAP_ARRAY
// #define G_TASK_MAP_ARRAY(TaskMissonName) (__FILE__##TaskMissonName##TASK_SPACE##__::g_TaskMapArray)

#endif