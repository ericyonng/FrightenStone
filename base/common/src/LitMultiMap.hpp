#ifndef	LIT_MULTIMAP_HPP_
#define LIT_MULTIMAP_HPP_

#pragma once


NAMESPACE_BEGIN(gtool)


template	< class T, class KEY>
class CLitMultiMap
{
public:
	typedef	std::multimap<KEY, T>			OBJMULTIMAP;
	typedef	typename OBJMULTIMAP::iterator			Iter;		//std容器迭代器
	typedef	typename OBJMULTIMAP::const_iterator		ConstIter;	//std容器迭代器
	typedef	typename OBJMULTIMAP::reverse_iterator	RIter;		//std容器迭代器
	typedef	typename OBJMULTIMAP::const_reverse_iterator	RConstIter;	//std容器迭代器

public:
	typedef std::pair<Iter, Iter> RangePair;
	//////////////////////////////////////////////////////
	//迭代器封装
	class	Iterator
	{
	private: // create by parent class
		Iterator(OBJMULTIMAP* pSet) : m_pSet(pSet),m_range(pSet->begin(),pSet->end()),m_iter(pSet->begin()),m_pObj(0) {}
		Iterator(OBJMULTIMAP* pSet, const KEY& key) : m_pSet(pSet),m_range(pSet->equal_range(key)),m_iter(m_range.first),m_pObj(0) {}
		friend class CLitMultiMap<T, KEY>;

	public: // application
		bool	Next()
		{
			if(m_iter==m_range.second) return m_pObj=NULL,false;
			m_key=m_iter->first;
			m_pObj=&m_iter->second;
			ASSERT(m_pObj);
			++m_iter;
			return true;
		}
		void	Reset()
		{
			m_iter=m_range.first;
			m_pObj=NULL;
			m_range.first=m_pSet->end();
			m_range.second=m_range.first;
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
		size_t	GetAmount() const
		{
			return std::distance(m_range.first, m_range.second);
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
		template <>	struct RET<KEY, true>
		{
			KEY NULLT() const
			{
				return KEY();
			}
		};
		template <> struct RET<KEY, false>
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
	Iterator	NewEnum		()
	{
		return Iterator(&m_map);
	}
	Iterator	NewEnum		(const KEY& key)
	{
		return Iterator(&m_map, key);
	}

//////////////////////////////////////////////////////
protected:
	CLitMultiMap() {}
	virtual ~CLitMultiMap();
public:
	static CLitMultiMap<T, KEY>*	CreateNew()
	{
		return new CLitMultiMap<T, KEY>();
	}

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
	RIter	RBegin()
	{
		return m_map.rbegin();
	}
	RIter	REnd()
	{
		return m_map.rend();
	}

	bool	AddObj(const KEY& key, const T& obj);
	bool	DelObj(const KEY& key);
	bool	DelObj(Iterator& iter);
	T*		GetObjByIter(Iter i)
	{
		return &i->second;
	}
	void	Clear();

	size_t	GetAmount() 						const
	{
		return m_map.size();
	}
	size_t	GetAmount(const KEY& key)			const
	{
		return m_map.count(key);
	}
	bool	IsExist(const KEY& key)
	{
		return GetAmount(key) > 0;
	}

protected:
	OBJMULTIMAP		m_map;
};

template	< class T, class KEY >
CLitMultiMap<T, KEY>::~CLitMultiMap()
{
	Clear();
}

template	< class T, class KEY >
void CLitMultiMap<T, KEY>::Clear()
{
	m_map.clear();
}

template	< class T, class KEY >
bool CLitMultiMap<T, KEY>::AddObj(const KEY& key, const T& obj)
{
	m_map.insert(OBJMULTIMAP::value_type(key, obj));
	return true;
}

template	< class T, class KEY >
bool	CLitMultiMap<T, KEY>::DelObj(Iterator& iter)
{
	bool bRet = false;

	if(iter.m_pObj)
	{
		iter.m_pObj	= NULL;

		IF_OK_NL(iter.m_iter != m_map.begin())
		{
			// #ifdef	USE_HASH_MAP
			// 				m_map.erase(pObj->GetID());
			// #else // USE_HASH_MAP
			Iter	temp = iter.m_iter;
			--temp;
			m_map.erase(temp);
			//#endif // USE_HASH_MAP
			bRet = true;
		}
	}
	return bRet;
}

template	< class T, class KEY >
bool	CLitMultiMap<T, KEY>::DelObj(const KEY& key)
{
	bool bRet = false;
	bRet = m_map.erase(key)!=0;

	return bRet;
}

NAMESPACE_END(gtool)

#endif // _TQ_TMULTIMAP_H_

