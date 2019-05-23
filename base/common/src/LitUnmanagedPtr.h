#ifndef LIT_UNMANAGED_PTR_H_
#define LIT_UNMANAGED_PTR_H_


NAMESPACE_BEGIN(gtool)

template <typename T>
class CLitUnmanagedPtr
{
public:
	typedef T element_type;
	
	CLitUnmanagedPtr(T* P = NULL)
		: m_ptr(P) { }
	~CLitUnmanagedPtr() { }
	
	CLitUnmanagedPtr<T>& operator=(T* P)
	{
		m_ptr = P;
		
		return (*this);
	}
	
	const T& operator*() const
	{
		ASSERT(m_ptr != NULL);
		return *m_ptr;
	}

	T& operator*()
	{
		ASSERT(m_ptr != NULL);
		return *m_ptr;
	}
	
	T* operator->() const
	{
		ASSERT(m_ptr != NULL);
		return m_ptr;
	}

	T* operator->()
	{
		ASSERT(m_ptr != NULL);
		return m_ptr;
	}
	
	operator T*() const
	{
		return m_ptr;
	}
	
	operator T*()
	{
		return m_ptr;
	}

	operator void*() const
	{
		return m_ptr;
	}

	operator void*()
	{
		return m_ptr;
	}

	operator bool() const
	{
		return m_ptr != NULL;
	}

	operator bool()
	{
		return m_ptr != NULL;
	}

private:
	T* m_ptr;
};

NAMESPACE_END(gtool)


#endif
