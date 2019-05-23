#ifndef MACRO_DEFINE_H_
#define MACRO_DEFINE_H_

#pragma once

// 默认IP地址
#ifndef DEFAULT_IP
#define DEFAULT_IP            "127.0.0.1"
#endif

//内存池垃圾回收周期
#define MEM_POOL_GC_PERIOD		1000		//1s唤醒一次

//内存池
#define		MEMBLOCK_MINIUM_SIZE		msize_t(8)						//块单位8BYTE，块大小需为块单位大小的整数倍
#define		ALIGN_SIZE					msize_t(8)						//8字节对齐
#define		MEMPAGE_MINIUM_SIZE			msize_t(64*1024*1024)			//64MB最小内存页，内存页大小需要为256MB的整数倍
#define		MEMPAGE_INIT_NUM			msize_t(4)						//初始化内存页数
#define		THREAD_WORK_PERIOD			msize_t(5*1000)					//维护线程执行周期
#define		PYSICAL_MEM_REMAIN_BYTE		msize_t(MEMPAGE_MINIUM_SIZE)	//物理内存保留余量，否则系统崩溃
#define		MEM_POOL_SUC_MERGE_SIZE		msize_t(64*1024)				//成功融合最小块限制
#define		MEM_POOL_MERGE_TIME_OUT		msize_t(10)						//10ms 融合区超时时间片
#define		MEM_POOL_CPU_IDLE_VAL		MYDOUBLE(60)					//cpu空闲的使用率阈值

//对象名
#undef THIS_OBJ_NAME_MACRO
#define THIS_OBJ_NAME_MACRO	(typeid(*this).name())

#undef OBJ_NAME_MACRO
#define OBJ_NAME_MACRO(obj)	(#obj)

#undef UNUSED_ARG
#define UNUSED_ARG(x)	(x)

#undef ZERO_CLEAN_OBJ
#define ZERO_CLEAN_OBJ(x)	memset(&(x), 0, sizeof(x));

#undef ZERO_CLEAN_PTR
#define ZERO_CLEAN_PTR(x, sz) memset((void *)(x), 0, (sz));

#undef COPY_OBJ
#define COPY_OBJ(dest, src) memmove(&(dest),&(src), gtool::Min(sizeof(dest), sizeof(src)));

#undef CONST_TO_NONE
#define  CONST_TO_NONE(TYPE, VAR) ( ASSERT(VAR), *(const_cast<TYPE *>(&(VAR))) )	//var必须非空

#undef STRARRAY_ADD_ZERO_TAIL
#define STRARRAY_ADD_ZERO_TAIL(strobj, pos) \
{ \
	auto zero_pos_##__LINE__ = gtool::Max(pos, I32(0));\
	strobj[gtool::Min(zero_pos_##__LINE__, I32(sizeof(strobj) - 1))] = 0;\
}

#undef STRPTR_ADD_ZERO_TAIL
#define STRPTR_ADD_ZERO_TAIL(strptr, strsize, pos)\
{ \
auto zero_pos_##__LINE__ = gtool::Max(pos, I32(0)); \
	strptr[gtool::Min(zero_pos_##__LINE__, I32(strsize - 1))] = 0;\
}

#undef LOOP_MAX_DEF
#define LOOP_MAX_DEF 1000

//浮点数比较
#undef FLOAT_COMPARE_A_EQUAL_B
#define FLOAT_COMPARE_A_EQUAL_B(a, b, epsilon)\
( ((double(a)-double(b))<(epsilon)) && (-(epsilon)<(double(a)-double(b))) )

#undef FLOAT_COMPARE_A_BIGGER_B
#define FLOAT_COMPARE_A_BIGGER_B(a, b, epsilon)\
((double(a)-double(b))>(epsilon))

#undef FLOAT_COMPARE_A_LOWWER_B
#define FLOAT_COMPARE_A_LOWWER_B(a, b, epsilon)\
((double(a)-double(b))<(-(epsilon)))

#undef FLOAT_COMPARE_A_BIGGER_EQUAL_B
#define  FLOAT_COMPARE_A_BIGGER_EQUAL_B(a, b, epsilon)\
(FLOAT_COMPARE_A_BIGGER_B(a, b, epsilon)||FLOAT_COMPARE_A_EQUAL_B(a, b, epsilon))

#undef FLOAT_COMPARE_A_LOWWER_EQUAL_B
#define  FLOAT_COMPARE_A_LOWWER_EQUAL_B(a, b, epsilon)\
(FLOAT_COMPARE_A_LOWWER_B(a, b, epsilon)||FLOAT_COMPARE_A_EQUAL_B(a, b, epsilon))

#endif

