#ifndef NET_MACRO_H_
#define NET_MACRO_H_

#pragma once

// 缓冲区长度 (1024*8)
// 之所以为什么设置8K，也是一个江湖上的经验值
// 如果确实客户端发来的每组数据都比较少，那么就设置得小一些，省内存
#ifndef MAX_BUFFER_LEN
#define MAX_BUFFER_LEN        8192
#endif


// 默认端口
#ifndef DEFAULT_PORT
#define DEFAULT_PORT          12345  
#endif

#ifndef INVALID_SOCKET_IDD
#define INVALID_SOCKET_IDD	U64(~0)		//无效套接字
#endif

#ifndef SEND_FALE_COUNT_MAX
#define SEND_FALE_COUNT_MAX 5			//发送5次失败则不发送
#endif

#ifndef IS_VALID_SOCKET_IDD
#define IS_VALID_SOCKET_IDD(x)  (((x)!=INVALID_SOCKET_IDD)&&((x)!=0))		//是否有效的socketIDD
#endif

#ifndef SET_INVALID_SOCKET
#define  SET_INVALID_SOCKET(x)	(x)=INVALID_SOCKET_IDD
#endif

#ifndef EXIT_CODE
#define EXIT_CODE						NULL	// 传递给Worker线程的退出信号
#endif

#define WORKER_THREADS_PER_PROCESSOR	2		// 每一个处理器上产生多少个线程(为了最大限度的提升服务器性能，详见配套文档)
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}	// 释放句柄宏
#define RELEASE_SOCKET(x)               {if(IS_VALID_SOCKET_IDD(x)) {  ASSERTNET(closesocket(x)==0);x=INVALID_SOCKET;}}	// 释放Socket宏

#endif

