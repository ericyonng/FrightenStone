#ifndef LIT_MAP_HPP_
#define LIT_MAP_HPP_

#pragma once

NAMESPACE_BEGIN(gtool)

template<typename T, typename KEY, DEL_WAY eDelWay=DEL_WAY_RELEASE >
class CLitPtrMap :public std::map<KEY, T* >
{
public:
	typedef typename CLitPtrMap<T, KEY, eDelWay>::iterator LitIter;
	typedef typename CLitPtrMap<T, KEY, eDelWay>::const_iterator ConstLitIter;

public:
	static CLitPtrMap<T, KEY, eDelWay> * CreateNew(bool bOwner=true)
	{ 
		return new CLitPtrMap<T, KEY, eDelWay>(bOwner);
	}
 	void  Cleans();
	I32 Release()
	{
		Cleans();
		delete this;
		return 0;
	}

	T*		GetObj(const KEY& key);
	T*		GetObj(const KEY& key)				const;
	T*		GetObjByIter(LitIter i)				const
	{
		if (i != this->end()) return i->second;
		return NULL;
	}
	T*		PopObj(const KEY& key);
	T*		PopObj(ConstLitIter& iter);
	typename CLitPtrMap<T, KEY, eDelWay>::iterator	PopObj(ConstLitIter& iter, bool bRetIter);
	bool	IsExist(const KEY& key) { return GetObj(key) != NULL; }

	bool DelObj(const KEY& rKey)
	{
		bool bRet = false;
		if (m_bOwner)
		{
			auto IterMap = this->find(rKey);
			if (IterMap == this->end())
			{
				return true;
			}

			T* pT = IterMap->second;
			IterMap->second = NULL;
			ReleaseObj(pT);

			bRet = (this->erase(rKey) != 0);
		}
		else
		{
			DelCache(rKey);
			bRet = (this->erase(rKey) != 0);
		}

		return bRet;
	}

	typename CLitPtrMap<T, KEY, eDelWay>::iterator DelObj(ConstLitIter& iter);

	bool	AddObj(const KEY& key, T* pObj);
protected:
	CLitPtrMap():m_bOwner(false), m_bEnableCache(true), m_pCache(NULL) {}
	CLitPtrMap(bool bOwner) :m_bOwner(bOwner), m_bEnableCache(true), m_pCache(NULL){}
	virtual ~CLitPtrMap()
	{
		EnableCache(false);
		Cleans();
	}

	void ReleaseObj(T* pT);
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
			try
			{
				ASSERT(pT);
				if (pT)
				{
					SAFE_DELETE_NL(pT);
				}
			}
			catch (...)
			{
				ASSERT(!"Del<T, DEL_WAY_DELETE> LitPtrMapCrash");
			}
		}
	};

	template <>
	struct Del<T, DEL_WAY_DELETE_MULTI>
	{
		void DelFun(T* pT)
		{
			try
			{
				ASSERT(pT);
				if (pT)
				{
					SAFE_DELETE_MULTI_NL(pT);
				}
			}
			catch (...)
			{
				ASSERT(!"Del<T, DEL_WAY_DELETE_MULTI> LitPtrMapCrash");
			}
		}
	};

	template <>
	struct Del<T, DEL_WAY_RELEASE>
	{
		void DelFun(T* pT)
		{
			try
			{
				ASSERT(pT);
				if (pT)
				{
					pT->Release();
					pT = NULL;
				}
			}
			catch (...)
			{
				ASSERT(!"Del<T, DEL_WAY_RELEASE> LitPtrMapCrash");
			}
		}
	};

public:
	void	EnableCache(bool bVal) { m_bEnableCache = bVal; }
protected:
	void	AddCache(T* pObj, const KEY& key) const;
	void	DelCache(const KEY& key);
	void	DelCache(T* pObj);
	T*		QueryCache(const KEY& key) const;

private:
	CLitPtrMap & operator=(const CLitPtrMap&) = delete;
	CLitPtrMap(const CLitPtrMap&) = delete;

private:
	bool m_bOwner = true;

	Del<T, eDelWay> m_Del;
	bool			m_bEnableCache;
	mutable T*		m_pCache;
	mutable KEY		m_cache_key;
	
};

template<typename T, typename KEY, DEL_WAY eDelWay>
void CLitPtrMap<T, KEY, eDelWay>::Cleans()
{
	const bool bEnableCache = m_bEnableCache;
	if (bEnableCache)
	{
		EnableCache(false);
		m_pCache = NULL;
	}

	if (m_bOwner && this->size())
	{
		for (auto i = this->begin(); i != this->end(); ++i)
		{
			T* p = i->second;
			i->second = NULL;
			ReleaseObj(p);
		}
	}

	this->clear();

	EnableCache(bEnableCache);
}

