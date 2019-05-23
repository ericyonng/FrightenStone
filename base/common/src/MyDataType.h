#ifndef MY_DATA_TYPE_H_
#define MY_DATA_TYPE_H_
#pragma once

#pragma MEM_ALIGN_BEGIN(1)

//函数返回结果
enum FUN_RET_STATUS
{
	FUN_RET_STATUS_UN_DO = -1,				//无状态
	FUN_RET_STATUS_FAILURE = 0,				//失败
	FUN_RET_STATUS_SUC=1,					//成功
};

#define RES_UN_DO		0											//无动作
#define RES_SUCCESS		1											//成功
#define RES_FAILED		2											//失败

//线程处理函数参数
#define		TIME_SLEEP_PER_TASK			10		//ms 线程空闲睡眠时间
#define		NO_TASK_BREAK_COUNT			10		//轮休无任务跳出次数
#define		THREAD_MIN_NUM				0		//最小线程数
#define		THREAD_MAX_NUM				200		//最大线程数

//#ifndef int
//typedef int int;
//#endif


typedef	unsigned long	CLS_ID;
#ifndef NULL
#define NULL NULL
#endif

#ifndef BUFFER64_ARRAY_TYPE
#define BUFFER64_ARRAY_TYPE

#undef		BUFFER_LEN8
#define		BUFFER_LEN8 8
#undef		BUFFER_LEN16
#define		BUFFER_LEN16 16
#undef		BUFFER_LEN32
#define		BUFFER_LEN32 32
#undef		BUFFER_LEN64
#define		BUFFER_LEN64 64
#undef		BUFFER_LEN128
#define		BUFFER_LEN128 128
#undef		BUFFER_LEN256
#define		BUFFER_LEN256 256
#undef		BUFFER_LEN512
#define		BUFFER_LEN512 512
#undef		BUFFER_LEN1024
#define		BUFFER_LEN1024 1024
#undef		BUFFER_LEN2048
#define		BUFFER_LEN2048 2048
#undef		BUFFER_LEN4096
#define		BUFFER_LEN4096 4096
#undef		BUFFER_LEN8192
#define		BUFFER_LEN8192 8192
#undef		BUFFER_LEN9126
#define		BUFFER_LEN9126 9126
#undef		BUFFER_LEN10240
#define		BUFFER_LEN10240 10240
#undef		BUFFER_LEN64K			
#define		BUFFER_LEN64K (65536)	//64K
#undef		BUFFER_LEN128K
#define		BUFFER_LEN128K (131072)
#undef		BUFFER_LEN256K
#define		BUFFER_LEN256K (262144)
#undef		BUFFER_LEN512K
#define		BUFFER_LEN512K (524288)
#undef		BUFFER_LEN1M
#define		BUFFER_LEN1M	(1048576)


typedef char BUFFER8[BUFFER_LEN8];
typedef char BUFFER16[BUFFER_LEN16];
typedef char BUFFER32[BUFFER_LEN32];
typedef char BUFFER64[BUFFER_LEN64];
typedef char BUFFER128[BUFFER_LEN128];
typedef char BUFFER256[BUFFER_LEN256];
typedef char BUFFER512[BUFFER_LEN512];
typedef char BUFFER1024[BUFFER_LEN1024];
typedef char BUFFER2048[BUFFER_LEN2048];
typedef char BUFFER4096[BUFFER_LEN4096];
typedef char BUFFER8192[BUFFER_LEN8192];
typedef char BUFFER9126[BUFFER_LEN9126];
typedef char BUFFER10240[BUFFER_LEN10240];
typedef char BUFFER64K[BUFFER_LEN64K];
typedef char BUFFER128K[BUFFER_LEN128K];
typedef char BUFFER256K[BUFFER_LEN256K];
typedef char BUFFER512K[BUFFER_LEN512K];
typedef char BUFFER1M[BUFFER_LEN1M];

#endif //BUFFER64_ARRAY_TYPE

typedef unsigned __int64	U64;
typedef __int64				I64;
typedef unsigned __int32	U32;
typedef __int32				I32;
typedef unsigned short		U16;
typedef short				I16;
typedef unsigned char		U8;
typedef char				I8;
typedef unsigned char		uint8_t;
typedef long				MYLONG;
typedef long long			MYLONGLONG;
typedef unsigned long		MYULONG;
typedef unsigned long long	MYULONGLONG;
typedef float				MYFLOAT;
typedef double				MYDOUBLE;


#undef SIZEU64
#define SIZEU64 (sizeof(U64))

typedef char DISPLAY_BUF[1024];
//typedef void* LPVOID;

typedef		I64							msize_t;						//内存大小类型
typedef		std::atomic<msize_t>		atmc_sizet;						//原子操作的内存大小类型
typedef		std::atomic<void *>		    atmc_memaddr;					//原子操作的内存地址类型


typedef U64 SOCKET_IDD;				//套接字对应的唯一id，暂时使用socket上下文的内存地址

#undef MAX_I64_NUM
#define MAX_I64_NUM	0x7FFFFFFFFFFFFFFF

typedef U64 NET_DATA_SIZE;

#pragma MEM_ALIGN_END(1)

#endif
