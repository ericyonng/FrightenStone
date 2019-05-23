#ifndef LIT_OBJ_ARRAY_HPP_
#define LIT_OBJ_ARRAY_HPP_

#pragma once

NAMESPACE_BEGIN(gtool)

//////////////////////////////////////////////////////////////////////
template <class T, int SZ=0>
class CLitObjArray
{
//-----------------------------------------------
public:	// new traversal

	//////////////////////////////////////////////////////////////////////////
	class	ConstIterator
	{
		enum { VALID_MASK=0x80000000, SIZE_MASK=0x7FFFFFFF };
	protected: // create by parent class
		ConstIterator(const std::vector<T>* pSet) : m_pSet(pSet),m_nValid_Size((U32)(pSet->size())),m_idx(0) {}
		friend class CLitObjArray<T>;

	public: // application
		virtual ~ConstIterator() {}
		bool	Next()
		{
			if(m_idx>=Size()) return false;
			if(!IsValid()) m_nValid_Size+=(U32)(VALID_MASK);
			else m_idx++;
			return m_idx<Size();
		}
		void	Reset()
		{
			m_idx=0;
			m_nValid_Size=pSet->size();
		}
		operator const T*()
		{
			ASSERT(IsValid());
			return &m_pSet->operator[](m_idx);
		}
		const T * operator->()
		{
			ASSERT(IsValid());
			return &m_pSet->operator[](m_idx);
		}
	protected:
		bool	IsValid()
		{
			return (m_nValid_Size&VALID_MASK) && m_idx<Size();
		}
		int		Size()
		{
			return m_nValid_Size & SIZE_MASK;
		}
		void	DelObj()
		{
			ASSERT_T(Size());    // set invalid
			m_nValid_Size = Size()-1;
		}

	protected:
		const std::vector<T>*	m_pSet;
		U32					m_nValid_Size;
		size_t					m_idx;
	};

	//////////////////////////////////////////////////////////////////////////
	class	Iterator
	{
		enum { VALID_MASK=0x80000000, SIZE_MASK=0x7FFFFFFF };
	protected: // create by parent class
		Iterator(std::vector<T>* pSet) : m_pSet(pSet),m_nValid_Size(pSet->size()),m_idx(0) {}
		friend class CLitObjArray<T>;

	public: // application
		virtual ~Iterator() {}
		bool	Next()
		{
			if(m_idx>=Size())
				return false;
			if(!IsValid())
				m_nValid_Size+=(size_t)VALID_MASK;
			else
				m_idx++;
			return m_idx< (size_t)Size();
		}
		void	Reset()
		{
			m_idx=0;
			m_nValid_Size=m_pSet->size();
		}
		operator T*()
		{
			ASSERT(IsValid());
			return &m_pSet->operator[](m_idx);
		}
		T * operator->()
		{
			ASSERT(IsValid());
			return &m_pSet->operator[](m_idx);
		}
		size_t	GetIdx() const
		{
			return m_idx;
		}
	protected:
		bool	IsValid()
		{
			return (m_nValid_Size&0x80000000) && m_idx<static_cast<int>(m_pSet->size());
		}
		int		Size()
		{
			return m_nValid_Size & 0x7FFFFFFF;
		}
		void	DelObj()
		{
			ASSERT_T(Size());    // set invalid
			m_nValid_Size = Size()-1;
		}
	protected:
		std::vector<T>*			m_pSet;
		size_t					m_nValid_Size;
		size_t					m_idx;
	};

