#ifndef MY_CLASS_MACRO_H_
#define MY_CLASS_MACRO_H_

#pragma once

#undef NO_ASSIGN
#define NO_ASSIGN(T)	protected: const T& operator=( const T& );

//禁用拷贝构造添加右值引用构造
#undef  NO_COPY_NEED_RVALUE
#define NO_COPY_NEED_RVALUE(x)\
private:\
		x (const x&);	\
		x& operator =(const x&);\
	public: \
	x ( x && rValue);

//禁用拷贝构造
#undef  NO_COPY
#define NO_COPY(x)\
private:\
		x (const x&);	\
		const x& operator=(const x&);

//禁用拷贝与转移构造
#undef  NO_COPY_NO_MOVE
#define NO_COPY_NO_MOVE(x)\
private:\
		x (const x&);	\
		const x& operator=(const x&);\
		x(x&&);

//堆区实例化接口
#undef HEAP_CREATE_NEED_CLEANS_DEFINE
#define HEAP_CREATE_NEED_CLEANS_DEFINE(T) \
protected: \
	T(); \
		virtual ~##T(); \
public:\
	static T* CreateNew()\
	{ \
		return new T(); \
	}\
	virtual int Release() { Cleans(); delete this; return 0; } \
	virtual void Cleans();

//堆区实例化接口
#undef HEAP_CREATE_NEED_CLEANS_PUBLIC
#define HEAP_CREATE_NEED_CLEANS_PUBLIC(T) \
public: \
	T(); \
		virtual ~##T(); \
	static T* CreateNew()\
	{ \
		return new T(); \
	}\
	virtual int Release() { Cleans(); delete this; return 0; } \
	virtual void Cleans();

#undef CREATE_FREE_DISABLED
#define CREATE_FREE_DISABLED(T) \
	protected:\
		T (void) {}\
virtual ~T() {} \
protected:

#undef INTERFACE_STYLE
#define INTERFACE_STYLE(T)	\
	NO_COPY(T)	\
	CREATE_FREE_DISABLED(T)

#undef PURE_VIRTUAL
#undef PURE_VIRTUAL_0
#undef PURE_VIRTUAL_
#define		PURE_VIRTUAL		=0;
#define		PURE_VIRTUAL_0		=0;
#define		PURE_VIRTUAL_(x)	=0;


///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////

#undef GEN_CLASS_TYPE_STD_MACRO
#define GEN_CLASS_TYPE_STD_MACRO(type, classname)  type(classname)

#undef GENCLASS_DEFINE_MODULE_INIT
#define GENCLASS_DEFINE_MODULE_INIT(MODULE_NUM, filename, line)\
namespace gtool {\
	template< int i>\
	struct __InitGenClass_##filename \
	{\
		enum{ realvalue = gtool::__InitGenClass_##filename< i - 1>::realvalue }; \
	};\
\
	template<>\
	struct __InitGenClass_##filename<-1> \
	{\
		enum{ realvalue = -1 }; \
	};\
	template<>\
	struct __InitGenClass_##filename<line> \
	{\
		enum{ realvalue = line }; \
	};\
	enum{MODULE_NUM=line};


#undef GENCLASS_DEFINE_MODULE_ADD
#define GENCLASS_DEFINE_MODULE_ADD(classname,filename, line, CLASS_DEFINE_MODULE_MACRO)\
	template<>\
	struct __InitGenClass_##filename<line> \
	{\
		enum{ realvalue = gtool::__InitGenClass_##filename<line - 1>::realvalue + 1 }; \
	};\
	GEN_CLASS_TYPE_STD_MACRO(CLASS_DEFINE_MODULE_MACRO, classname)\

#undef GENCLASS_DEFINE_MODULE_END
#define GENCLASS_DEFINE_MODULE_END(MODULE_NUM, filename, line) \
		enum { MODULE_NUM = gtool::__InitGenClass_##filename<line - 1>::realvalue }; }

/////////////////////////////////////////////////////////////// 【以上不可轻易更改】 ///////////////////////////////////////////////////////////////////////////////

//标准宏接口
#define GEN_CLASS_INIT_BEGIN() GENCLASS_DEFINE_MODULE_INIT(MODLULE_BEGIN, __FILE__, __LINE__)
#define GEN_CLASS_INIT_ADD(classname, CLASS_DEFINE_MODULE_MACRO) GENCLASS_DEFINE_MODULE_ADD(classname,__FILE__, __LINE__, CLASS_DEFINE_MODULE_MACRO)
#define GEN_CLASS_INIT_END() GENCLASS_DEFINE_MODULE_END(MODULE_MAX, __FILE__, __LINE__)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//内存泄露侦测
#define	HEAP_MEM_MGR(HeapObj)											\
public:\
		void*  operator new	(size_t size)	{ return HeapObj.ApplyMem(size); }	\
		void   operator delete	(void* p)	{ HeapObj.FreeMem(p); }			\
		void*  operator new[] (size_t size)	{ return HeapObj.ApplyMem(size); }	\
		void   operator delete[] (void* p)	{ HeapObj.FreeMem(p); }			\
	public:																\
		static bool	IsValidPt(void* p)		{ return HeapObj.IsValidPt(p); }\
	public:\
		static void PrintMemLeak()					{ HeapObj.Cleans();}\
	protected:															\
		static gtool::CHeapMgr	HeapObj;


#define	HEAPMEMMGR_IMPLEMENTATION(T,HeapObj)									\
	gtool::CHeapMgr	T::HeapObj(#T);		

#endif

