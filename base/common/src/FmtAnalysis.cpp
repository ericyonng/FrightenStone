#include <BaseCode/FmtAnalysis.h>

gtool::CFmtAnalysis::CFmtAnalysis()
:m_ErrFmt("")
{
	m_pAnalysisMap = FmtStringAnalysisMap::CreateNew();
	m_pShareLocker = CDataLocker::CreateNew();
}

gtool::CFmtAnalysis::~CFmtAnalysis()
{
}

void gtool::CFmtAnalysis::Cleans()
{
	ASSERT(m_pShareLocker->Lock());

	IF_OK_NL(m_pAnalysisMap)
		m_pAnalysisMap->Cleans();

	ASSERT(m_pShareLocker->UnLock());
}

//只能在单线程内完成 多线程会出现意外
bool gtool::CFmtAnalysis::Init()
{
	CHECKF_NL(m_pShareLocker);
	ASSERT(m_pShareLocker->Lock());
	IF_NOT_NL(m_pAnalysisMap)
	{
		ASSERT(m_pShareLocker->UnLock());
		return false;
	}

	
	m_bInit = true;
	ASSERT(m_pShareLocker->UnLock());

	return true;
}

bool gtool::CFmtAnalysis::Fini()
{
	DOTRY_B_NL
	{
		ASSERT(m_pShareLocker->Lock());
		auto bInit = m_bInit;
		if (!bInit)
		{
			ASSERT(m_pShareLocker->UnLock());
			return true;
		}
		m_bInit = false;
		ASSERT(m_pShareLocker->UnLock());

		Cleans();
	}
	DOCATCH_ANY_NL("gtool::CFmtAnalysis::Fini()")
	{
		return false;
	}
	DOCATCH_ANY_NL_END

	return true;
}

bool gtool::CFmtAnalysis::IsInit()
{
	ASSERT(m_pShareLocker->Lock());
	auto bInit = m_bInit;
	ASSERT(m_pShareLocker->UnLock());

	return bInit;
}

bool gtool::CFmtAnalysis::GetFmtConstObj(std::string& strFmt, boost::format *& pOutObj)
{
	pOutObj = NULL;
	CHECKF_NL(strFmt.length() > 0);

	ASSERT(m_pShareLocker->Lock());
	IF_NOT_NL(m_pAnalysisMap)
	{
		ASSERT(m_pShareLocker->UnLock());
		return false;
	}

	IF_NOT_NL(m_bInit)
	{
		ASSERT(m_pShareLocker->UnLock());
		return false;
	}

	boost::format* pFmtObjTmp= m_pAnalysisMap->GetObj(strFmt);
	boost::format *pFmt2 = NULL;
	if (!pFmtObjTmp)
	{
		CSmartPtrC<boost::format> pFmt = new boost::format;
		IF_NOT_NL(pFmt)
		{
			ASSERT(m_pShareLocker->UnLock());
			return false;
		}

		//解析格式化控制字符串
		pFmt->parse(strFmt.c_str());
		IF_NOT_NL(pFmt->remaining_args() > 0)
		{
			ASSERT(m_pShareLocker->UnLock());
			return false;
		}

		m_pAnalysisMap->insert(std::make_pair(strFmt, pFmt2 = pFmt.pop()));

		pFmtObjTmp = m_pAnalysisMap->GetObj(strFmt);
	}

	IF_NOT_NL(pFmtObjTmp)
	{
		SAFE_DELETE_NL(pFmt2);
		ASSERT(m_pShareLocker->UnLock());
		return false;
	}


	//解除绑定
	pFmtObjTmp->clear_binds();

	pOutObj =  new  boost::format(*pFmtObjTmp);

	ASSERT(m_pShareLocker->UnLock());

	return true;
}
