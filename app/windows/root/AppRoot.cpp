#include "stdafx.h"
#include <resource.h>
#include <AppRoot.h>
#include <DlgRoot.h>
#include <BaseCode/Win/IniFileIO.h>
//#include <BaseCode/ThreadPool.h>

//#include <BaseCode/MemoryPool.inl>
boost::atomic_bool CAppRoot::m_bDestroy{ false };
CAppRoot::CAppRoot()
{
}

CAppRoot::~CAppRoot()
{
}

void CAppRoot::Cleans()
{
}

bool CAppRoot::Init()
{	
	SEH_TRY
	{
		//log初始化
		//CHECKF_NL(LogInitHelper<LOG_NUM_MAX>::InitLog());	//log初始化
		IF_NOT_NL(InitLogModule())
		{
			TestDisPlay("Log Module 初始化失败.");
			return false;
		}
		TestDisPlay("Log Module 初始化成功.");
		LOGSYS_FMT_C4096("Log Module 初始化成功.");

		IF_NOT(gtool::MemPoolInitModule())				//内存池	
		{
			TestDisPlay("MemPool Module 初始化失败.");
			return false;
		}
		TestDisPlay("MemPool Module 初始化成功.");
		LOGSYS_FMT_C4096("MemPool Module 初始化成功.");

		IF_NOT_NL(THREAD_TRANSFER::InitThreadPostModule())
		{
			TestDisPlay("ThreadPost Module 初始化失败.");
			return false;
		}
		TestDisPlay("ThreadPost Module 初始化成功.");
		LOGSYS_FMT_C4096("ThreadPost Module 初始化成功.");		

		//服务器地址配置
		IF_NOT(ConfigSvrAddr())
		{
			TestDisPlay("ConfigSvrAddr 失败.");
			return false;
		}
		TestDisPlay("ConfigSvrAddr 成功.");
		LOGSYS_FMT_C4096("ConfigSvrAddr 成功.");

		IF_NOT(gtool::NetWorkInitModule())				//iocp网络模块初始化
		{
			TestDisPlay("NetWork Module 初始化失败.");
			return false;
		}
		TestDisPlay("NetWork Module 初始化成功.");
		LOGSYS_FMT_C4096("NetWork Module 初始化成功.");

		IF_NOT(InitBusinessLogicModule())
		{
			TestDisPlay("BusinessLogic Module 初始化失败.");
			return false;
		}
		TestDisPlay("BusinessLogic Module 初始化成功.");
		LOGSYS_FMT_C4096("BusinessLogic Module 初始化成功.");

		//设置业务逻辑线程实例
//		ASSERTEX(THREAD_TRANSFER::SetDisplayFun(GetBusinessLogicMainThread(), POST_OBJ_TYPE_MAIN_THREAD, &DlgDisplay));	//界面显示
//		ASSERTEX(THREAD_TRANSFER::SetDisplayFun(gtool::NetWorkModulGetSenderThread(), POST_OBJ_TYPE_NETWORK_SENDER, &DlgDisplay)); //界面显示
		gtool::NetWorkModulSetGetBusinessLogicMainThread(GetBusinessLogicMainThread());	//网络模块接入业务逻辑
		BusinessLogicSetNetworkSenderThread(gtool::NetWorkModulGetSenderThread());	//业务逻辑模块接入网络模块

		m_bInit = true;
		m_bDestroy = false;
	}
	SEH_FULLINFO_CATCH("bool CAppRoot::Init()")
	{
		LOGCRASH(GET_SEHCATCH_BUFFER_STRING());
		m_bInit = false;
		return false;
	}
	SEH_CATCH_END

	return true;
}

//一系列的释放 与初始化相反，先初始化的后finish
bool CAppRoot::Fini(bool bShowInWindow /*= true*/)
{
	SEH_TRY
	{
		if (m_bDestroy)
			return true;

		m_bDestroy = true;
		m_bInit = false;
		ASSERTEX(FiniBusinessLogicModule());					//业务模块释放
		if(bShowInWindow)
			TestDisPlay("BusinessLogic Module Fini.");

		LOGSYS_FMT_C4096("BusinessLogic Module Fini.");
	}
	SEH_FULLINFO_CATCH("bool CAppRoot::Fini() FiniBusinessLogicModule")
	{
		LOGCRASH(GET_SEHCATCH_BUFFER_STRING());
	}
	SEH_CATCH_END

	SEH_TRY
	{
		m_bInit = false;
		gtool::NetWorkFinishModule();					//网络模块释放
		if (bShowInWindow)
			TestDisPlay("NetWork Module Fini.");
		LOGSYS_FMT_C4096("NetWork Module Fini.");
	}
	SEH_FULLINFO_CATCH("bool CAppRoot::Fini() NetWorkFinishModule")
	{
		LOGCRASH(GET_SEHCATCH_BUFFER_STRING());
	}
	SEH_CATCH_END

	return true;
}

bool CAppRoot::ConfigSvrAddr()
{
	CHECKF_NL(gtool::IsLogModuleInit());

	//配置服务器地址
	gtool::NetWorkModulSetIp("");
	BUFFER512 szConfigPath = { 0 };

	//获取配置文件绝对路径
	CHECKF(gtool::GetProcPath(true, _getpid(), szConfigPath, sizeof(szConfigPath)));
	auto strConfig = gtool::CFileDirSys::GetFilePathInWholePath(szConfigPath);
	strConfig += "SvrConfig.ini";

	//配置文件初始化
	CIniFileIO IniConfig;
	CHECKF(IniConfig.SetFilePath(strConfig.c_str()));

	//读取配置
	BUFFER128 szip = { 0 };
	char *pip = szip;
	U32 nPort = 0;
	IniConfig.ReadString("AddrInfo", "ip", "", pip, sizeof(szip));
	nPort = IniConfig.ReadInt("AddrInfo", "port", 0);

	//设置ip
	if (strlen(pip) > 0)
		gtool::NetWorkModulSetIp(pip);

	//设置端口
	if ((nPort > 0))
		gtool::NetWorkModulSetSvrPort(nPort);

	return true;
}

bool CAppRoot::InitLogModule()
{
	CHECKF_NL(gtool::InitLogModule());
	CHECKF_NL(LogInitHelper<LOG_NUM_MAX>::InitLog());

	return true;
}

