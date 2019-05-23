#ifndef HEAP_MGR_H_
#define HEAP_MGR_H_

#pragma once
#include <BaseCode/HeapExApi.h>

//#include <BaseCode/MyBaseMacro.h>
//#include <BaseCode/myclassmacro.h>
//#include <map>
//#include <BaseCode/MyDataType.h>
//#include <BaseCode/WinLocker.h>
class CCriticalSectionProtect;



//1字节对齐
#pragma MEM_ALIGN_BEGIN(1)

//重载类的 operator new delete
//使用一个类内static 管理本类的内存状况 validptr：可以用一个map来判断该部分内存是否被申请过

NAMESPACE_BEGIN(gtool)

struct HEAP_EX_API ObjMemInfo
{
	U32				u32ObjBaseAddress;				//对象首地址
	size_t			szObjSize;						//对象所占内存空间
	unsigned long	idThread;						//创建者线程id
	std::string		strLastUserModuleName;			//生命期最后一次发生异常的模块名称
	std::string		strLastUserDetailName;			//类名 | 函数名
	U32				u32LastUserModuleAddress;		//生命期最后一次发生异常的调用者地址
	I32				i32LastUserProcessId;			//生命周期最后一次发生异常的进程id
	ObjMemInfo()
	:u32ObjBaseAddress(0), szObjSize(0), idThread(0), u32LastUserModuleAddress(0), i32LastUserProcessId(0)
	{}
};

//要确保线程安全 在其他类中需要以静态类存在，以便在程序退出或者模块释放时候打印内存泄露信息
class HEAP_EX_API CHeapMgr
{
public:
	CHeapMgr(const char *szClassName = "");
	virtual ~CHeapMgr();

public:
	void*		ApplyMem(size_t szApply);
	void		FreeMem(void* ptr);
	bool		IsValidPt(void* p);

public:
	inline bool GetModuleCallerInfo(void *&pCallerAddress, void *&pModuleHandle, std::string& strName);
	inline void SetLastUserInfo(void *ObjAddress, const I32 i32ProcessId, const U32 u32CallerAddress, const char *szCallerModuleName, const char *szClassName, const char * szFuncName);

public:
	void		Cleans();

protected:
	std::map<U32, ObjMemInfo>	m_PtrMemInfoMap;	//key：对象首地址， value：ObjMemInfo		//用于Heap释放时候打内存泄露信息
	
	std::string					m_ClassName;		//本类名称
	void *						m_hCurModuleHandle = NULL;	//当前所处模块句柄（类型=HANDLE）
	std::string					m_strModuleName;		//模块名
	I32							m_ProcessId=0;			//进程id
	void *						m_pCallerAddress=NULL;	//调用者地址
	CCriticalSectionProtect		m_SafeGuardLocker;	//线程安全
	volatile	bool			m_bHeepInit=false;
};



NAMESPACE_END(gtool)

#pragma MEM_ALIGN_END(1)

#endif