template< typename T, typename KEY, DEL_WAY eDelWay>
void CLitPtrMap<T, KEY, eDelWay>::ReleaseObj(T* pT)
{
	DelCache(pT);
	if (m_bOwner)
	{
		this->m_Del.DelFun(pT);
	}
}

template< typename T, typename KEY, DEL_WAY eDelWay >
void CLitPtrMap<T, KEY, eDelWay>::AddCache(T* pObj, const KEY& key) const
{
	if (!m_bEnableCache)
		return;
	m_pCache = pObj;
	m_cache_key = key;
}

template<typename T, typename KEY, DEL_WAY eDelWay >
void CLitPtrMap<T, KEY, eDelWay>::DelCache(const KEY& key)
{
	if (!m_bEnableCache)
		return;

	if (key == m_cache_key)
		m_pCache = NULL;
}

template< typename T, typename KEY, DEL_WAY eDelWay >
void CLitPtrMap<T, KEY, eDelWay>::DelCache(T* pObj)
{
	if (!m_bEnableCache)
		return;

	if (m_pCache == pObj)
		m_pCache = NULL;
}

template< typename T, typename KEY, DEL_WAY eDelWay >
T* CLitPtrMap<T, KEY, eDelWay>::QueryCache(const KEY& key) const
{
	if (!m_bEnableCache)
		return NULL;

	if (m_pCache && (m_cache_key == key))
		return m_pCache;
	return NULL;
}


template< typename T, typename KEY, DEL_WAY eDelWay  >
T*  CLitPtrMap<T, KEY, eDelWay>::GetObj(const KEY& key)
{
	T* pRet = QueryCache(key);
	if (pRet)
		return pRet;

	CLitPtrMap<T, KEY, eDelWay>::iterator i;
	if ((i = this->find(key)) != this->end())
		pRet = i->second;

	if (pRet)
		AddCache(pRet, key);

	return pRet;
}

template< typename T, typename KEY, DEL_WAY eDelWay>
T* CLitPtrMap<T, KEY, eDelWay>::GetObj(const KEY& key) const
{
	T* pRet = QueryCache(key);
	if (pRet)
		return pRet;

	CLitPtrMap<T, KEY, eDelWay>::iterator i;
	if ((i = this->find(key)) != this->end())
		pRet = i->second;

	if (pRet)
		AddCache(pRet, key);

	return pRet;
}


template< typename T, typename KEY, DEL_WAY eDelWay >
T*	CLitPtrMap<T, KEY, eDelWay>::PopObj(const KEY& key)
{
	CLitPtrMap<T, KEY, eDelWay>::iterator i;
	if ((i = this->find(key)) != this->end())
	{
		T* p = i->second;
		DelCache(key);
		this->erase(key);
		return p;
	}
	return NULL;
}

template< typename T, typename KEY, DEL_WAY eDelWay >
T*	CLitPtrMap<T, KEY, eDelWay>::PopObj(ConstLitIter& iter)
{
	IF_OK_NL(iter != this->end())
	{
		if (iter->second)
		{
			T* pObj = iter->second;
			DelCache(pObj);
			this->erase(iter);//或者 this->erase(iter); map中该迭代器失效
			return pObj;
		}
	}

	return NULL;
}

template< typename T, typename KEY, DEL_WAY eDelWay >
typename CLitPtrMap<T, KEY, eDelWay>::iterator	CLitPtrMap<T, KEY, eDelWay>::PopObj(ConstLitIter& iter, bool bRetIter)
{
	UNUSED_ARG(bRetIter);

	IF_OK_NL(iter != this->end())
	{
		auto pObj = iter->second;
		DelCache(pObj);
		auto IterNext = this->erase(iter);//或者 this->erase(iter); map中该迭代器失效

		return IterNext;
	}

	return this->end();
}

template< typename T, typename KEY, DEL_WAY eDelWay >
typename CLitPtrMap<T, KEY, eDelWay>::iterator CLitPtrMap<T, KEY, eDelWay>::DelObj(ConstLitIter& iter)
{
	CLitPtrMap<T, KEY, eDelWay>::iterator IterTmp = this->end();
	if (iter->second)
	{
		IterTmp = this->find(iter->first);
		++IterTmp;
		T* pObj = PopObj(iter);
		IF_OK_NL(pObj)
		{
			ReleaseObj(pObj);
			return IterTmp;
		}
	}
	return IterTmp;
}


template< typename T, typename KEY, DEL_WAY eDelWay >
bool CLitPtrMap<T, KEY, eDelWay>::AddObj(const KEY& key, T* pNewObj)
{
	if (pNewObj == NULL)
		return false;

	auto itOld = this->find(key);
	if (itOld != this->end()){

		//处理重复key
		T* pOld = GetObjByIter(itOld);
		if (pOld == pNewObj){
			return true;
		}else{
			ReleaseObj(itOld->second);
			itOld->second = pNewObj;
		}

	}else{
		(*this)[key] = pNewObj;
	}

	return true;
}

NAMESPACE_END(gtool)

#endif
