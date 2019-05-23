#ifndef	LIT_OBJ_MAP_HPP_
#define LIT_OBJ_MAP_HPP_

#pragma once

NAMESPACE_BEGIN(gtool)

template	< typename T, typename KEY >
class CLitObjMap :public std::map<KEY, T>
{
public:
	typedef	typename CLitObjMap<T, KEY>::Iterator		OBJMAPIter;		//std容器迭代器
	typedef	typename CLitObjMap<T, KEY>::const_iterator	CONSTOBJMAPIter;		//std容器迭代器
public:

//////////////////////////////////////////////////////
public:
	CLitObjMap() {}
	virtual ~CLitObjMap();
public:
	static CLitObjMap<T, KEY>*	CreateNew()
	{
		return new CLitObjMap<T, KEY>();
	}

	//////////////////////////////////////////////////////////////////////////
	//interface of IGameMap
public:
	I32	Release()
	{
		delete this;
		return 0;
	}
	OBJMAPIter	Begin()
	{
		return this->begin();
	}
	OBJMAPIter	End()
	{
		return this->end();
	}

	bool	AddObj			(const KEY& key, const T& obj);
	bool	PlusEqualObj	(const KEY& key, const T& obj);
	bool	DelObj			(const KEY& key);
	bool	DelObj			(OBJMAPIter& iter);
	T*		GetObjByIter	(OBJMAPIter i)			const
	{
		return &i->second;
	}

	T*		GetObj			(const KEY& key);
	const T*GetObj			(const KEY& key)	const;

	//获得CurKey迭代器的下一个对象
	T*		GetNextObj		(const KEY& CurKey);
	const T*GetNextObj		(const KEY& CurKey)	const;

	//获得CurKey迭代器的上一个对象
	T*		GetPrevObj		(const KEY& CurKey);
	const T*GetPrevObj		(const KEY& CurKey)	const;

	size_t	GetAmount		() 					const
	{
		return this->size();
	}
	bool	IsExist		(const KEY& key) const
	{
		return GetObj(key)!=NULL;
	}

	void	Cleans();
protected:
};

template	< class T, class KEY >
CLitObjMap<T, KEY>::~CLitObjMap()
{
	Cleans();
}

template	< class T, class KEY >
void CLitObjMap<T, KEY>::Cleans()
{
	this->clear();
}

template	< class T, class KEY >
bool CLitObjMap<T, KEY>::AddObj(const KEY& key, const T& obj)
{
	if(DelObj(key))
	{
		ASSERT(!"CLitObjMap duplicate found!");
	};
	(*this)[key] = obj;
	return true;
}

template	< class T, class KEY >
bool CLitObjMap<T, KEY>::PlusEqualObj(const KEY& key, const T& obj)
{
	T* p = GetObj(key);
	if (p)
		(*p) += obj;
	else
		return AddObj(key, obj);

	return true;
}

template	< class T, class KEY >
bool CLitObjMap<T, KEY>::DelObj(OBJMAPIter& iter)
{
	return this->erase(iter.GetKey()) != 0;
}

template	< class T, class KEY>
bool CLitObjMap<T, KEY >::DelObj(const KEY& key)
{
	return this->erase(key) != 0;
}

template	< class T, class KEY >
T* CLitObjMap<T, KEY>::GetObj(const KEY& key)
{
	OBJMAPIter i = this->find(key);
	if(i != this->end())
		return &i->second;

	return NULL;
}

template	< class T, class KEY >
const T* CLitObjMap<T, KEY>::GetObj(const KEY& key) const
{
	CONSTOBJMAPIter i;
	if((i=this->find(key)) != this->end())
		return &i->second;

	return NULL;
}


template	< class T, class KEY >
const T* CLitObjMap<T, KEY>::GetNextObj(const KEY& CurKey)	const
{
	if(this->empty())
		return NULL;

	CONSTOBJMAPIter i = this->find(CurKey);
	if(i == this->end())
		return NULL;
	if(++i == this->end())
		return NULL;

	return &i->second;
}

template	< class T, class KEY >
T* CLitObjMap<T, KEY>::GetNextObj(const KEY& CurKey)
{
	if(this->empty())
		return NULL;

	OBJMAPIter i = this->find(CurKey);
	if(i == this->end())
		return NULL;
	if(++i == this->end())
		return NULL;

	return &i->second;
}

template	< class T, class KEY >
T* CLitObjMap<T, KEY>::GetPrevObj(const KEY& CurKey)
{
	if(this->empty())
		return NULL;

	OBJMAPIter i = this->find(CurKey);
	if(i == this->end())
		return NULL;
	if(i == this->begin())
		return NULL;
	--i;

	return &i->second;
}

template	< class T, class KEY >
const T* CLitObjMap<T, KEY>::GetPrevObj(const KEY& CurKey) const
{
	if(this->empty())
		return NULL;

	CONSTOBJMAPIter i = this->find(CurKey);
	if (i == this->end())
		return NULL;
	if(i == this->begin())
		return NULL;
	--i;

	return &i->second;
}

NAMESPACE_END(gtool)

#endif 

