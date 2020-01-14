/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
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
 * @file  : GlobalSysMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/14
 * @brief :
 */

#include "stdafx.h"
#include "Logic/Common/LogicCommon.h"
#include "Logic/Modules/Common/ModuleCommon.h"
#include "Logic/Modules/GlobalSys/Impl/GlobalSysMgr.h"

IGlobalSysMgr *g_GlobalSysMgr = NULL;

GlobalSysMgr::GlobalSysMgr()
{
    g_GlobalSysMgr = this;
}

GlobalSysMgr::~GlobalSysMgr()
{
    _Cleanup();
}

Int32 GlobalSysMgr::RegisterGlobalSys(IFS_LogicSysFactory *sysFactory)
{
    auto sysInfo = sysFactory->CreateSysInfo();

    auto &sysName = sysInfo->GetSysName();
    if(GetSysInfo(sysName))
    {
        Fs_SafeFree(sysInfo);
        return StatusDefs::Repeat;
    }

    _sysInfosList.push_back(sysInfo);
    _sysInfosDict.insert(std::make_pair(sysName, sysInfo));

    //创建GlobalSys
    auto sys = reinterpret_cast<IGlobalSys *>(sysFactory->CreateSys());
    sys->SetSysInfo(sysInfo);
    sys->SetGlobalMgr(this);

    _globalSysNameRefGlobalSyss.insert(std::make_pair(sysName, sys));
    _globalSysNameRefIGlobalSyss.insert(std::make_pair(fs::FS_String("I") + sysName, sys));

    g_Log->i<GlobalSysMgr>(_LOGFMT_("Register global sys: [%s]"), sysInfo->ToString().c_str());

    return StatusDefs::Success;
}

Int32 GlobalSysMgr::OnInitialize()
{
    return StatusDefs::Success;
}

Int32 GlobalSysMgr::BeforeStart()
{
//     for(auto sysIt = _globalSysNameRefGlobalSyss.begin(); sysIt != _globalSysNameRefGlobalSyss.end(); ++sysIt)
//     {
//         // 前置取得变量
//         auto &sys = sysIt->second;
//         auto &sysName = sysIt->first;
//         auto sysInfo = sys->GetSysInfo();
//     }

    return StatusDefs::Success;
}

Int32 GlobalSysMgr::Start()
{
    return StatusDefs::Success;
}

void GlobalSysMgr::BeforeClose()
{
    _Cleanup();
}

void GlobalSysMgr::Close()
{

}

const ILogicSysInfo *GlobalSysMgr::GetSysInfo(const fs::FS_String &globalSysName) const
{
    auto it = _sysInfosDict.find(globalSysName);
    return it != _sysInfosDict.end() ? it->second : NULL;
}

const std::vector<ILogicSysInfo *> &GlobalSysMgr::GetSysInfosList() const
{
    return _sysInfosList;
}

const std::map<fs::FS_String, ILogicSysInfo *> &GlobalSysMgr::GetSysInfosDict() const
{
    return _sysInfosDict;
}

IGlobalSys *GlobalSysMgr::GetSys(const fs::FS_String &sysName)
{
    // 优先从Interface字典中索引
    auto it2 = _globalSysNameRefIGlobalSyss.find(sysName);
    if(it2 != _globalSysNameRefIGlobalSyss.end())
        return it2->second;

    // 从实现类字典中索引
    auto it1 = _globalSysNameRefGlobalSyss.find(sysName);
    return it1 != _globalSysNameRefGlobalSyss.end() ? it1->second : NULL;
}

void GlobalSysMgr::_Cleanup()
{
    _sysInfosList.clear();
    fs::STLUtil::DelMapContainer(_sysInfosDict);
    fs::STLUtil::DelMapContainer(_globalSysNameRefGlobalSyss);
    _globalSysNameRefIGlobalSyss.clear();
}
