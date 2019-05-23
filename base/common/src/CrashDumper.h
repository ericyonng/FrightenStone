#ifndef CRASH_DUMPER_H_
#define CRASH_DUMPER_H_

#pragma once

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
typedef struct _EXCEPTION_RECORD EXCEPTION_RECORD, *PEXCEPTION_RECORD;
typedef struct _CONTEXT CONTEXT, *PCONTEXT;


NAMESPACE_BEGIN(gtool)

bool SehLock();
bool SehUnlock();
//bool RegisterExceptionFilter();
//pExceptPtrs 配合GetExceptionInformation()宏 GetExceptionInformation函数在异常处理中调用，需要先把其中的异常处理信息保存起来避免被销毁
I32 RecordExceptionInfo(EXCEPTION_RECORD ExceptionRec, CONTEXT ContextRec, const char *Message, CFmtIOString &rBufferOut, bool bSimpleInfo);

NAMESPACE_END(gtool)


#undef RECORD_ECEPTION_SIMPLEINFO
#undef RECORD_ECEPTION_FULLINFO
#undef GET_EXCEPTION_INFO
#undef SEH_TRY
#undef SEH_SIMPLEINFO_CATCH
#undef SEH_FULLINFO_CATCH
#undef SEH_CATCH_END
#undef GET_SEHCATCH_BUFFER_OBJ
#undef GET_SEHCATCH_BUFFER_STRING

#define GET_EXCEPTION_INFO() ((PEXCEPTION_POINTERS)(GetExceptionInformation()))
#define SEH_TRY  { EXCEPTION_RECORD SavedExceptionRec; CONTEXT SavedContextRec; gtool::CFmtIOString rBufferSeh; __try {
#define SEH_SIMPLEINFO_CATCH(Message) } __except(ASSERT(gtool::SehLock()), SavedExceptionRec=*(GET_EXCEPTION_INFO()->ExceptionRecord), SavedContextRec=*(GET_EXCEPTION_INFO()->ContextRecord),ASSERT(gtool::SehUnlock()), gtool::RecordExceptionInfo(SavedExceptionRec ,SavedContextRec, Message, rBufferSeh, true)) {
#define SEH_FULLINFO_CATCH(Message) } __except(ASSERT(gtool::SehLock()),SavedExceptionRec=*(GET_EXCEPTION_INFO()->ExceptionRecord), SavedContextRec=*(GET_EXCEPTION_INFO()->ContextRecord),ASSERT(gtool::SehUnlock()), gtool::RecordExceptionInfo(SavedExceptionRec ,SavedContextRec, Message, rBufferSeh, false)) {
#define SEH_CATCH_END }}
#define GET_SEHCATCH_BUFFER_OBJ() (rBufferSeh)
#define GET_SEHCATCH_BUFFER_STRING() (rBufferSeh.c_str())


#endif

