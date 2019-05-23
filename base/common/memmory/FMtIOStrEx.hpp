#ifndef FMT_IO_STR_EX_HPP_
#define FMT_IO_STR_EX_HPP_

#pragma once

//使用内存池的标准输入输出
NAMESPACE_BEGIN(gtool)

template<POOL_SWITCH_TYPE T = POOL_SWITCH_TYPE_CLOSE>
class CFmtIOStrEx :public CFmtIOString
{
	MEM_POOL_SWITCH(T, m_MemPoolAlloctor)

	NO_COPY_NEED_RVALUE(CFmtIOStrEx);

public:
	CFmtIOStrEx<T> & operator = (const char * strVal)
	{
		DOTRY_B_NL
		{
			if (strVal == NULL) return *this;

			m_strBuffer = strVal;
		}
		DOCATCH_ANY_NL("gtool::CFmtIOStrEx::operator = (const char * strVal)")
		{
		}
		DOCATCH_ANY_NL_END

		return *this;
	}

	CFmtIOStrEx<T> &operator = (char * strVal)
	{
		DOTRY_B_NL
		{
			if (strVal == NULL) return *this;

			m_strBuffer = strVal;
		}
		DOCATCH_ANY_NL("gtool::CFmtIOStrEx::operator = (const char * strVal)")
		{
		}
		DOCATCH_ANY_NL_END

		return *this;
	}

public:
	CFmtIOStrEx() :CFmtIOString() {}
	CFmtIOStrEx(const char * str) :CFmtIOString(str) {}
	CFmtIOStrEx(char * str) :CFmtIOString(str) {}
	virtual ~CFmtIOStrEx() {}
};

template<POOL_SWITCH_TYPE T >
CFmtIOStrEx<T>::CFmtIOStrEx(CFmtIOStrEx && rValue)
{
	Clear();
	//m_strFmt.swap(rValue.m_strFmt);
	m_strBuffer = rValue.m_strBuffer.c_str();
}

template<POOL_SWITCH_TYPE T>
MEM_POOL_SWITCH_IMPLEMENTATION(CFmtIOStrEx<T>, T, m_MemPoolAlloctor)

NAMESPACE_END(gtool)

#endif

