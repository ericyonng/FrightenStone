#ifndef APP_ROOT_H_
#define APP_ROOT_H_

#pragma once

//1.一系列必要的初始化
//app启动
//app等待
//app结束（一系列的释放）

//BusinessLogic api
typedef bool(*pfnInitBusinessLogicModule)();
typedef bool(*pfnFiniBusinessLogicModule)();
typedef const void* (*pfnGetBusinessLogicMainThread)();

class CAppRoot
{
	NO_COPY_NO_MOVE(CAppRoot);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CAppRoot);

public:
	bool Init();
	bool Fini(bool bShowInWindow = true);

	//配置
protected:
	bool ConfigSvrAddr();
	bool InitLogModule();

private:
	boost::atomic_bool				m_bInit{ false };
	static boost::atomic_bool		m_bDestroy;

};

//单例模式
typedef gtool::CSingleton<CAppRoot> AppRootInstance;
#define AppRootObj (*AppRootInstance::GetInstance())


#endif

