#ifndef CFMT_IO_STRING_HPP_
#define CFMT_IO_STRING_HPP_

#pragma once
//最好放在cpp中避免编译变慢

// #include <BaseCode/InlineFun.hpp>
// #include <BaseCode/SmartPtrC.hpp>
// #include <boost/format.hpp>
//#include <BaseCode/MyDataType.h>
//#include <BaseCode/myclassmacro.h>
//#include <boost/format.hpp>
//#include <BaseCode/MyBaseMacro.h>

//#pragma MEM_ALIGN_BEGIN(1)

NAMESPACE_BEGIN(gtool)

class CFmtIOString
{
		NO_COPY_NEED_RVALUE(CFmtIOString);

public:
	CFmtIOString();
	CFmtIOString(const char * str);
	CFmtIOString(char * str);
	virtual ~CFmtIOString();

public:
//	CFmtIOString &operator = (const std::string &strVal) = delete;
	CFmtIOString &operator = (const char * strVal);
	CFmtIOString &operator = (char * strVal);
	CFmtIOString &operator << (const gtool::CFmtIOString &str);			//字符串输出
	CFmtIOString &operator << (CFmtIOString &&str);			//字符串输出
	CFmtIOString &operator << (const bool& bVal);					//布尔值输出
	CFmtIOString &operator << (const char& cVal);					//字符输出
	CFmtIOString &operator << (const unsigned char& hhuVal);		//字符输出
	CFmtIOString &operator << (const short& hdVal);				//短整型输出
	CFmtIOString &operator << (const unsigned short& huVal);		//短整型输出
	CFmtIOString &operator << (const int& dVal);					//整型输出
	CFmtIOString &operator << (const unsigned int& uVal);			//整型输出
	CFmtIOString &operator << (const long& ldVal);				//长整型输出
	CFmtIOString &operator << (const unsigned long& luVal);		//长整型输出
	CFmtIOString &operator << (const std::thread::id& idVal);		//线程id 

#if (defined(_WIN32)|defined(_WIN64))
	CFmtIOString &operator << (const __int64& I64dVal);			//64位输出
	CFmtIOString &operator << (const unsigned __int64& I64uVal);	//64位输出
#else
	CFmtIOString &operator << (const long long& lldVal);			//64位输出
	CFmtIOString &operator << (const unsigned long long& lluVal);	//64位输出
#endif
	CFmtIOString &operator << (const float& fVal);	//单精度
	CFmtIOString &operator << (const double& lfVal);	//双精度
	CFmtIOString &operator << (const char * sVal);	//字符串
	CFmtIOString &operator << (const void * vpVal);	//打印地址
	CFmtIOString &operator << (const void *const& vpVal);	//打印地址
	operator CFmtIOString && ();
	operator bool();

	//辅助
public:
	const char *c_str() const;
	auto& GetBuffer() { return m_szBuffer; }
	U64 GetLength();
	void Clear();
	static CFmtIOString* CreateNew(){ return new CFmtIOString(); }
	int Release() { Clear(); delete this; return 0; } 
	std::string& GetStrObj() { return m_strBuffer; }
	void Swap(gtool::CFmtIOString&& rRightVal);
	void UpdateLastModifyTimestamp(const time_t tLast = 0);
	time_t GetLastModifyTimestamp() { return m_tLastModify; }

	//fmt输入
public:
	//绑定的参数不可超过提供的可变参数个数 普通版本
	template<typename... Args>
	const char * FormatLowEx(const char *szFmt, const Args&... rest)
	{
		//m_strFmt.clear_binds();
		//m_strFmt.parse(szFmt);
		boost::format fmtObj(szFmt);
		const auto nBindArgs = fmtObj.remaining_args();
		const auto CurArgs = sizeof...(rest);
		const auto nFmtLen = strlen(szFmt);
		if (nBindArgs > CurArgs) return NULL;

		CFmtIOString rActualMore;

		if (nBindArgs <= 0)
		{
			if (szFmt&&nFmtLen > 0)
			{
				rActualMore << (char *)szFmt;
			}
		}

		Append(fmtObj, rActualMore, rest...);

		if (nBindArgs > 0)
		{
			m_strBuffer += fmtObj.str().c_str();
		}
		
		if (CurArgs > nBindArgs || nBindArgs<=0)
		{
			m_strBuffer += rActualMore.c_str();
		}
		
		return m_strBuffer.c_str();
	}

