#ifndef LOG_FILE_DEF_H_
#define LOG_FILE_DEF_H_

#pragma once
#include <BaseCode/LogEx.inl>

DECLEAR_GENERAL_LOG_BEGIN()
GENERAL_ADDLOG(SYSLOG, crash)			//±ÀÀ£
GENERAL_ADDLOG(SYSLOG, err)				//´íÎó
GENERAL_ADDLOG(SYSLOG, memleak)			//ÄÚ´æÐ¹Â¶
GENERAL_ADDLOG(SYSLOG, info)			//ÐÅÏ¢
GENERAL_ADDLOG(SYSLOG, net)				//ÍøÂç
GENERAL_ADDLOG(SYSLOG, sys)				//ÏµÍ³
GENERAL_ADDLOG(LOGICLOG, gen)			//Âß¼­
GENERAL_ADDLOG(TESTLOG, testcode)		//´úÂë²âÊÔ
DECLEAR_GENERAL_LOG_END()



#if !(defined(linux)|defined(__CYGWIN__))
/************************************************* [ ÈÕÖ¾ºê ] *************************************************/
// #undef	LOGCRASH
// #define LOGCRASH(szFmt, ...)  (gtool::WriteFmtLog(".\\SYSLOG\\crash.log", "%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))		
// 
// #undef	LOGERR
// #define LOGERR(szFmt, ...)	(gtool::WriteFmtLog(".\\SYSLOG\\err.log","%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))
// 
// #undef	LOGMEMLEAK
// #define LOGMEMLEAK(szFmt, ...)		(gtool::WriteFmtLog(".\\SYSLOG\\memleak.log","%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))
// 
// #undef	LOGINFO
// #define LOGINFO(szFmt,...)		(gtool::WriteFmtLog(".\\SYSLOG\\info.log","%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))
// 
// #undef	LOGNET
// #define LOGNET(szFmt,...)		(gtool::WriteFmtLog(".\\SYSLOG\\net.log","%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))
// 
// #undef	LOGSYS
// #define LOGSYS(szFmt,...)		(gtool::WriteFmtLog(".\\SYSLOG\\sys.log","%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))
// 
// #undef LOGGEN
// #define LOGGEN(szFmt,...)		(gtool::WriteFmtLog(".\\LOGIC\\gen.log","%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))
// 
// #undef LOGTESTCODE
// #define LOGTESTCODE(szFmt,...)		(gtool::WriteFmtLog(".\\TESTLOG\\testcode.log","%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))

#undef SYSLOG_CRASH_LOG_NAME
#define SYSLOG_CRASH_LOG_NAME ".\\SYSLOG\\crash.log"

#undef SYSLOG_ERR_LOG_NAME
#define SYSLOG_ERR_LOG_NAME ".\\SYSLOG\\err.log"

#undef SYSLOG_MEMLEAK_LOG_NAME
#define SYSLOG_MEMLEAK_LOG_NAME ".\\SYSLOG\\memleak.log"

#undef SYSLOG_INFO_LOG_NAME
#define SYSLOG_INFO_LOG_NAME ".\\SYSLOG\\info.log"

#undef SYSLOG_NET_LOG_NAME
#define SYSLOG_NET_LOG_NAME ".\\SYSLOG\\net.log"

#undef SYSLOG_SYS_LOG_NAME
#define SYSLOG_SYS_LOG_NAME ".\\SYSLOG\\sys.log"

#undef LOGIC_GEN_LOG_NAME
#define LOGIC_GEN_LOG_NAME ".\\LOGICLOG\\gen.log"

#undef TEST_CODE_LOG_NAME
#define TEST_CODE_LOG_NAME ".\\TESTLOG\\testcode.log"

#undef	LOGCRASH
#define LOGCRASH(...)  (gtool::WriteLog(_SYSLOG_crash_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))		

#undef	LOGERR
#define LOGERR(...)	(gtool::WriteLog(_SYSLOG_err_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))

#undef	LOGMEMLEAK
#define LOGMEMLEAK(...)		(gtool::WriteLog(_SYSLOG_memleak_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))

#undef	LOGINFO
#define LOGINFO(...)		(gtool::WriteLog(_SYSLOG_info_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))

#undef	LOGNET
#define LOGNET(...)		(gtool::WriteLog(_SYSLOG_net_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))

#undef	LOGSYS
#define LOGSYS(...)		(gtool::WriteLog(_SYSLOG_sys_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))

#undef LOGGEN
#define LOGGEN(...)		(gtool::WriteLog(_LOGICLOG_gen_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))

#undef LOGTESTCODE
#define LOGTESTCODE(...)		(gtool::WriteLog(_TESTLOG_testcode_, __FILE__," ", __LINE__," ", __FUNCSIG__," ", ##__VA_ARGS__))

#undef LOGTESTFMT
#define LOGTESTFMT(szFmt, ...)		(gtool::WriteFmtLog(_TESTLOG_testcode_, "%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))	//Ð§ÂÊµÍ£¬µ«»º³å²»ÏÞÖÆ

#undef LOGSYS_FMT_CSTYLE
#define LOGSYS_FMT_CSTYLE(BUFFER_TYPE, szFmt, ...) (gtool::WriteLogCStyle<BUFFER_TYPE>(_SYSLOG_sys_, "%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))

