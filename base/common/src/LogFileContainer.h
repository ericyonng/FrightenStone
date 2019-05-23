#ifndef LOG_FILE_CONTAINER_H_
#define LOG_FILE_CONTAINER_H_
#pragma once

NAMESPACE_BEGIN(gtool)

//单例对象 线程安全
class CLogFileContainer
{
	NO_COPY_NO_MOVE(CLogFileContainer);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CLogFileContainer);

public:
	bool Init();	//只能在单线程内完成，多线程Init会出现意外
	bool Fini() throw ();
	bool IsInit();

	//获取解析字符串对象
public:
	//输出的fmt是堆区创建的对象
	const gtool::CFmtIOString& GetLogFileStingObj(const std::string& strFileName);

private:
	typedef CLitPtrMap<CFmtIOString, std::string, DEL_WAY_DELETE> FileContainerMap;		//解析过的格式控制字符串
	bool m_bInit = false;
	
	CSmartPtr<CDataLocker> m_pShareLocker;
	CSmartPtr<FileContainerMap> m_pFileContainerMap;
	CFmtIOString m_ErrIOString="";
};

NAMESPACE_END(gtool)

//需要在Fini之前使用，避免crash
typedef gtool::CSingleton<gtool::CLogFileContainer> LogFileContainerInstance;
#define LogFileContainerObj (*LogFileContainerInstance::GetInstance())

#endif

