#ifndef	LIT_VECTOR_HPP_
#define LIT_VECTOR_HPP_

#pragma once

NAMESPACE_BEGIN(gtool)

template <typename T>
class CLitVector : public std::vector<T>
{	
public:
	class	ConstIterator
	{
		enum { VALID_MASK = 0x80000000, SIZE_MASK = 0x7FFFFFFF };
	private: // create by parent class
		ConstIterator(const std::vector<T>* pSet) : m_pSet(pSet), m_nValid_Size(pSet->size()), m_idx(0) {}
		friend class CLitVector<T>;

	public: // application
		bool	Next()
		{
			if (m_idx >= Size()) return false;
			if (!IsValid()) m_nValid_Size += VALID_MASK;
			else m_idx++;
			return m_idx < Size();
		}
		void	Reset()
		{
			m_idx = 0;
			m_nValid_Size = pSet->size();
		}
		operator const T*()
		{
			IF_OK_NL(IsValid()) return &m_pSet->at(m_idx);
			return NULL;
		}
		const T * operator->()
		{
			IF_OK_NL(IsValid()) return &m_pSet->operator[](m_idx);
			return NULL;
		}
	protected:
		bool	IsValid()
		{
			return (m_nValid_Size&VALID_MASK) && m_idx < Size();
		}
		int		Size()
		{
			return m_nValid_Size & SIZE_MASK;
		}
		void	DelObj()
		{
			IF_OK_NL(Size()) m_nValid_Size = Size() - 1;    // set invalid
			return;
		}

	protected:
		const std::vector<T>*	m_pSet;
		I64		m_nValid_Size;
		I64					m_idx;
	};
	class	Iterator
	{
		enum { VALID_MASK = 0x80000000, SIZE_MASK = 0x7FFFFFFF };
	private: // create by parent class
		Iterator(std::vector<T>* pSet) : m_pSet(pSet), m_nValid_Size(pSet->size()), m_idx(0) {}
		friend class CLitVector<T>;

	public: // application
		bool	Next()
		{
			if (m_idx >= Size()) return false;
			if (!IsValid()) m_nValid_Size += VALID_MASK;
			else m_idx++;
			return m_idx < Size();
		}
		void	Reset()
		{
			m_idx = 0;
			m_nValid_Size = m_pSet->size();
		}
		operator T*()
		{
			IF_OK_NL(IsValid()) return &m_pSet->at(m_idx);
			return NULL;
		}
		T * operator->()
		{
			IF_OK_NL(IsValid()) return &m_pSet->operator[](m_idx);
			return NULL;
		}

	protected:
		bool	IsValid()
		{
			return (m_nValid_Size & 0x80000000) && m_idx < m_pSet->size();
		}
		int		Size()
		{
			return m_nValid_Size & 0x7FFFFFFF;
		}
		void	DelObj()
		{
			IF_OK_NL(Size()) m_nValid_Size = Size() - 1;    // set invalid
			return;
		}

	protected:
		std::vector<T>*			m_pSet;
		I64		m_nValid_Size;
		I64					m_idx;
	};
public: // traverse
	const ConstIterator	NewEnum() 	const
	{
		return ConstIterator(this);
	}
	Iterator	NewEnum()
	{
		return Iterator(this);
	}
	U64			Size() 	const
	{
		return this->size();
	}
	//-----------------------------------------------

public:

#pragma warning(push)
#pragma warning(disable: 4018 4715)	// 'function' : not all control paths return a value 
	// 重载vector操作符 防止越界访问
	template<class E>
	T& operator [] (E position)
	{
		if (position >= 0 && position < this->size())
		{
			return this->at(position);
		}
		DOTRY_B_NL
		{
			throw (0);
		}
		DOCATCH_ANY_NL("★CLitVector()★ position error")
		{
		}
		DOCATCH_ANY_NL_END
		return this->at(position);
	}
	template<class E>
	const T& operator [] (E position) const
	{
		if (position >= 0 && position < this->size())
		{
			return this->at(position);
		}
		DOTRY_B_NL
		{
			throw (0);
		}
			DOCATCH_ANY_NL("★CLitVector()★ position error")
		{
		}
		DOCATCH_ANY_NL_END
		return this->at(position);
	}
#pragma warning(pop)
public:
	void	Clear()
	{
		this->clear();
	}
	void	Push(const T& obj)
	{
		this->push_back(obj);
	}
	bool	DelObj(Iterator& iter)
	{
		if (!iter.IsValid()) return false;
		this->erase(this->begin() + iter.m_idx);
		iter.DelObj();
		return true;
	}
	bool	DelObj(const size_t idx)
	{
		if (idx >= this->size()) return false;
		this->erase(this->begin() + idx);
		return true;
	}

};


NAMESPACE_END(gtool)

#endif