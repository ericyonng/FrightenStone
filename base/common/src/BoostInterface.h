#ifndef BOOST_INTERFACE_H_
#define BOOST_INTERFACE_H_

#pragma once

#include <boost/format.hpp>					//格式字符串解析
#include <boost/type_traits/is_class.hpp>	//bosst判断是否类（无法区分结构体和类）的方法
#include <boost/atomic.hpp>					//boost原子操作，提升多线程性能 需要链接静态库或者动态库

// #define BOOST_LIB_NAME "boost_atomic"
// 
// #include <boost/atomic/detail/atomic_template.hpp>	//原子操作模板
//#include <boost/config/auto_link.hpp>		//boost自动链接库
//#  pragma comment(lib, BOOST_LIB_PREFIX BOOST_STRINGIZE(atomic) "-" BOOST_LIB_TOOLSET BOOST_LIB_THREAD_OPT BOOST_LIB_RT_OPT "-" BOOST_LIB_VERSION ".lib")


//原子库
#ifndef BASECODE_INC_BOOST_ATOMIC_HPP_LIB
#define BASECODE_INC_BOOST_ATOMIC_HPP_LIB

	#ifndef _DEBUG
		#pragma comment(lib, "libboost_atomic-vc141-mt-x64-1_66.lib")
	#else
		#pragma comment(lib, "libboost_atomic-vc141-mt-gd-x64-1_66.lib")
	#endif

#endif

#endif

