#ifndef BASE_INCLUDE_DEFINE_H_
#define BASE_INCLUDE_DEFINE_H_

#pragma once
#include <BaseCode/WarningControl.h>

//以下需要放置stdafx.h
#include <BaseCode/myclassmacro.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <windows.h>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <deque>
#include <stdlib.h>
#include <math.h>
#include <algorithm>						//std的算法
#include <memory>							//一些关于内存的算法
#include <limits>							//数据的范围
#include <thread>							//线程
#include <atomic>
//#include <BaseCode/BoostInterface.h>		//boost接口
#include <BaseCode/OpenSSLInterface.h>		//openssl接口
#include<chrono>


//基础库------------------------------------------------------------------end
#include <BaseCode/UnionPtrMacro.h>
#include <BaseCode/MyBaseMacro.h>			//基本宏
//#include <BaseCode/PathMacro.h>				//路径名称
#include <BaseCode/MyDataType.h>			//数据类型
#include <BaseCode/InlineFun.hpp>			//内联函数
#include <BaseCode/ConstantValueDefine.h>	//常量定义
#include<BaseCode/LEN_MACRO.h>				//长度宏定义
#include <BaseCode/enumtype.h>				//枚举定义
//#include <BaseCode/MemPoolObj.h>			//内存池创建宏
#include<BaseCode/MyBaseClassTemplete.hpp>	//基本构成类模板
#include <BaseCode/MacroDefine.h>			//通用宏
#include <BaseCode/CheckNoLog.h>			//无log的断言
#include <BaseCode/Timer.h>					//时钟
#include <BaseCode/Win/Cpu.h>				//cpuinfo
#include <BaseCode/BufferAdapter.hpp>		//缓存适配器

#include <BaseCode/Win/WinDependInl.h>		//windows api依赖需要使用 NEED_WINDOWS_API 开关宏

//轻对象------------------------------------------------------------------end


#include <BaseCode/SmartPtrC.hpp>		//智能指针支持包括类在内的其他数据类型
#include <BaseCode/SmartPtr.hpp>		//只能指针只支持类（且必须带 release）
#include <BaseCode/LitUnmanagedPtr.h>	//无需堆管理指针
#include <BaseCode/TPtrList.hpp>		//list容器
#include <BaseCode/LitPtrMap.hpp>		//map容器
#include <BaseCode/LitObjSet.hpp>		//set
#include <BaseCode/ComAlgorithm.h>		//通用算法
#include <BaseCode/TaskBase.h>			//任务基类
#include <BaseCode/Win/WinLocker.h>			//windows下的锁
#include <BaseCode/Singleton.hpp>		//单例
#include <BaseCode/FileDirSys.h>		//文件目录读写
#include <BaseCode/FmtIOString.h>		//字符串应用
#include <BaseCode/FmtAnalysis.h>		//boost格式字符串解析
#include <BaseCode/File2.h>				//安全的文件类
#include <BaseCode/gtool.hpp>			//通用工具
#include <BaseCode/Win/WinGtool.h>	//windows函数



#endif

