#ifndef SMART_PTR_HPP_
#define SMART_PTR_HPP_

#pragma once
//#include <BaseCode/MyDataType.h>
//#include<BaseCode/MyBaseClassTemplete.hpp>
//#include <BaseCode/MyBaseMacro.h>

namespace gtool
{
	template<typename T, typename T2 = T>
	class CSmartPtr
	{
	public:
		CSmartPtr() : m_ptr(NULL) {}
		CSmartPtr(T* ptr) : m_ptr(ptr) {}		// may be null
		CSmartPtr(CSmartPtr&& rValue)	//使用std::move方法将临时对象转换成右值引用
		:m_ptr(NULL)
		{
			if (rValue.m_ptr != m_ptr)
			{
				m_ptr = rValue.m_ptr;
			}			
			rValue.m_ptr = NULL;
		}
		virtual ~CSmartPtr() { Release(); }
		T*	pop() { T* p = m_ptr; m_ptr = NULL; return p; }


	private:
		CSmartPtr & operator=(const CSmartPtr& ptr)=delete;
		CSmartPtr(const CSmartPtr&)=delete;

	public: // create new
		T * New(CLS_ID id) { Release(); m_ptr = T2::CreateNew(id); ASSERT(m_ptr); return m_ptr; }
		int Release() { if (m_ptr) m_ptr->Release(); m_ptr = NULL; return 0; }

	public:
		CSmartPtr & operator=(T* ptr) 
		{
			if (m_ptr && m_ptr != ptr) 
				Release(); 

			m_ptr = ptr; 
			return *this; 
		}

		CSmartPtr& operator=(CSmartPtr&& rValue) 
		{
			if (m_ptr && m_ptr != rValue.m_ptr)
				Release();

			m_ptr = rValue.m_ptr;
			rValue.m_ptr = NULL; 
			return *this; 
		}

		operator gtool::CSmartPtr<T> && () { return std::move(*this); }
		operator T*() { return m_ptr; }
		operator const T*()	 	 const { return m_ptr; }
		operator bool() { return m_ptr != NULL; }
		operator const bool() const { return m_ptr != NULL; }
 		operator void *() { return (void *)m_ptr; }
 		operator const void *() { return (const void *)m_ptr; }
// 		operator int() { return m_ptr != NULL; }
// 		operator const int() const { return m_ptr != NULL; }

		POINT_NOCOPY_PRVRTL_RELEASE<T>*	operator->(){ASSERT(m_ptr);	return static_cast<POINT_NOCOPY_PRVRTL_RELEASE<T>*>(m_ptr);	}
		
		
		const POINT_NOCOPY_PRVRTL_RELEASE<T>* operator->() const { ASSERT(m_ptr); return static_cast<POINT_NOCOPY_PRVRTL_RELEASE<T>*>(m_ptr); }

		T& operator [](int nIndex)	//有越界风险 注意判断越界
		{
			T* ptr = m_ptr;
			if (m_ptr)
			{
				ptr = m_ptr + nIndex;
			}
			ASSERT(ptr);

			return *ptr;
		}
		const T& operator [](int nIndex)	 	 const //有越界风险 注意判断越界
		{
			T* ptr = m_ptr;
			if (m_ptr)
			{
				ptr = m_ptr + nIndex;
			}
			ASSERT(ptr);
			return *ptr;
		}

	protected:
		T * m_ptr=NULL;
	};
};

#endif