	//绑定的参数不可超过提供的可变参数个数 低性能版本
	template<typename... Args>
	const char * FormatBoostEx(boost::format& fmtObj, const Args&... rest)
	{
		fmtObj.clear_binds();
		const auto nBindArgs = fmtObj.remaining_args();
		const auto CurArgs = sizeof...(rest);
		if (nBindArgs <= 0 || nBindArgs > CurArgs) return NULL;

		CFmtIOString rActualMore;

		Append(fmtObj, rActualMore, rest...);

		m_strBuffer += fmtObj.str().c_str();

		if (CurArgs > nBindArgs)
		{
			m_strBuffer += rActualMore.c_str();
		}

		return m_strBuffer.c_str();
	}

	//绑定的参数不可超过提供的可变参数个数 高性能版本
	template<typename... Args>
	const char * FormatPro(std::string& strFmt, const Args&... rest)
	{
		//定义
		//const auto CurArgs = sizeof...(rest);
		CFmtIOString rActualMore;
		U64 u64FirstIndex = 0, u64NextIndex = 0;

		ASSERT(com_algorithm::GetFirstValidFmtString(strFmt, u64FirstIndex, m_strBuffer));
		com_algorithm::NextFmtString(strFmt, u64FirstIndex, u64NextIndex);

		if (u64FirstIndex!=std::string::npos)
		{			
			//解析
			AppendPro(strFmt, u64FirstIndex, u64NextIndex, rActualMore,rest...);
			m_strBuffer += rActualMore.c_str();
		}
		else
		{
			//不用解析控制字符
			NoFmtInput(rest...);
		}

		return m_strBuffer.c_str();
	}
	//c语言风格可变参数 有溢出风险需要确保 参数个数与格式控制符个数一致
//	const char *FormatExHighCStyle(const char *szFmt, ...);

	//无格式控制符
	template< typename... Args>
	const char * NoFmtInput(const Args&... rest)
	{
		AppendNoFmt(rest...);
		return m_strBuffer.c_str();
	}

	//内部接口
protected:
	template<typename T>
	void Append(boost::format&FmtObj, CFmtIOString&rOs, const T &tVal)	//特例函数模板终止递归
	{
		if (FmtObj.remaining_args() > 0) {
			FmtObj%tVal;
		}
		else {
			rOs << tVal;
		}
	}

	template<typename T, typename... Args>
	void Append(boost::format&FmtObj, CFmtIOString&rOs, const T&tVal, const Args&... rest)	//Args&...对 参数包Args进行扩展，得到函数参数包：rest
	{
		auto nRemain = FmtObj.remaining_args();
		if (nRemain > 0) {
			FmtObj%tVal;
		}
		else {
			rOs << tVal;
		}

		Append(FmtObj, rOs, rest...);	//对rest函数参数包进行扩展
	}

	//u64StartFmtIndex 需要为当前格式控制符位置，std::string::npos为格式控制符不存在
	template<typename T>
	void AppendPro(std::string& strFmtLeft, U64& u64StartFmtIndex, U64& u64NextFmtIndex, CFmtIOString&rOs, const T &tVal)	//特例函数模板终止递归
	{
		DoAnalysisFmt(strFmtLeft, u64StartFmtIndex, u64NextFmtIndex, rOs, tVal);
		ASSERT(u64StartFmtIndex == std::string::npos);
	}

