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
 * @file  : BlackListCfgMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/31
 * @brief : 黑名单配置管理
 */
#ifndef __Service_LogicSvc_Cfgs_Impl_BlackListCfgMgr_H__
#define __Service_LogicSvc_Cfgs_Impl_BlackListCfgMgr_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "Service/LogicSvc/Cfgs/Interface/ICfgMgr.h"

class BlackListCfg;

class BlackListCfgMgr : public ICfgMgr
{
public:
    BlackListCfgMgr();
    ~BlackListCfgMgr();

public:
    virtual Int32 Load();
    virtual Int32 GetCfgFileType();
    virtual void Clear();

    #pragma region cfg api
public:
    const std::vector<BlackListCfg *> *GetAllCfgs() const;
    const BlackListCfg *GetByBreakCount(Int32 breakCount) const;
    #pragma endregion

protected:
    virtual void _OnWillReload();
    virtual Int32 _OnLoadCfgs(bool isReload);

private:
    Int32 _cfgFileType;
    std::vector<BlackListCfg *> _allCfgs;
    std::map<Int32, BlackListCfg *> _breakCountRefCfg;
};

#pragma region inline
inline Int32 BlackListCfgMgr::GetCfgFileType()
{
    return _cfgFileType;
}

inline const std::vector<BlackListCfg *> *BlackListCfgMgr::GetAllCfgs() const
{
    return &_allCfgs;
}

inline const BlackListCfg *BlackListCfgMgr::GetByBreakCount(Int32 breakCount) const
{
    auto iterCfg = _breakCountRefCfg.find(breakCount);
    return iterCfg == _breakCountRefCfg.end() ? NULL : iterCfg->second;
}
#pragma endregion

extern BlackListCfgMgr *g_BlackListCfgMgr;
#endif
