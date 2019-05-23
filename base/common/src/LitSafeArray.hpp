#ifndef	LIT_SAFE_ARRAY_HPP_
#define LIT_SAFE_ARRAY_HPP_

#pragma once

NAMESPACE_BEGIN(gtool)

// T: 数组的元素类型
// MAXSIZE: 数组的尺寸
template<class T, size_t MAXSIZE>
class CLitSafeArray
{
public:
	CLitSafeArray()
	{
		memset(m_set, 0, sizeof(T)*MAXSIZE);
	}
	CLitSafeArray(const T* buf, size_t nSize)
	{
		IF_OK_NL(nSize && nSize<=MAXSIZE) memmove(m_set, buf, sizeof(T)*nSize);
	}
	void Clear()
	{
		memset(m_set, 0, sizeof(m_set));
	}
	void ClearSize(size_t nSize, T tValue=T())
	{
		IF_OK_NL(nSize && nSize<=MAXSIZE) memset(m_set, tValue, sizeof(T)*nSize);
	}
	size_t	GetSize()	const
	{
		return MAXSIZE;
	}
	//不支持重载	virtual ~CSafeArray() {}

public: // 重载数组操作符
	template<class E>
	T* operator [] (E nPos)
	{
		//Reading invalid data from 'this->m_set':  the readable size is '160' bytes, but 'nPos' bytes may be read.
#pragma warning( push )  
#pragma warning(disable:6385) 
		IF_NOT_NL(nPos >= 0 && nPos < MAXSIZE)
		{
			ASSERT(false);
			return NULL;
		}
		return &(m_set[nPos]);
#pragma warning( pop )   
	}
	template<class E>
	const T* operator [] (E nPos) const
	{
		//Reading invalid data from 'this->m_set':  the readable size is '160' bytes, but 'nPos' bytes may be read.
#pragma warning( push )  
#pragma warning(disable:6385) 
		IF_NOT_NL(nPos >= 0 && nPos < MAXSIZE)
		{
			ASSERT(false);
		}
		return &(m_set[nPos]);
#pragma warning( pop )   

	}
	void Copy(const CLitSafeArray<T,MAXSIZE>& rFrom)
	{
		CHECK_NL(sizeof(m_set)>=sizeof(rFrom.m_set));
		memmove(m_set, rFrom.m_set, gtool::Min(sizeof(m_set), sizeof(rFrom.m_set)));
	}
public: // 兼容接口
	//operator char* ()						{ return (char*)m_set; }
	operator T* ()
	{
		return (T*)m_set;
	}
	template<class E>
	T* Get(E nPos)
	{
		return (*this)[nPos];
	}

protected:
	T	m_set[MAXSIZE];
};

NAMESPACE_END(gtool)

// 二维数组
#ifndef MY_LITARRAY2
#define	MY_LITARRAY2(T,CX,CY)		gtool::CLitSafeArray<tq::CSafeArray<T, CY>, CX>
#endif


// 三维数组
#ifndef MY_LITARRAY3
#define	MY_LITARRAY3(T,CX,CY,CZ)		gtool::CLitSafeArray<MY_LITARRAY2(T, CY,CZ), CX>
#endif



#endif