//ÍøÂç
#undef LOGNET_FMT_CSTYLE
#define LOGNET_FMT_CSTYLE(BUFFER_TYPE, szFmt, ...) (gtool::WriteLogCStyle<BUFFER_TYPE>(_SYSLOG_net_, "%s %d %s ", szFmt, __FILE__, __LINE__, __FUNCSIG__, ##__VA_ARGS__))

//512 ÍøÂç
#undef LOGNET_FMT_C512
#define LOGNET_FMT_C512(szFmt, ...)	LOGNET_FMT_CSTYLE(BUFFER_LEN_TYPE_512, szFmt, ##__VA_ARGS__)

//1K ÍøÂç
#undef LOGNET_FMT_C1024
#define LOGNET_FMT_C1024(szFmt, ...)	LOGNET_FMT_CSTYLE(BUFFER_LEN_TYPE_1024, szFmt, ##__VA_ARGS__)

//4K ÍøÂç
#undef LOGNET_FMT_C4096
#define LOGNET_FMT_C4096(szFmt, ...)	LOGNET_FMT_CSTYLE(BUFFER_LEN_TYPE_4096, szFmt, ##__VA_ARGS__)

//sys
#undef LOGSYS_FMT_C512
#define LOGSYS_FMT_C512(szFmt, ...)		LOGSYS_FMT_CSTYLE(BUFFER_LEN_TYPE_512, szFmt, ##__VA_ARGS__)

#undef LOGSYS_FMT_C1024
#define LOGSYS_FMT_C1024(szFmt, ...)	LOGSYS_FMT_CSTYLE(BUFFER_LEN_TYPE_1024, szFmt, ##__VA_ARGS__)

#undef LOGSYS_FMT_C4096
#define LOGSYS_FMT_C4096(szFmt, ...)	LOGSYS_FMT_CSTYLE(BUFFER_LEN_TYPE_4096, szFmt, ##__VA_ARGS__)

#undef LOGSYS_FMT_C10240
#define LOGSYS_FMT_C10240(szFmt, ...)	LOGSYS_FMT_CSTYLE(BUFFER_LEN_TYPE_10240, szFmt, ##__VA_ARGS__)

#undef LOGSYS_FMT_C64K
#define LOGSYS_FMT_C64K(szFmt, ...)	LOGSYS_FMT_CSTYLE(BUFFER_LEN_TYPE_64K, szFmt, ##__VA_ARGS__)

#undef LOGSYS_FMT_C128K
#define LOGSYS_FMT_C128K(szFmt, ...)	LOGSYS_FMT_CSTYLE(BUFFER_LEN_TYPE_128K, szFmt, ##__VA_ARGS__)

#undef LOGSYS_FMT_C256K
#define LOGSYS_FMT_C256K(szFmt, ...)	LOGSYS_FMT_CSTYLE(BUFFER_LEN_TYPE_256K, szFmt, ##__VA_ARGS__)

//»º³å 512
#undef LOGSYSFMT_C512
#define LOGSYSFMT_C512(szFmt, ...)	LOGSYS_FMT_C512(szFmt, ##__VA_ARGS__)

//»º³å 1K
#undef LOGSYSFMT_C1024
#define LOGSYSFMT_C1024(szFmt, ...)	LOGSYS_FMT_C1024(szFmt, ##__VA_ARGS__)

//»º³å 4096
#undef LOGSYSFMT_4K
#define LOGSYSFMT_4K(szFmt, ...)	LOGSYS_FMT_C4096(szFmt, ##__VA_ARGS__)

//´ó»º³ålog 10KB
#undef LOGSYSFMT_10K
#define LOGSYSFMT_10K(szFmt, ...)	LOGSYS_FMT_C10240(szFmt, ##__VA_ARGS__)

//´ó»º³ålog 64KB
#undef LOGSYSFMT_64K
#define LOGSYSFMT_64K(szFmt, ...)	LOGSYS_FMT_C64K(szFmt, ##__VA_ARGS__)

//´ó»º³ålog 64KB
#undef LOGSYSFMT_128K
#define LOGSYSFMT_128K(szFmt, ...)	LOGSYS_FMT_C128K(szFmt, ##__VA_ARGS__)

//´ó»º³ålog 64KB
#undef LOGSYSFMT_256K
#define LOGSYSFMT_256K(szFmt, ...)	LOGSYS_FMT_C256K(szFmt, ##__VA_ARGS__)

//NET 4K
#undef LOGNET_FMT4K
#define LOGNET_FMT4K(szFmt, ...)	LOGNET_FMT_C4096(szFmt, ##__VA_ARGS__)

//NET 1K
#undef LOGNET_FMT1K
#define LOGNET_FMT1K(szFmt, ...)	LOGNET_FMT_C1024(szFmt, ##__VA_ARGS__)

//NET 512
#undef LOGNET_FMT512
#define LOGNET_FMT512(szFmt, ...)	LOGNET_FMT_C512(szFmt, ##__VA_ARGS__)


/************************************************* [ E N D ] *************************************************/

#else

//linux

#endif

//logµÄ³õÊ¼»¯£º
//LogInitHelper<LOG_NUM_MAX>::InitLog();

#endif

