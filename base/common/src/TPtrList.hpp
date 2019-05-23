#ifndef TPTR_LIST_H_
#define TPTR_LIST_H_

#pragma once

// #include <list>
// #include <algorithm>
//#include "String.h"
// #include <Boost/type_traits/is_class.hpp>
// #include <BaseCode/MyBaseMacro.h>
// #include <BaseCode/enumtype.h>
// #include <BaseCode/CheckNoLog.h>

NAMESPACE_BEGIN(gtool)

//////////////////////////////////////////////////////////////////////////
//TPtrList

template< class T, DEL_WAY delWay = DEL_WAY_DELETE, bool is_class = boost::is_class<T>::value >
class TPtrList
{
public:
	typedef	std::list<T*>				OBJPTRLIST;
protected:
	typedef	typename OBJPTRLIST::iterator			Iter;		//stdÈÝÆ÷µü´úÆ÷
	typedef	typename OBJPTRLIST::const_iterator		ConstIter;	//stdÈÝÆ÷µü´úÆ÷

public:
	//////////////////////////////////////////////////////
	//µü´úÆ÷·â×°
	class	Iterator
	{
	public:
		Iterator():m_pSet(NULL), m_pObj(NULL){}

	private: // create by parent class
		Iterator(OBJPTRLIST* pSet) : m_pSet(pSet), m_iter(pSet->begin()), m_pObj(0) {}
		Iterator(OBJPTRLIST* pSet, bool bEndFlag) : m_pSet(pSet), m_iter(pSet->end()), m_pObj(0) {}
		friend class TPtrList<T, delWay, is_class>;

	public: // application
		Iterator & operator =(Iterator& other)
		{
			m_pSet = other.m_pSet;
			m_iter = other.m_iter;
			m_pObj = other.m_pObj;
			other.m_pSet = NULL;
			other.m_iter = m_pSet->begin();
			other.m_pObj = NULL;

			return *this;
		}
		bool	Next()
		{
			if (m_iter == m_pSet->end()) return m_pObj = NULL, false;
			m_pObj = (*m_iter);
			ASSERT(m_pObj);
			++m_iter;
			return true;
		}

		bool Pre()
		{
			if (m_iter == m_pSet->begin()) return false;
			--m_iter;
			m_pObj = (*m_iter);
			ASSERT(m_pObj);		
			return true;
		}

		void	Reset()
		{
			m_iter = m_pSet->begin();
			m_pObj = NULL;
		}

		void ResetEnd()
		{
			m_iter = m_pSet->end();
			m_pObj = NULL;
		}
		operator T*()
		{
			return m_pObj;
		}
		T * operator->()
		{
			ASSERT(m_pObj);
			return m_pObj;
		}
	protected:
		Iterator & operator =(const Iterator& other) = delete;

	protected:
		OBJPTRLIST * m_pSet=NULL;
		Iter		m_iter;
		T*	m_pObj=NULL;
	};
	//////////////////////////////////////////////////////
	class	ConstIterator
	{
	private: 
		ConstIterator(const OBJPTRLIST* pSet) : m_pSet(pSet), m_iter(pSet->begin()), m_pObj(0) {}
		ConstIterator(const OBJPTRLIST* pSet, bool bEndFlag) : m_pSet(pSet), m_iter(pSet->end()), m_pObj(0) {}
		friend class TPtrList<T, delWay, is_class>;

	public: // application
		virtual ~ConstIterator() {}
		bool	Next()
		{
			if (m_iter == m_pSet->end()) return m_pObj = NULL, false;
			m_pObj = (*m_iter);
			ASSERT(m_pObj);
			++m_iter;
			return true;
		}

		bool Pre()
		{
			if (m_iter == m_pSet->begin()) return false;
			--m_iter;
			m_pObj = (*m_iter);
			ASSERT(m_pObj);
			return true;
		}

		void	Reset()
		{
			m_iter = m_pSet->begin();
			m_pObj = NULL;
		}

		void ResetEnd()
		{
			m_iter = m_pSet->end();
			m_pObj = NULL;
		}
		operator const T*()
		{
			return m_pObj;
		}
		const T * operator->()
		{
			ASSERT(m_pObj);
			return m_pObj;
		}

	protected:
		const OBJPTRLIST*	m_pSet=NULL;
		ConstIter		m_iter;
		const T*	m_pObj=NULL;
	};
	//////////////////////////////////////////////////////
public:
	Iterator	NewEnum()
	{
		return Iterator(&m_set);
	}
	Iterator NewEndEnum()
	{
		return Iterator(&m_set, true);
	}
	ConstIterator	NewConstEnum()
	{
		return ConstIterator(&m_set);
	}

	//////////////////////////////////////////////////////
public:
	TPtrList() {}
	virtual ~TPtrList();
public:
	static TPtrList<T, delWay, is_class >*	CreateNew()
	{
		return new TPtrList<T, delWay, is_class>();
	}

public:
	int	Release()
	{
		delete this;
		return 0;
	}

	bool	PushBack(T* pObj);
	bool	PushFront(T* pObj);

	bool	DelObj(const T& val);
	bool	DelFirstObj(const T& val);
	bool	DelObj(Iterator & iter);
	T*		Front();
	const T*Front() const;
	T*		Back();
	const T*Back() const;
	T*		PopBack();
	T*		PopFront();
	T *		PopObj(Iterator & iter);

	bool	IsEmpty()							const
	{
		return m_set.empty();
	}
	size_t	GetAmount() 						const
	{
		return m_set.size();
	}
	bool	IsExist(const T* pObj)				const
	{
		return std::find(m_set.begin(), m_set.end(), pObj) != m_set.end();
	}
	void	Clear();

