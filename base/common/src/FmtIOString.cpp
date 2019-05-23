#include<BaseCode/FmtIOString.h>
#include <iostream>

//#pragma MEM_ALIGN_BEGIN(1)

// gtool::CFmtIOString::IOEndl::IOEndl(IOEndl&& rValue)
// {
// 
// }

#if (defined(_WIN32)|defined(_WIN64))
const char *gtool::CFmtIOString::endl = "\r\n";
#else
const char *gtool::CFmtIOString::endl = "\n";
#endif

gtool::CFmtIOString::CFmtIOString()
{

}

gtool::CFmtIOString::CFmtIOString(const char * str)
{
	m_strBuffer = str;
}

gtool::CFmtIOString::CFmtIOString(char * str)
{
	m_strBuffer = str;
}

gtool::CFmtIOString::CFmtIOString(CFmtIOString&& rValue)
{
	Clear();
	//m_strFmt.swap(rValue.m_strFmt);
	m_strBuffer = rValue.m_strBuffer.c_str();
}

void gtool::CFmtIOString::Clear()
{
	//m_strFmt.clear_binds();
	if (m_szBuffer[0] != 0)
	{
		m_szBuffer[0] = 0;
	}
	
	if (m_strBuffer.length() > 0)
	{
		m_strBuffer.clear();
	}
}

const char * gtool::CFmtIOString::c_str() const
{
	return m_strBuffer.c_str();
}

U64 gtool::CFmtIOString::GetLength()
{
	return m_strBuffer.length();
}

