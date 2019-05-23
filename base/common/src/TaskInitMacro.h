#ifndef TASK_INIT_MACRO_H_
#define TASK_INIT_MACRO_H_

#pragma once

//#include <BaseCode/TaskBase.h>

#undef DECLEAR_BASE_TASK_RUN_BEGIN
#define DECLEAR_BASE_TASK_RUN_BEGIN(TASK_NUM, filename, line, taskindexbase) \
namespace TASK_EXECUTE_HELPER_INIT_##filename##_{\
template< int i>\
struct TaskExecuteHelper\
{\
	enum { realvalue = TaskExecuteHelper< i - 1 >::realvalue };\
};\
\
template<>\
struct TaskExecuteHelper<-1>\
{\
	enum { realvalue = -1 };\
};\
template<int i>\
CTaskBase *MakeTaskObj();\
	enum {TASK_NUM = line};\
	template<>\
struct TaskExecuteHelper<line>\
{\
	enum {realvalue = taskindexbase-1};\
};\
template<>\
CTaskBase *MakeTaskObj<TaskExecuteHelper<line>::realvalue>()\
{\
return NULL; \
}

#undef	ADD_TASK_RUN
#define ADD_TASK_RUN(task_class_name, base, line) \
	template<>\
struct TaskExecuteHelper<line>\
{\
	enum {realvalue =  TaskExecuteHelper<line-1>::realvalue+1};\
};\
	class task_class_name :public CTaskBase	\
	{\
		NO_COPY_NEED_RVALUE(task_class_name);\
	protected:\
		task_class_name() {}\
		virtual ~task_class_name() {}\
	public:\
		static  CTaskBase *CreateNew() {  return new task_class_name();}\
		virtual int Run();\
		\
		virtual int Release() { delete this; return 0; }\
	};\
template<>\
CTaskBase *MakeTaskObj< TaskExecuteHelper<line>::realvalue >()\
{\
	return task_class_name::CreateNew();\
}


#undef DECLEAR_BASE_TASK_RUN_END
#define DECLEAR_BASE_TASK_RUN_END(TASK_NUM,base,filename, line) \
	enum {TASK_NUM = TaskExecuteHelper<line-1>::realvalue};} using namespace  TASK_EXECUTE_HELPER_INIT_##filename##_;

//声明任务类模块宏 
#undef MODULE_DECLEAR_TASK_BEGIN
#define MODULE_DECLEAR_TASK_BEGIN() DECLEAR_BASE_TASK_RUN_BEGIN(TASK_BEGIN, __FUNCDNAME__,  __LINE__, 0)
#undef MODULE_TASK_RUN_ADD
#define MODULE_TASK_RUN_ADD(task_class_name)	ADD_TASK_RUN(task_class_name,  TASK_BEGIN, __LINE__)
#undef MODULE_DECLEAR_TASK_END
#define MODULE_DECLEAR_TASK_END() DECLEAR_BASE_TASK_RUN_END(TASK_MAX,TASK_BEGIN, __FUNCDNAME__, __LINE__)

//任务映射表通用宏
// #undef	 DECLEAR_TEST_TASK_MAP_BEGIN
// #define  DECLEAR_TEST_TASK_MAP_BEGIN()		DECLEAR_BASE_TASK_RUN_MAP_BEGIN(Test, __LINE__)
// #undef  DECLEAR_TEST_TASK_MAP_ADD
// #define DECLEAR_TEST_TASK_MAP_ADD(ClassName) DECLEAR_TASK_MAP_ADD(ClassName)
// 
// #undef DECLEAR_TEST_TASK_MAP_END
// #define DECLEAR_TEST_TASK_MAP_END()		DECLEAR_BASE_TASK_RUN_MAP_END(Test)			


#endif
