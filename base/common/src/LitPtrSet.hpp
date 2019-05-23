#ifndef	LIT_PTR_SET_HPP_
#define LIT_PTR_SET_HPP_

NAMESPACE_BEGIN(gtool)

//template CLitPtrSet 逆向的vector
template	< typename T, DEL_WAY eDelWay = DEL_WAY_DELETE>
class CLitPtrSet	//内部vector实现
{
public:
	typedef	std::vector<T*>						OBJSET;
	typedef	typename OBJSET::iterator			Iter;			//std容器迭代器
	typedef	typename OBJSET::const_iterator		ConstIter;		//std容器迭代器
public:
	static CLitPtrSet<T, eDelWay>*	CreateNew(bool bOwner=true)
	{
		return new CLitPtrSet<T, eDelWay>(bOwner);
	}

//////////////////////////////////////////////////////
public:
	//////////////////////////////////////////////////////
	//迭代器封装
	class	Iterator
	{
	private: // create by parent class
		Iterator(OBJSET* pSet) : m_pSet(pSet),m_idx(pSet->size()-1),m_pObj(0) {}
		friend class CLitPtrSet<T, eDelWay>;
	public: // application
		bool	Next()
		{
			if(m_idx<0) return m_pObj=NULL,false;
			m_pObj=m_pSet->operator[](m_idx);
			m_idx--;
			return true;
		}
		void	Reset()
		{
			m_idx=m_pSet->size()-1;
			m_pObj=NULL;
		}
		operator T*()
		{
			return m_pObj;
		}
		T * operator->()
		{
			return m_pObj;
		}

	protected:
		OBJSET*		m_pSet;
		INT_PTR			m_idx;
		T*			m_pObj;
	};

	class	ConstIterator
	{
	private: // create by parent class
		ConstIterator(const OBJSET* pSet) : m_pSet(pSet),m_idx(pSet->size()-1),m_pObj(0) {}
		friend class CLitPtrSet<T, eDelWay>;
	public: // application
		bool	Next()
		{
			if(m_idx<0) return m_pObj=NULL,false;
			m_pObj=m_pSet->operator[](m_idx);
			m_idx--;
			return true;
		}
		void	Reset()
		{
			m_idx=m_pSet->size()-1;
			m_pObj=NULL;
		}
		operator const T*()
		{
			return m_pObj;
		}
		const T * operator->()
		{
			return m_pObj;
		}

	protected:
		const OBJSET*		m_pSet;
		INT_PTR		m_idx;
		const T*			m_pObj;
	};

	//////////////////////////////////////////////////////
	//迭代器相关接口
public: // traverse
	Iterator	NewEnum		()
	{
		return Iterator(&m_setObj);
	}
	ConstIterator	NewConstEnum() 	const
	{
		return ConstIterator(&m_setObj);
	}

	//////////////////////////////////////////////////////////////////////////
protected:
	CLitPtrSet(bool bOwner):m_bOwner(bOwner) {}
	virtual ~CLitPtrSet();

	//////////////////////////////////////////////////////////////////////////
	//interface of IGameObjSet
public:
	I32	Release()
	{
		delete this;
		return 0;
	}
	Iter	Begin()
	{
		return m_setObj.begin();
	}
	Iter	End()
	{
		return m_setObj.end();
	}

	bool	AddObj(T* pObj);
	bool	DelObjByIndex(size_t idx);
	T*		GetObjByIndex(size_t idx)			const
	{
		IF_NOT_NL(idx<m_setObj.size()) return nullptr;
		return m_setObj[idx];
	}
	//把最后一个节点的指针copy到目标节点，删除最后节点，删除目标节点原对象
	bool	MoveEndToIndex(size_t idx);

	size_t	GetAmount() 						const
	{
		return m_setObj.size();
	}
	void	Reserve(typename OBJSET::size_type n)
	{
		m_setObj.reserve(n);
	}

	//////////////////////////////////////////////////////////////////////////
public:
	void	Clear();
	bool	DelObj(Iterator& iter);
	void	Swap(CLitPtrSet<T, eDelWay>& lstTarget);
protected:
	T*		PopObj(Iterator& iter);

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
	OBJSET	m_setObj;
	bool	m_bOwner;
	Del<T, eDelWay> m_Del;
};

//////////////////////////////////////////////////////////////////////////
template	< typename T, DEL_WAY eDelWay>
bool CLitPtrSet<T, eDelWay>::MoveEndToIndex( size_t idx )
{
	const size_t szOld = m_setObj.size();
	if(szOld < idx+1)
		return false;

	T*& pOld = m_setObj[idx];
	if(m_bOwner)
	{
		m_Del.DelFun(pOld);
		pOld = NULL;
	}

	if(idx+1 == szOld)
	{
		//删掉最后一个
		m_setObj.pop_back();
		return true;
	}

	T*& pEnd = m_setObj[szOld-1];
	pOld = pEnd;

	m_setObj.pop_back();

	return true;
}

template	< typename T, DEL_WAY eDelWay>
void CLitPtrSet<T, eDelWay>::Swap(CLitPtrSet<T, eDelWay>& lstTarget )
{
	m_setObj.swap(lstTarget.m_setObj);
}

//////////////////////////////////////////////////////////////////////////
//接口实现 CLitPtrSet

//////////////////////////////////////////////////////////////////////
template<typename T, DEL_WAY eDelWay>
CLitPtrSet<T, eDelWay>::~CLitPtrSet()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////
template<typename T, DEL_WAY eDelWay>
void CLitPtrSet<T, eDelWay>::Clear()
{
	if(m_bOwner)
	{
		for(OBJSET::size_type i=0; i<m_setObj.size(); i++)
		{
			T* pObj=m_setObj[i];
			if(pObj)
			{
				m_setObj[i] = NULL;		// 加强安全性
				m_Del.DelFun(pObj);
			}
		}
	}
	m_setObj.clear();
}

//////////////////////////////////////////////////////////////////////
template< typename T, DEL_WAY eDelWay>
bool CLitPtrSet<T, eDelWay>::AddObj(T* pObj)
{
	if(pObj == NULL)
		return false;

	m_setObj.push_back(pObj);
	return true;
}

//////////////////////////////////////////////////////////////////////
template	< typename T, DEL_WAY eDelWay>
bool CLitPtrSet<T, eDelWay>::DelObjByIndex( size_t idx )
{
	if(m_setObj.size() < idx+1)
		return false;
	Iter it = m_setObj.begin();
	std::advance(it, idx);
	if(m_bOwner)
	{
		IF_OK_T(it != m_setObj.end())
		m_Del.DelFun(*it);
	}
	//ASSERT_T(m_setObj.erase(it) != NULL);
	m_setObj.erase(it);

	return true;
}

//////////////////////////////////////////////////////////////////////
template< typename T, DEL_WAY eDelWay>
bool CLitPtrSet<T, eDelWay>::DelObj(Iterator& iter)
{
	if(iter.m_pObj)
	{
		T* pObj = PopObj(iter);
		if(m_bOwner)
			m_Del.DelFun(pObj);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
template< typename T, DEL_WAY eDelWay>
T* CLitPtrSet<T, eDelWay>::PopObj(Iterator& iter)
{
	if(iter.m_pObj)
	{
		T* pObj = iter.m_pObj;
		iter.m_pObj	= NULL;

		size_t	idx = iter.m_idx+1;
		IF_OK_T(idx < m_setObj.size())
		{
			m_setObj.erase(m_setObj.begin() + idx);
			return pObj;
		}
	}
	return NULL;
}



NAMESPACE_END(gtool)

#endif 
