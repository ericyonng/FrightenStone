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
 * @file  : ProtocolTypeCfgMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/18
 * @brief :
 */

#include "stdafx.h"
#include "Service/LogicSvc/Cfgs/Defs/CommonInclude.h"
#include "Service/LogicSvc/Cfgs/Impl/ProtocolTypeCfgMgr.h"
#include "Service/LogicSvc/Cfgs/Impl/ProtocolTypeCfg.h"

static const fs::FS_String g_ProtocalTypeCfgIniFileName = "ProtocolTypeCfgMgr.ini";
ProtocolTypeCfgMgr *g_ProtocolTypeCfgMgr = NULL;

ProtocolTypeCfgMgr::ProtocolTypeCfgMgr()
    :_cfgFileType(CfgFileUniqueType::ProtocolTypeCfgMgr)
{

}

ProtocolTypeCfgMgr::~ProtocolTypeCfgMgr()
{
    fs::STLUtil::DelMapContainer(_handlerTypeRefCfg);
}

Int32 ProtocolTypeCfgMgr::Load()
{
    auto iniPath = g_RootPath + g_AllCfgsDir + "/" + g_ProtocalTypeCfgIniFileName;
    return _Load(iniPath.c_str());
}

void ProtocolTypeCfgMgr::Clear()
{
    // 清理配置
    _allCfgs.clear();
    _portRefCfg.clear();
    fs::STLUtil::DelMapContainer(_handlerTypeRefCfg);
}

void ProtocolTypeCfgMgr::_OnWillReload()
{
    Clear();
}

Int32 ProtocolTypeCfgMgr::_OnLoadCfgs(bool isReload)
{
    // 更新内存配置
    std::vector<fs::FS_String> keys;
    std::vector<fs::FS_String> protocolTypes;
    std::vector<fs::FS_String> ports;

    {// HandlerType
        fs::FS_String result;
        if (!_ini->ReadStr("ProtocolHandlerDict", "HandlerType", "", result))
            return StatusDefs::Cfg_ReadValueFail;

        // 分割成数组
        fs::StringUtil::SplitString(result, ",", keys);
    }

    {// ProtocolType
        fs::FS_String result;
        if (!_ini->ReadStr("ProtocolHandlerDict", "ProtocolType", "", result))
            return StatusDefs::Cfg_ReadValueFail;

        // 分割成数组
        fs::StringUtil::SplitString(result, ",", protocolTypes);
    }

    {// Ports
        fs::FS_String result;
        if (!_ini->ReadStr("ProtocolHandlerDict", "Ports", "", result))
            return StatusDefs::Cfg_ReadValueFail;

        // 分割成数组
        fs::StringUtil::SplitString(result, ",", ports);
    }

    // 配置表必须要有主键
    if (keys.empty())
        return StatusDefs::Cfg_NoKeys;

    // 映射字典
    const Int32 quantity = static_cast<Int32>(keys.size());
    for (Int32 i = 0; i < quantity; ++i)
    {
        auto newCfg = new ProtocolTypeCfg;
        newCfg->_handlerType = fs::StringUtil::StringToInt32(keys[i].c_str());
        newCfg->_protocolType = protocolTypes[i];
        newCfg->_port = fs::StringUtil::StringToUInt16(ports[i].c_str());

        _handlerTypeRefCfg.insert(std::make_pair(newCfg->_handlerType, newCfg));
        _allCfgs.push_back(newCfg);
        _portRefCfg.insert(std::make_pair(newCfg->_port, newCfg));
    }

    return StatusDefs::Success;
}
