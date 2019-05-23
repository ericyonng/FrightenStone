#ifndef	LIT_QUEUE_HPP_
#define LIT_QUEUE_HPP_


NAMESPACE_BEGIN(gtool)


template	< typename T, bool is_class = boost::is_class<T>::value >
class LitQueue
{
public:
	typedef	std::deque<T>				OBJDQUEUE;
protected:
	typedef	typename OBJDQUEUE::iterator			Iter;		//std容器迭代器
	typedef	typename OBJDQUEUE::const_iterator	ConstIter;	//std容器迭代器

public:
	//////////////////////////////////////////////////////////////////////////
	class	ConstIterator
	{
	protected: // create by parent class
		enum { VALID_MASK=0x80000000, SIZE_MASK=0x7FFFFFFF };
		ConstIterator(const OBJDQUEUE* pSet) : m_pSet(pSet),m_nValid_Size(pSet->size()),m_idx(0) {}
		friend class LitQueue<T, is_class>;

	public: // application
		virtual ~ConstIterator() {}
		bool	Next()
		{
			if(m_idx>=Size()) return false;
			if(!IsValid()) m_nValid_Size+=VALID_MASK;
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
			return &m_pSet->at(m_idx);
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
			ASSERT(Size());    // set invalid
			m_nValid_Size = Size()-1;
		}

	protected:
		const OBJDQUEUE*	m_pSet;
		unsigned long		m_nValid_Size;
		size_t				m_idx;
	};

	//////////////////////////////////////////////////////////////////////////
	class	Iterator
	{
	protected: // create by parent class
		Iterator(OBJDQUEUE* pSet) : m_pSet(pSet),m_nValid_Size((int)pSet->size()),m_idx(0) {}
		friend class LitQueue<T, is_class>;

		static const size_t VALID_MASK=0x80000000;
		static const size_t SIZE_MASK=0x7FFFFFFF;

	public: // application
		virtual ~Iterator() {}
		bool	Next()
		{
			if(m_idx>=Size()) return false;
			if(!IsValid()) m_nValid_Size+=VALID_MASK;
			else m_idx++;
			return m_idx<Size();
		}
		void	Reset()
		{
			m_idx=0;
			m_nValid_Size=m_pSet->size();
		}
		operator T*()
		{
			ASSERT(IsValid());
			return &m_pSet->at(m_idx);
		}
		T * operator->()
		{
			ASSERT(IsValid());
			return &m_pSet->operator[](m_idx);
		}
	protected:
		bool	IsValid()
		{
			return (m_nValid_Size&VALID_MASK) && m_idx<m_pSet->size();
		}
		size_t	Size()
		{
			return m_nValid_Size & SIZE_MASK;
		}
		void	DelObj()
		{
			ASSERT_T(Size());    // set invalid
			m_nValid_Size = Size()-1;
		}
		int		GetIdx() const
		{
			return m_idx;
		}
	protected:
		OBJDQUEUE*	m_pSet;
		size_t		m_nValid_Size;
		size_t		m_idx;
	};

	//////////////////////////////////////////////////////
	//迭代器相关接口
public: // traverse
	Iterator	NewEnum		()
	{
		return Iterator(&m_set);
	}
	ConstIterator	NewConstEnum		()
	{
		return ConstIterator(&m_set);
	}

//////////////////////////////////////////////////////
protected:
	LitQueue() {}
	virtual ~LitQueue();
public:
	static LitQueue<T, is_class>*	CreateNew()
	{
		return new LitQueue<T, is_class>();
	}

	//////////////////////////////////////////////////////////////////////////
	//interface of IGameMap
public:
	I32	Release()
	{
		delete this;
		return 0;
	}

	T&		operator[](int idx)
	{
		ASSERT(idx>=0 && idx<m_set.size());
		return m_set[idx];
	}
	const T& operator[](int idx) const
	{
		ASSERT(idx>=0 && idx<m_set.size());
		return m_set[idx];
	}

	bool	PushBack(const T& val);
	bool	PushFront(const T& val);
	bool	DelObj(Iterator& iter)
	{
		if(!iter.IsValid()) return false;
		m_set.erase(m_set.begin()+iter.m_idx);
		iter.DelObj();
		return true;
	}
	T		PopBack();
	T		PopFront();

	bool	IsEmpty()							const
	{
		return m_set.empty();
	}
	int		GetAmount() 						const
	{
		return (int)m_set.size();
	}
	bool	IsExist(const T& val)				const
	{
		return std::find(m_set.begin(), m_set.end(), val) != m_set.end();
	}
	void	Clear();

protected:
	template <typename, bool> struct RET { };
	template <>	struct RET<T, true>
	{
		T NULLT() const
		{
			return T();
		}
	};
	template <> struct RET<T, false>
	{
		T NULLT() const
		{
			return 0;
		}
	};
	RET<T, is_class> m_ret;

protected:
	OBJDQUEUE		m_set;
};
template	< class T, bool is_class>
LitQueue<T, is_class>::~LitQueue()
{
	Clear();
}

template	< class T, bool is_class>
void LitQueue<T, is_class>::Clear()
{
	m_set.clear();
}

template	< class T, bool is_class>
bool LitQueue<T, is_class>::PushBack(const T& val)
{
	m_set.push_back(val);
	return true;
}

template	< class T, bool is_class>
bool LitQueue<T, is_class>::PushFront(const T& val)
{
	m_set.push_front(val);
	return true;
}

template	< class T, bool is_class>
T LitQueue<T, is_class>::PopBack()
{
	if(m_set.empty())
		return m_ret.NULLT();
	T t = m_set.back();
	m_set.pop_back();
	return t;
}

template	< class T, bool is_class>
T LitQueue<T, is_class>::PopFront()
{
	if(m_set.empty())
		return m_ret.NULLT();
	T t = m_set.front();
	m_set.pop_front();
	return t;
}


NAMESPACE_END(gtool)


#endif
