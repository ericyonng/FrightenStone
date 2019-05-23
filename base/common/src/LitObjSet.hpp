#ifndef	LIT_OBJ_SET_HPP_
#define LIT_OBJ_SET_HPP_

#pragma once

NAMESPACE_BEGIN(gtool)


template	< typename T>
class CLitObjSet :public std::set<T>
{
public:
	typedef	CLitObjSet<T>			OBJSET;
	typedef	typename OBJSET::iterator			Iter;		//std容器迭代器
	typedef	typename OBJSET::const_iterator		ConstIter;	//std容器迭代器

//////////////////////////////////////////////////////
public:
	CLitObjSet() {}
	virtual ~CLitObjSet();
public:
	static CLitObjSet<T>*	CreateNew()
	{
		return new CLitObjSet<T>();
	}

	//////////////////////////////////////////////////////////////////////////
	//interface of IGameMap
public:
	I32	Release()
	{
		delete this;
		return 0;
	}

	bool	AddObj(const T& val);
	bool	DelObj(const T& val);
	Iter	DelObj(Iter & itr)
	{
		return this->erase(itr);
	};

	bool	IsEmpty()							const
	{
		return this->empty();
	}
	int		GetAmount() 						const
	{
		return (int)(this->size());
	}
	bool	IsExist(const T& val)				const
	{
		return this->find(val) != this->end();
	}
	void	Clear();

};

template	< class T>
CLitObjSet<T>::~CLitObjSet()
{
	Clear();
}

template	< class T>
void CLitObjSet<T>::Clear()
{
	this->clear();
}

template	< class T>
bool CLitObjSet<T>::AddObj(const T& val)
{
	if(DelObj(val))
	{
		ASSERT(!"CLitObjSet AddObj error .");
	};

	IF_NOT_NL(this->insert(val).second)
		return false;

	return true;
}

template	< class T>
bool CLitObjSet<T>::DelObj(const T& val)
{
	return this->erase(val)!=0;
}

NAMESPACE_END(gtool)

#pragma warning(default:4541)

#endif 