	void PopAll();

	bool Swap(TPtrList<T, delWay , is_class>& rOther);

	
protected:

	void	ReleaseObj(T* pT);

	template <typename, DEL_WAY>
	struct Del
	{
		void DelFun(T* pT)
		{
		}
	};

	template <>
	struct Del<T, DEL_WAY_DELETE>
	{
		void DelFun(T* pT)
		{
			SAFE_DELETE_NL(pT);
		}
	};

	template <>
	struct Del<T, DEL_WAY_DELETE_MULTI>
	{
		void DelFun(T* pT)
		{
			SAFE_DELETE_MULTI_NL(pT);
		}
	};

	template <>
	struct Del<T, DEL_WAY_RELEASE>
	{
		void DelFun(T* pT)
		{
			SAFE_RELEASE_NL(pT);
		}
	};

protected:
	OBJPTRLIST		m_set;
	Del<T, delWay> m_Del;
};
template	<  class T, DEL_WAY delWay, bool is_class >
TPtrList<T, delWay, is_class >::~TPtrList()
{
	Clear();
}

template	<  class T, DEL_WAY delWay, bool is_class >
void TPtrList<T, delWay, is_class>::Clear()
{
	for (Iter iter = m_set.begin(); iter != m_set.end(); ++iter)
	{
		T* ptr = *iter;
		*iter = NULL;
		ReleaseObj(ptr);
	}
	m_set.clear();
}

template	<  class T, DEL_WAY delWay, bool is_class >
void TPtrList<T, delWay, is_class>::PopAll()
{
	m_set.clear();
}

template	<  class T, DEL_WAY delWay, bool is_class >
bool TPtrList<T, delWay, is_class>::PushBack(T* pObj)
{
	m_set.push_back(pObj);
	return true;
}

template	<  class T, DEL_WAY delWay, bool is_class >
bool TPtrList<T, delWay, is_class>::PushFront(T* pObj)
{
	m_set.push_front(pObj);
	return true;
}

template	<  class T, DEL_WAY delWay, bool is_class >
bool TPtrList<T, delWay, is_class>::DelObj(const T& val)
{
	bool bDel = false;
	Iter i = m_set.begin();
	while (i != m_set.end())
	{
		T* pTmpValue = *i;
		if (*pTmpValue == val)
		{
			ReleaseObj(pTmpValue);
			i = m_set.erase(i);
			bDel = true;
		}
		else
			++i;
	}
	return bDel;
}

template	<  class T, DEL_WAY delWay, bool is_class >
bool	TPtrList<T, delWay, is_class>::DelFirstObj(const T& val)
{
	Iter i = m_set.begin();
	while (i != m_set.end())
	{
		T* pTmpValue = *i;
		if (*pTmpValue == val)
		{
			ReleaseObj(pTmpValue);
			i = m_set.erase(i);
			return true;
		}
		else
		{
			++i;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
template	<  class T, DEL_WAY delWay, bool is_class >
bool TPtrList<T, delWay, is_class>::DelObj(Iterator & iter)
{
	if (iter.m_pObj)
	{
		T* pObj = PopObj(iter);
		IF_OK(pObj)
		{
			ReleaseObj(pObj);
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
template	<  class T, DEL_WAY delWay, bool is_class>
T* TPtrList<T, delWay, is_class>::PopObj(Iterator & iter)
{
	if (iter.m_pObj)
	{
		T* pObj = iter.m_pObj;
		iter.m_pObj = NULL;

		CLASSIF_OK_NL(iter.m_iter != m_set.begin())
		{
			Iter	temp = iter.m_iter;
			--temp;
			m_set.erase(temp);
			return pObj;
		}
	}
	return NULL;
}

template	<  class T, DEL_WAY delWay, bool is_class >
T* TPtrList<T, delWay, is_class>::Front()
{
	ASSERT(!m_set.empty());
	return m_set.front();
}

template	< class T,  DEL_WAY delWay, bool is_class>
const T* TPtrList<T, delWay, is_class>::Front() const
{
	ASSERT(!m_set.empty());
	return m_set.front();
}

template	<  class T, DEL_WAY delWay, bool is_class >
const T* TPtrList<T, delWay, is_class>::Back() const
{
	ASSERT(!m_set.empty());
	return m_set.back();
}

template	<  class T, DEL_WAY delWay, bool is_class >
T* TPtrList<T, delWay, is_class>::Back()
{
	ASSERT(!m_set.empty());
	return m_set.back();
}

template	<  class T, DEL_WAY delWay, bool is_class >
T* TPtrList<T, delWay, is_class>::PopBack()
{
	if (m_set.empty())
		return NULL;
	T* t = m_set.back();
	m_set.pop_back();
	return t;
}

template	< class T, DEL_WAY delWay, bool is_class >
T* TPtrList<T, delWay, is_class>::PopFront()
{
	if (m_set.empty())
		return NULL;
	T* t = m_set.front();
	m_set.pop_front();
	return t;
}

template	< class T, DEL_WAY delWay, bool is_class>
void TPtrList<T, delWay, is_class>::ReleaseObj(T* pT)
{
	m_Del.DelFun(pT);
}

template	< class T, DEL_WAY delWay, bool is_class>
bool TPtrList<T, delWay, is_class>::Swap(TPtrList<T, delWay, is_class>& rOther)
{
	std::swap(this->m_set, rOther.m_set);
	return true;
}

NAMESPACE_END(gtool)



#endif