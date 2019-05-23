#ifndef	LIT_PTR_MULTIMAP_HPP_
#define LIT_PTR_MULTIMAP_HPP_

#pragma once


NAMESPACE_BEGIN(gtool)


template<typename T, typename KEY = INT64, DEL_WAY eDelWay = DEL_WAY_RELEASE >
class CLitPtrMultiMap
{
public:
	typedef	std::multimap<KEY, T*>			OBJMULTIMAP;
	typedef	typename OBJMULTIMAP::iterator			Iter;		//std容器迭代器
	typedef	typename OBJMULTIMAP::const_iterator		ConstIter;	//std容器迭代器
	typedef std::pair<Iter, Iter> RangePair;
public:
	static CLitPtrMultiMap<T, KEY, eDelWay>*	CreateNew()
	{
		return new CLitPtrMultiMap<T, KEY, eDelWay>();
	}

public:
	//////////////////////////////////////////////////////
	//迭代器封装
	class	Iterator
	{
	private: // create by parent class
		Iterator(OBJMULTIMAP* pSet) : m_pSet(pSet), m_range(pSet->begin(), pSet->end()), m_iter(pSet->begin()), m_pObj(0) {}
		Iterator(OBJMULTIMAP* pSet, const KEY& key) : m_pSet(pSet), m_range(pSet->equal_range(key)), m_iter(m_range.first), m_pObj(0) {}
		friend class CLitPtrMultiMap < T, KEY, eDelWay > ;

	public: // application
		bool	Next()
		{
			if (m_iter == m_range.second) return m_pObj = NULL, false;
			m_key = m_iter->first;
			m_pObj = m_iter->second;
			ASSERT(m_pObj);
			++m_iter;
			return true;
		}
		void	Reset()
		{
			m_iter = m_range.first;
			m_pObj = NULL;
			m_range.first = m_pSet->end();
			m_range.second = m_range.first;
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
		KEY		GetKey() const
		{
			IF_OK_NL(m_pObj) return m_key;    //std::string(NULL)会崩溃
			return m_ret.NULLT();
		}

	protected:
		OBJMULTIMAP*	m_pSet;
		RangePair		m_range;
		Iter			m_iter;
		T*				m_pObj;
		KEY				m_key=KEY();

	protected:
		template <typename, bool>
		struct RET { };
		template <>	struct RET < KEY, true >
		{
			KEY NULLT() const
			{
				return KEY();
			}
		};
		template <> struct RET < KEY, false >
		{
			KEY NULLT() const
			{
				return 0;
			}
		};
		RET<KEY, boost::is_class<KEY>::value> m_ret;
	};
	//////////////////////////////////////////////////////
	//迭代器相关接口
public: // traverse
	Iterator	NewEnum()
	{
		return Iterator(&m_map);
	}
	Iterator	NewEnum(const KEY& key)
	{
		return Iterator(&m_map, key);
	}

	//////////////////////////////////////////////////////
protected:
	CLitPtrMultiMap() {}
	virtual ~CLitPtrMultiMap();

	//////////////////////////////////////////////////////////////////////////
	//interface of IGameObjMultiMap
public:
	I32	Release()
	{
		delete this;
		return 0;
	}
	Iter	Begin()
	{
		return m_map.begin();
	}
	Iter	End()
	{
		return m_map.end();
	}

	bool	AddObj(const KEY& key, T* pObj);
	bool	DelObj(const KEY& key);
	bool	DelObj(Iterator& iter);
	T*		GetObjByIter(Iter i)
	{
		return i->second;
	}
	void	Clear();

	int		GetAmount() 						const
	{
		return m_map.size();
	}
	int		GetAmount(const KEY& key)			const
	{
		return m_map.count(key);
	}
	bool	IsExist(const KEY& key)
	{
		return GetAmount(key) > 0;
	}

	//////////////////////////////////////////////////////////////////////////
protected:
	T*		PopObj(Iterator& iter);

	void	ReleaseObj(T* pT);

	template <typename, DEL_WAY>
	struct Del
	{
		void DelFun(T* pT)
		{
		}
	};

	template <>
	struct Del < T, DEL_WAY_DELETE >
	{
		void DelFun(T* pT)
		{
			SAFE_DELETE_NL(pT);
		}
	};

	template <>
	struct Del < T, DEL_WAY_DELETE_MULTI >
	{
		void DelFun(T* pT)
		{
			SAFE_DELETE_MULTI_NL(pT);
		}
	};

	template <>
	struct Del < T, DEL_WAY_RELEASE >
	{
		void DelFun(T* pT)
		{
			SAFE_RELEASE_NL(pT);
		}
	};
protected:
	OBJMULTIMAP	m_map;
	Del<T, eDelWay> m_Del;
};

template	< typename T, typename KEY, DEL_WAY eDelWay>
void CLitPtrMultiMap<T, KEY, eDelWay>::ReleaseObj(T* pT)
{
	m_Del.DelFun(pT);
}

template	< typename T, typename KEY, DEL_WAY eDelWay>
CLitPtrMultiMap<T, KEY, eDelWay>::~CLitPtrMultiMap()
{
	Clear();
}

template	< typename T, typename KEY, DEL_WAY eDelWay>
void CLitPtrMultiMap<T, KEY, eDelWay>::Clear()
{
	if (m_map.size())
	{
		for (Iter i = Begin(); i != End(); ++i)
		{
			ReleaseObj(i->second);
		}
	}
	m_map.clear();
}

template	< typename T, typename KEY, DEL_WAY eDelWay>
bool CLitPtrMultiMap<T, KEY, eDelWay>::AddObj(const KEY& key, T* pObj)
{
	IF_NOT_NL(pObj)
		return false;

	m_map.insert(OBJMULTIMAP::value_type(key, pObj));
	return true;
}

template	< typename T, typename KEY, DEL_WAY eDelWay>
bool	CLitPtrMultiMap<T, KEY, eDelWay>::DelObj(Iterator& iter)
{
	if (iter.m_pObj)
	{
		T* pObj = PopObj(iter);

		IF_OK_NL(pObj)
		{
			ReleaseObj(pObj);
			return true;
		}
	}
	return false;
}

template	< typename T, typename KEY, DEL_WAY eDelWay>
T*	CLitPtrMultiMap<T, KEY, eDelWay>::PopObj(Iterator& iter)
{
	if (iter.m_pObj)
	{
		T* pObj = iter.m_pObj;
		iter.m_pObj = NULL;

		IF_OK_NL(iter.m_iter != m_map.begin())
		{
			Iter	temp = iter.m_iter;
			--temp;
			m_map.erase(temp);
			return pObj;
		}
	}
	return NULL;
}

template	< typename T, typename KEY, DEL_WAY eDelWay>
bool	CLitPtrMultiMap<T, KEY, eDelWay>::DelObj(const KEY& key)
{
	bool bRet = false;
	const KEY del_key = key;
	Iterator it = NewEnum(del_key);
	while (it.Next())
	{
		try
		{
			ReleaseObj((T*)it);
		}
		catch (...)
		{
			ASSERT(!"DelObj(key)");
		}
	}
	bRet = m_map.erase(del_key) != 0;

	return bRet;
}

NAMESPACE_END(gtool)

#endif 

