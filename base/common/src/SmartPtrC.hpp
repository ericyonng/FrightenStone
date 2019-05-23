#ifndef SMART_PTR_C_HPP_
#define SMART_PTR_C_HPP_

#pragma once
// #include <BaseCode/MyDataType.h>
// #include <boost/type_traits/is_class.hpp>
// #include <BaseCode/MyBaseMacro.h>

//对struct class is_class 没法区分
namespace gtool
{
	template<typename T, typename T2 = T, DEL_WAY eDel_Type=DEL_WAY_DELETE>
	class CSmartPtrC
	{
	public:
		CSmartPtrC() : m_ptr(NULL) {}
		CSmartPtrC(T* ptr) : m_ptr(ptr) {}
		CSmartPtrC(CSmartPtrC&& rValue)	//使用std::move方法将临时对象转换成右值引用
			:m_ptr(NULL)
		{
			if (m_ptr != rValue.m_ptr)
			{
				m_ptr = rValue.m_ptr;
			}			
			rValue.m_ptr = NULL;
		}
		virtual ~CSmartPtrC() { Release(); }
		T*	pop()
		{ 
			T* p = m_ptr; 
			m_ptr = NULL; 
			return p; 
		}		

	private: 
		CSmartPtrC & operator=(const CSmartPtrC& ptr)=delete;
		CSmartPtrC(const CSmartPtrC&)=delete;

	public: // create new
		T * New(CLS_ID id) { Release(); m_ptr = new T2[id]; ASSERT(m_ptr); return m_ptr; }
		void Release() { m_DelObj.Release(m_ptr); m_ptr = NULL; }

	public:
		CSmartPtrC & operator=(T* ptr)
		{
			if (m_ptr && m_ptr != ptr)
				Release();

			m_ptr = ptr;
			return *this;
		}

		CSmartPtrC& operator=(CSmartPtrC&& rValue) 
		{
			if (m_ptr && m_ptr != rValue.m_ptr) 
				Release();

			m_ptr = rValue.m_ptr; 
			rValue.m_ptr = NULL; 
			return *this; 
		}

		operator gtool::CSmartPtrC<T> && () { return std::move(*this); }
		operator T*() { return m_ptr; }
		operator const T*()	 	 const { return m_ptr; }
		operator void *() { return (void *)m_ptr; }
		operator const void *() { return (const void *)m_ptr; }
		operator bool() { return m_ptr != NULL; }
		operator const bool() const { return m_ptr != NULL; }
// 		operator int() { return m_ptr != NULL; }
// 		operator const int() const { return m_ptr != NULL; }

		T*	operator->() { ASSERT(m_ptr); return static_cast<T*>(m_ptr); }

		const T*	operator->() const { ASSERT(m_ptr); return static_cast<T*>(m_ptr); }	

		T& operator [](int nIndex) //有越界风险 注意判断越界
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
		template<typename, DEL_WAY >
			struct DelObj 
			{
				void Release()
				{
				}
			};

		template<>
		struct DelObj<T, DEL_WAY_DELETE >
		{
			int Release(T* & pT)
			{
				SAFE_DELETE_NL(pT);

				return 0;
			}
		};

		template<>
		struct DelObj<T, DEL_WAY_DELETE_MULTI >
		{
			int Release(T* & pT)
			{
				SAFE_DELETE_MULTI_NL(pT);

				return 0;
			}
		};

		template<>
		struct DelObj<T, DEL_WAY_RELEASE >
		{
			int Release(T* & pT)
			{
				SAFE_RELEASE_NL(pT);

				return 0;
			}
		};

		template<>
		struct DelObj<T, DEL_WAY_NOT_OWNER >
		{
			int Release(T* & pT)
			{
				pT = NULL;

				return 0;
			}
		};

	protected:
		T * m_ptr=NULL;
		DelObj<T, eDel_Type> m_DelObj;
	};
	template<typename T, typename T2>
	inline void		safe_release(CSmartPtrC<T, T2, boost::is_class<T>::value>& pT) { pT.Release(); }
};

#endif