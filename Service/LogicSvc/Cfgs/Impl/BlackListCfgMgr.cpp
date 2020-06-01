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
 * @file  : BlackListCfgMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/31
 * @brief : 黑名单配置管理
 */
#include "stdafx.h"
#include "Service/LogicSvc/Cfgs/Defs/CommonInclude.h"
#include "Service/LogicSvc/Cfgs/Impl/BlackListCfgMgr.h"
#include "Service/LogicSvc/Cfgs/Impl/BlackListCfg.h"

static const fs::FS_String g_BlackListCfgIniFileName = "BlackListCfgMgr.ini";
BlackListCfgMgr *g_BlackListCfgMgr = NULL;

BlackListCfgMgr::BlackListCfgMgr()
    :_cfgFileType(CfgFileUniqueType::BlackListCfgMgr)
{

}

BlackListCfgMgr::~BlackListCfgMgr()
{

}

Int32 BlackListCfgMgr::Load()
{
    auto iniPath = g_RootPath + g_AllCfgsDir + "/" + g_BlackListCfgIniFileName;
    return _Load(iniPath.c_str());
}

void BlackListCfgMgr::Clear()
{
    // 清理配置
    _allCfgs.clear();
    fs::STLUtil::DelMapContainer(_breakCountRefCfg);
}

void BlackListCfgMgr::_OnWillReload()
{
    Clear();
}

Int32 BlackListCfgMgr::_OnLoadCfgs(bool isReload)
{
    // 更新内存配置
    std::vector<fs::FS_String> keys;
    std::vector<fs::FS_String> expireTimeSlice;

    {// BreakCount
        fs::FS_String result;
        if (!_ini->ReadStr("BreakRuleCount", "BreakCount", "", result))
            return StatusDefs::Cfg_ReadValueFail;

        // 分割成数组
        fs::StringUtil::SplitString(result, ",", keys);
    }

    {// ExpireTimeSlice
        fs::FS_String result;
        if (!_ini->ReadStr("BreakRuleCount", "ExpireTimeSlice", "", result))
            return StatusDefs::Cfg_ReadValueFail;

        // 分割成数组
        fs::StringUtil::SplitString(result, ",", expireTimeSlice);
    }

    // 配置表必须要有主键
    if (keys.empty())
        return StatusDefs::Cfg_NoKeys;

    // 映射字典
    const Int32 quantity = static_cast<Int32>(keys.size());
    for (Int32 i = 0; i < quantity; ++i)
    {
        auto newCfg = new BlackListCfg;
        newCfg->_breakCount = fs::StringUtil::StringToInt32(keys[i].c_str());
        newCfg->_expireTimeSlice = fs::StringUtil::StringToInt64(expireTimeSlice[i].c_str());

        _breakCountRefCfg.insert(std::make_pair(newCfg->_breakCount, newCfg));
        _allCfgs.push_back(newCfg);
    }

    return StatusDefs::Success;
}