gtool::CFmtIOString  & gtool::CFmtIOString::operator = (const char * strVal)
{
	DOTRY_B_NL
	{
		if (strVal == NULL) return *this;

		m_strBuffer = strVal;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator = (const char * strVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator=(char * strVal)
{
	DOTRY_B_NL
	{
		if (strVal == NULL) return *this;

		m_strBuffer = strVal;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator = (char * strVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(CFmtIOString&& str)
{
	DOTRY_B_NL
	{
		m_strBuffer += str.c_str();
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(CFmtIOString&& str)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const bool& bVal)
{
	DOTRY_B_NL
	{
		sprintf(m_szBuffer, "%d", bVal);
		m_szBuffer[1] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL(" gtool::CFmtIOString::operator<<(const bool& bVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const char& cVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%c", cVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const char& cVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const unsigned char& hhuVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%hhu", hhuVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const unsigned char& hhuVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const short& hdVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%hd", hdVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const short& hdVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const unsigned short& huVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%hu", huVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const unsigned short& huVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const int& dVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%d", dVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const int& dVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const unsigned int& uVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%u", uVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const unsigned int& uVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const long& ldVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%ld", ldVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const long& ldVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const unsigned long& luVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%lu", luVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const unsigned long& luVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

//线程id 
gtool::CFmtIOString &gtool::CFmtIOString::operator << (const std::thread::id& idVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%u", idVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
		DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const std::thread::id& idVal)")
	{
	}
	DOCATCH_ANY_NL_END

		return *this;
}


#if (defined(_WIN32)|defined(_WIN64))

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const __int64& I64dVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%I64d", I64dVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL(" gtool::CFmtIOString::operator<<(const __int64& I64dVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const unsigned __int64& I64uVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%I64u", I64uVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const unsigned __int64& I64uVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}
#else


gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const long long& lldVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%lld", lldVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const long long& lldVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const unsigned long long& lluVal)
{
	DOTRY_B_NL
	{
		auto nLen = sprintf(m_szBuffer, "%llu", lluVal);
		nLen = ((nLen < BUFFER_LEN32) ? gtool::Max(nLen, 0) : (BUFFER_LEN32 - 1));
		m_szBuffer[nLen] = 0;
		m_strBuffer += m_szBuffer;
	}
	DOCATCH_ANY_NL(" gtool::CFmtIOString::operator<<(const unsigned long long& lluVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

#endif


gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const float& fVal)
{
	DOTRY_B_NL
	{
		gtool::BufferAdapter<float> strBuffer;
		sprintf(strBuffer.Buffer, "%f", fVal);
		m_strBuffer += strBuffer.Buffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const float& fVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const double& lfVal)
{
	DOTRY_B_NL
	{
		gtool::BufferAdapter<double> strBuffer;
		sprintf(strBuffer.Buffer, "%lf", lfVal);
		m_strBuffer += strBuffer.Buffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const double& lfVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const void * vpVal)
{
	DOTRY_B_NL
	{
		gtool::BufferAdapter<void *> strBuffer;
		sprintf(strBuffer.Buffer, "%p", vpVal);
		m_strBuffer += strBuffer.Buffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const void * vpVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const void *const& vpVal)
{
	DOTRY_B_NL
	{
		gtool::BufferAdapter<void *> strBuffer;
		sprintf(strBuffer.Buffer, "%p", vpVal);
		m_strBuffer += strBuffer.Buffer;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const void *const vpVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const char * sVal)
{
	DOTRY_B_NL
	{
		if (sVal == NULL) return *this;
		m_strBuffer += sVal;
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const char * sVal)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString & gtool::CFmtIOString::operator<<(const gtool::CFmtIOString &str)
{
	DOTRY_B_NL
	{
		m_strBuffer += str.m_strBuffer.c_str();
	}
	DOCATCH_ANY_NL("gtool::CFmtIOString::operator<<(const gtool::CFmtIOString &str)")
	{
	}
	DOCATCH_ANY_NL_END

	return *this;
}

gtool::CFmtIOString::~CFmtIOString()
{
	//Clear();
}

gtool::CFmtIOString::operator gtool::CFmtIOString && ()
{
	return std::move(*this);
}

gtool::CFmtIOString::operator bool()
{
	return m_strBuffer.length() > 0;
}

void gtool::CFmtIOString::Swap(gtool::CFmtIOString&& rRightVal)
{
	m_strBuffer.swap(rRightVal.m_strBuffer);
}

void gtool::CFmtIOString::UpdateLastModifyTimestamp(const time_t tLast /*= 0*/)
{
	if (tLast <= 0)
	{
		m_tLastModify = time(NULL);
		return;
	}
	m_tLastModify = tLast;		
}

// const char * gtool::CFmtIOString::FormatExHighCStyle(const char *szFmt, ...)
// {
// 	va_list args;
// 	std::string strFmt = szFmt;
// 	auto fmtLen = strFmt.length();
// 	//将变参转化为字符串
// 	va_start(args, szFmt); //可变参数在szFmt的下一个开始
// 
// 	//格式占位符个数
// 	const ptrdiff_t nCountFmt = std::count(strFmt.begin(), strFmt.end(), '%');
// 
// 
// 	
// 
// 
// 	//估算最大所需缓存空间	
// 	int nMemneeded = MAX_INTEGER_STR_WIDTH * nCountFmt + strFmt.length() + 1;
// 
// 	//寻找字符串
// // 	long nPosStr = 0;
// // 	long nPosPosStr = 0;
// // 	std::string strTmp;
// // 	ptrdiff_t leftCountFmt = 0;
// // 	while (nPosStr != std::string::npos)
// // 	{
// // 		nPosStr = strFmt.find_first_of("%s", nPosStr);
// // 		if (nPosStr != std::string::npos)
// // 		{
// // 			strTmp = strFmt.substr(nPosStr + 2, fmtLen - nPosStr - 2);
// // 			leftCountFmt = nCountFmt - std::count(strTmp.begin(), strTmp.end(), '%') - 1;	//%s之前格式控制符个数
// // 
// // 																							//
// // 		}
// // 	}
// 	//打印list
// // 	auto pTmp = args;
// // 	ptrdiff_t nCount = 0;
// // 	while (true)
// // 	{
// // 		nCount++;
// // 		std::cout << pTmp << std::endl;
// // 		std::cout << ++pTmp << std::endl;
// // 		if(nCount>= nCountFmt)
// // 			break;
// // 	}
// // 	
// 	auto pParam = &szFmt;
// 	long nLoop = nCountFmt;
// 	while ((nLoop--) > 0)
// 	{
// 		pParam++;
// 		auto Value = (decltype(*pParam))(*pParam);
// 		std::cout << Value << std::endl;
// 	}
// 
// 	gtool::CSmartPtrC<char> pBuffer = new char[nMemneeded];
// 	CHECKF_NL(pBuffer);
// 
// 	//auto nLen= vsprintf(NULL, strFmt.c_str(), args);
// 	//std::cout << nLen << std::endl;
// 	//auto len=strlen(args);
// 	//len;
// 	va_end(args);
// 	pBuffer[nMemneeded - 1] = 0;
// 	m_strBuffer += (char *)pBuffer;
// }

//#pragma MEM_ALIGN_END(1)