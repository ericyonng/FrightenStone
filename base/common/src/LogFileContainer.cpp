#include <BaseCode/LogFileContainer.h>

gtool::CLogFileContainer::CLogFileContainer()
{
	m_pFileContainerMap = FileContainerMap::CreateNew();
	m_pShareLocker = CDataLocker::CreateNew();
}

gtool::CLogFileContainer::~CLogFileContainer()
{
}

void gtool::CLogFileContainer::Cleans()
{
	ASSERT(m_pShareLocker->Lock());

	IF_OK_NL(m_pFileContainerMap)
		m_pFileContainerMap->Cleans();

	ASSERT(m_pShareLocker->UnLock());
}

//只能在单线程内完成 多线程会出现意外
bool gtool::CLogFileContainer::Init()
{
	CHECKF_NL(m_pShareLocker);
	ASSERT(m_pShareLocker->Lock());
	IF_NOT_NL(m_pFileContainerMap)
	{
		ASSERT(m_pShareLocker->UnLock());
		return false;
	}

	
	m_bInit = true;
	ASSERT(m_pShareLocker->UnLock());

	return true;
}

bool gtool::CLogFileContainer::Fini()
{
	DOTRY_B_NL
	{
		ASSERT(m_pShareLocker->Lock());
		if (!m_bInit)
		{
			ASSERT(m_pShareLocker->UnLock());
			return true;
		}
		m_bInit = false;
		ASSERT(m_pShareLocker->UnLock());

		Cleans();
	}
	DOCATCH_ANY_NL("gtool::CLogFileContainer::Fini()")
	{
		return false;
	}
	DOCATCH_ANY_NL_END

	return true;
}

bool gtool::CLogFileContainer::IsInit()
{
	ASSERT(m_pShareLocker->Lock());
	auto bInit = m_bInit;
	ASSERT(m_pShareLocker->UnLock());

	return bInit;
}

const gtool::CFmtIOString&  gtool::CLogFileContainer::GetLogFileStingObj(const std::string& strFileName)
{
	IF_NOT_NL(strFileName.length() > 0)
		return m_ErrIOString;

	ASSERT(m_pShareLocker->Lock());
	IF_NOT_NL(m_pFileContainerMap)
	{
		ASSERT(m_pShareLocker->UnLock());
		return m_ErrIOString;
	}

	IF_NOT_NL(m_bInit)
	{
		ASSERT(m_pShareLocker->UnLock());
		return m_ErrIOString;
	}

	CFmtIOString* pIOString= m_pFileContainerMap->GetObj(strFileName);
	CFmtIOString* pIOString2 = NULL;
	if (!pIOString)
	{
		CSmartPtr<CFmtIOString> pIOStr = CFmtIOString::CreateNew();
		IF_NOT_NL(pIOStr)
		{
			ASSERT(m_pShareLocker->UnLock());
			return m_ErrIOString;
		}

		*pIOStr << (char *)strFileName.c_str();

		m_pFileContainerMap->insert(std::make_pair(strFileName, pIOString2 = pIOStr.pop()));

		pIOString = m_pFileContainerMap->GetObj(strFileName);
	}

	IF_NOT_NL(pIOString)
	{
		SAFE_RELEASE_NL(pIOString2);
		ASSERT(m_pShareLocker->UnLock());
		return m_ErrIOString;
	}

	ASSERT(m_pShareLocker->UnLock());

	return *pIOString;
}
