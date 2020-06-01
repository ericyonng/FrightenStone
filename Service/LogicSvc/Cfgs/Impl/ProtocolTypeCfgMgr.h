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
 * @file  : ProtocolTypeCfgMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/18
 * @brief : 协议类型配置
 */
#ifndef __Service_LogicSvc_Cfgs_Impl_ProtocolTypeCfgMgr_H__
#define __Service_LogicSvc_Cfgs_Impl_ProtocolTypeCfgMgr_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "Service/LogicSvc/Cfgs/Interface/ICfgMgr.h"

class ProtocolTypeCfg;

class ProtocolTypeCfgMgr : public ICfgMgr
{
public:
    ProtocolTypeCfgMgr();
    ~ProtocolTypeCfgMgr();

public:
    virtual Int32 Load();
    virtual Int32 GetCfgFileType();
    virtual void Clear();

    #pragma region cfg api
public:
    const ProtocolTypeCfg *GetByHandlerType(Int32 handlerType) const;
    const std::vector<ProtocolTypeCfg *> *GetAllCfgs() const;
    const ProtocolTypeCfg *GetByPort(UInt16 port) const;
    #pragma endregion

protected:
    virtual void _OnWillReload();
    virtual Int32 _OnLoadCfgs(bool isReload);

private:
    Int32 _cfgFileType;
    std::vector<ProtocolTypeCfg *> _allCfgs;
    std::map<Int32, ProtocolTypeCfg *> _handlerTypeRefCfg;
    std::map<UInt16, ProtocolTypeCfg *> _portRefCfg;
};

#pragma region inline
inline Int32 ProtocolTypeCfgMgr::GetCfgFileType()
{
    return _cfgFileType;
}

inline const ProtocolTypeCfg *ProtocolTypeCfgMgr::GetByHandlerType(Int32 handlerType) const
{
    auto iterCfg = _handlerTypeRefCfg.find(handlerType);
    return iterCfg == _handlerTypeRefCfg.end() ? NULL : iterCfg->second;
}

inline const std::vector<ProtocolTypeCfg *> *ProtocolTypeCfgMgr::GetAllCfgs() const
{
    return &_allCfgs;
}

inline const ProtocolTypeCfg *ProtocolTypeCfgMgr::GetByPort(UInt16 port) const
{
    auto iterCfgs = _portRefCfg.find(port);
    return (iterCfgs == _portRefCfg.end()) ? NULL : (iterCfgs->second);
}
#pragma endregion

extern ProtocolTypeCfgMgr *g_ProtocolTypeCfgMgr;
#endif