	//////////////////////////////////////////////////////////////////////////
public: // traverse
	const ConstIterator	NewEnum	() 	const
	{
		return ConstIterator(&m_set);
	}
	Iterator	NewEnum	()
	{
		return Iterator(&m_set);
	}
	int			GetAmount	() 	const
	{
		return static_cast<int>(m_set.size());
	}
	int			Size	() 	const
	{
		return static_cast<int>(m_set.size());
	}
	bool		IsEmpty	() 	const
	{
		return m_set.empty();
	}
//-----------------------------------------------

public:
	CLitObjArray()
	{
		m_set.resize(SZ,T());
	}
	virtual ~CLitObjArray()
	{
		m_set.clear();
	}
	explicit CLitObjArray(int nSize, T data)
	{
		m_set.resize(nSize, data);
	}
	CLitObjArray(const CLitObjArray& obj)
	{
		m_set = obj.m_set;
	}
	CLitObjArray<T>& operator=(const CLitObjArray<T>& obj)
	{
		if(this==&obj) return *this;
		m_set = obj.m_set;
		return *this;
	}

public:
	T&		operator[](int idx)
	{
		ASSERT(idx>=0 && idx<static_cast<int>(m_set.size()));
		return m_set[idx];
	}
	T&		At(int idx)
	{
		ASSERT(idx>=0 && idx<static_cast<int>(m_set.size()));
		return m_set[idx];
	}
	const T& operator[](int idx) const
	{
		ASSERT(idx>=0 && idx<static_cast<int>(m_set.size()));
		return m_set[idx];
	}
	const T& At(int idx) const
	{
		ASSERT(idx>=0 && idx<static_cast<int>(m_set.size()));
		return m_set[idx];
	}

	void	Clear()
	{
		m_set.clear();
	}
	void	Resize(int nSize, T t=T())
	{
		m_set.resize(nSize,t);
	}
	void	Reserve(int nSize)
	{
		m_set.reserve(nSize);
	}

	void	Fill(const T& obj)
	{
		fill(m_set.begin(), m_set.end(), obj);
	}
	bool	Find(const T& obj)				const
	{
		return find(m_set.begin(), m_set.end(), obj) != m_set.end();
	}
	bool	Push(const T& obj)
	{
		m_set.push_back(obj);
		return true;
	}
	bool	Erase(int idx);
	bool	Insert(int idx, const T& obj);
	bool	Insert(Iterator& itor, const T& obj);
	T		PopBack()
	{
		if(m_set.empty()) return T();
		T obj=m_set[m_set.size()-1];
		m_set.pop_back();
		return obj;
	}
	T&		Back()
	{
		ASSERT_T(!m_set.empty());
		return m_set.back();
	}

	bool	DelObj(Iterator& iter)
	{
		if(!iter.IsValid()) return false;
		m_set.erase(m_set.begin()+iter.m_idx);
		iter.DelObj();
		return true;
	}
	bool	DelObj(int idx)
	{
		ASSERT_T(idx<static_cast<int>(m_set.size()));
		m_set.erase(m_set.begin()+idx);
		return true;
	}
	template<class X>
	void CopyFrom(const CLitObjArray<X>& obj)
	{
		Clear();
		for(int i=0; i<obj.Size(); i++) Push(obj[i]);
	}
	template<class X>
	void CopyFrom(const X buf[], int nSize)
	{
		Clear();
		for(int i=0; i<nSize; i++) Push(buf[i]);
	}

protected:
	std::vector<T>		m_set;
};


//////////////////////////////////////////////////////////////////////////
template <class T, int SZ>
bool CLitObjArray<T, SZ>::Erase( int idx )
{
	IF_NOT(idx >= 0)
	return false;
	m_set.erase(m_set.begin()+idx);
	return true;
}

template <class T, int SZ>
bool CLitObjArray<T,SZ>::Insert(int idx, const T& obj)
{
	IF_NOT(idx >= 0)
	return false;

	std::vector<T>::iterator it = m_set.begin() + idx;
	//std::vector<T>::iterator it = (idx<m_set.size()-1)?m_set.begin() + idx:m_set.end();
// 	IF_NOT(it != m_set.end())
// 		return false;

	m_set.insert(it, obj);
	return true;
}

template <class T, int SZ>
bool CLitObjArray<T,SZ>::Insert(Iterator& itor, const T& obj)
{
	const size_t idx = itor.GetIdx();
	std::vector<T>::iterator it = m_set.begin() + idx;
//	std::vector<T>::iterator it = (idx<m_set.size()-1)?m_set.begin() + idx:m_set.end();

// 	IF_NOT(it != m_set.end())
// 		return false;

	m_set.insert(it, obj);
	return true;
}

NAMESPACE_END(gtool)

#endif 
