#ifndef MY_BASE_CLASS_TEMPLETE_HPP_
#define MY_BASE_CLASS_TEMPLETE_HPP_

#pragma once
//#include <BaseCode/myclassmacro.h>

template<typename T>
class POINT_NOCOPY_PRVRTL_RELEASE : public T
{
public:
	virtual int	Release() = 0;		//

private:
	NO_COPY(POINT_NOCOPY_PRVRTL_RELEASE<T>);
	virtual ~POINT_NOCOPY_PRVRTL_RELEASE<T>();
};

template<typename T>
class POINT_RELEASE : public T
{
public:
	virtual int	Release() = 0;		//
};

#endif