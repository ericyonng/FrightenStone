/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : Application.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/15
 * @brief :
 */
#include "stdafx.h"
#include "Service/Application.h"
#include "Service/ServiceFactory.h"
#include "Service/ServiceType.h"
#include "Service/Common/LogicCommon.h"

FS_Application *g_ThisApp = NULL;
FS_Application::FS_Application()
    :_config(NULL)
{
    g_ThisApp = this;
    // 基类构造时候不会动态调到派生类的虚函数
    // _config = _NewConfigMgr();
}

FS_Application::~FS_Application()
{
    fs::STLUtil::DelVectorContainer(_logics);
    Fs_SafeFree(_config);
}

void FS_Application::Run()
{
    // 公共对象初始化
    if (!_CommonInit())
    {
        printf("commonInit fail\n");
        return;
    }

    _config = _NewConfigMgr();
    auto st = Init();
    if(st != StatusDefs::Success)
    {
        printf("AppName[%s] init fail st[%d]", _appName.c_str(), st);
    }
    else
    {
        st = Start();
        if(st == StatusDefs::Success)
        {
            getchar();
        }
        else
        {
            g_Log->e<FS_Application>(_LOGFMT_("appname[%s] start  fail st[%d]"), _appName.c_str(), st);
        }
    }

    Close();
}

fs::IFS_ConfigMgr * FS_Application::_NewConfigMgr()
{
    return new fs::IFS_ConfigMgr;
}

fs::IFS_ConfigMgr * FS_Application::_PreparConfigFile() const
{
    fs::FS_String iniName;
    iniName.AppendFormat("%s/%s/%s.ini", g_RootPath.c_str(), g_AllCfgsDir.c_str(), _appName.c_str());

    auto ret = _config->Init(iniName.c_str());
    if(ret != StatusDefs::Success)
    {
        g_Log->e<FS_Application>(_LOGFMT_("config Init fail ret[%d]"), ret);
    }

    return _config;

/*    _programName = fs::SystemUtil::GetApplicationName();*/
}

Int32 FS_Application::_ReadCustomCfgs()
{
    // 服务名
    _serviceName = ServiceType::GetStr(_totalCfgs->_dispatcherCfgs._logicServiceId);
    return StatusDefs::Success;
}

Int32 FS_Application::_OnInitFinish()
{
    _logics.resize(_totalCfgs->_commonCfgs._dispatcherQuantity);
    Int32 quantity = static_cast<Int32>(_logics.size());
    const Int32 appServiceId = _totalCfgs->_dispatcherCfgs._logicServiceId;
    for(Int32 i = 0; i < quantity; ++i)
    {
        auto newSvc = _NewSvc(appServiceId);
        if(!newSvc)
        {
            g_Log->e<FS_Application>(_LOGFMT_("create service fail..."));
            return StatusDefs::Failed;
        }

        _logics[i] = newSvc;
    }

    return StatusDefs::Success;
}

fs::IFS_BusinessLogic *FS_Application::_NewSvc(Int32 serviceId)
{
    return ServiceFactory::Create(serviceId);
}

void FS_Application::_GetLogics(std::vector<fs::IFS_BusinessLogic *> &logics)
{
    logics = _logics;
}

bool FS_Application::_CommonInit()
{
    auto st = fs::SystemUtil::GetProgramPath(true, g_RootPath);
    g_RootPath = fs::FS_DirectoryUtil::GetFileDirInPath(g_RootPath);
    if (g_RootPath.empty())
    {
        printf("GetProgramPath fail st[%d]\n", st);
        return false;
    }

    g_AllCfgsDir = "/Cfgs";
    return true;
}

