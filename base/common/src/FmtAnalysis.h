#ifndef FMT_ANALYSIS_H_
#define FMT_ANALYSIS_H_
#pragma once

class CDataLocker;

NAMESPACE_BEGIN(gtool)

//单例对象 线程安全
class CFmtAnalysis
{
	NO_COPY_NO_MOVE(CFmtAnalysis);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CFmtAnalysis);

public:
	bool Init();	//只能在单线程内完成，多线程Init会出现意外
	bool Fini() throw ();
	bool IsInit();

	//获取解析字符串对象
public:
	//输出的fmt是堆区创建的对象
	bool GetFmtConstObj(std::string& strFmt, boost::format *& pOutObj);

private:
	typedef CLitPtrMap<boost::format, std::string, DEL_WAY_DELETE> FmtStringAnalysisMap;		//解析过的格式控制字符串
	bool m_bInit = false;
	
	CSmartPtr<CDataLocker> m_pShareLocker;
	CSmartPtr<FmtStringAnalysisMap> m_pAnalysisMap;
	const boost::format m_ErrFmt;
};

NAMESPACE_END(gtool)

//需要在Fini之前使用，避免crash
typedef gtool::CSingleton<gtool::CFmtAnalysis> FmtAnalysisInstance;
#define FmtAnalysisObj (*FmtAnalysisInstance::GetInstance())

#endif