	template<typename T, typename... Args>
	void AppendPro(std::string& strFmtLeft, U64& u64StartFmtIndex, U64& u64NextFmtIndex, CFmtIOString&rOs, const T&tVal, const Args&... rest)	//Args&...对 参数包Args进行扩展，得到函数参数包：rest
	{
		DoAnalysisFmt(strFmtLeft, u64StartFmtIndex, u64NextFmtIndex, rOs, tVal);
		AppendPro(strFmtLeft, u64StartFmtIndex, u64NextFmtIndex, rOs, rest...);	//对rest函数参数包进行扩展
	}

	template<typename T>
	void DoAnalysisFmt(std::string& strFmtLeft, U64& u64StartFmtIndex, U64& u64NextFmtIndex, CFmtIOString&rOs, const T&tVal)
	{
		if (u64StartFmtIndex != std::string::npos)
		{
			//仍有格式控制符
			auto u64StartFmtIndexTmp = u64StartFmtIndex;
			u64StartFmtIndex = u64NextFmtIndex;	//下一个解析位置

			std::string strFmtObj;
			if (u64NextFmtIndex != std::string::npos)
			{
				strFmtObj = strFmtLeft.substr(u64StartFmtIndexTmp, u64NextFmtIndex - u64StartFmtIndexTmp).c_str();
			}
			else
			{
				strFmtObj = strFmtLeft.substr(u64StartFmtIndexTmp).c_str();
			}

			//解析
			{
				auto nFmtPartLen = strFmtObj.length();

				//buffer长度
				U64 u64BufferSize = nFmtPartLen + 1;
				//std::cout << typeid(tVal).name() << std::endl;
				u64BufferSize += GetBufferAddapterSize<T>::GetBufferNeeded(tVal);

				//合成字符串
				CSmartPtrC<char, char, DEL_WAY_DELETE_MULTI> pszBuffer = new char[u64BufferSize];
				CHECK_NL(pszBuffer);
				//memset(pszBuffer, 0, u64BufferSize);
				pszBuffer[0] = 0;
				auto nLen=sprintf((char*)pszBuffer, (const char *)strFmtObj.c_str(), tVal);
				auto nIndex = ((nLen < u64BufferSize) ? gtool::Max(nLen, 0) : (u64BufferSize - 1));
				pszBuffer[I32(nIndex)] = 0;
				m_strBuffer += (char *)pszBuffer;
			}

			//取下一个格式控制字符串片段
			com_algorithm::NextFmtString(strFmtLeft, u64StartFmtIndex, u64NextFmtIndex);

		}
		else
		{
			//剩余参数
			rOs << tVal;
			//u64NextFmtIndex = std::string::npos;
		}
	}

	template<typename T>
	void AppendNoFmt(const T &tVal)	//特例函数模板终止递归
	{
		(*this) << tVal;
	}

	template<typename T, typename... Args>
	void AppendNoFmt(const T&tVal, const Args&... rest)	//Args&...对 参数包Args进行扩展，得到函数参数包：rest
	{
		(*this) << tVal;
		AppendNoFmt(rest...);	//对rest函数参数包进行扩展
	}

	//类型定义
public:

#if(defined(_WIN32)|defined(_WIN64))
	static const char * endl;
#else
	static const char * endl;
#endif
	
// 	class IOEndl
// 	{
// 		NO_COPY_NEED_RVALUE(IOEndl);
// 	public:
// 		IOEndl() {}
// 		virtual ~IOEndl() {}
// 		operator const char*()
// 		{
// #if(defined(_WIN32)|defined(_WIN64))
// 			return "\r\n";
// #else
// 			return "\n";
// #endif
// 		}
// 	};
	
protected:
	volatile time_t		m_tLastModify = 0;		//最后修改字符串时间戳
	std::string m_strBuffer;		//线程不安全	需要外部上锁
//	boost::format m_strFmt;
	BUFFER32	m_szBuffer = { 0 };	//同一对象线程不安全	需要外部上锁
};

NAMESPACE_END(gtool)

//#pragma MEM_ALIGN_END(1)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





#endif
