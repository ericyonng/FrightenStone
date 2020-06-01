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
 * @file  : CfgLoader.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/5/19
 * @brief : ÅäÖÃ¼ÓÔØÆ÷
 */
#include "stdafx.h"
#include "Service/LogicSvc/Cfgs/Interface/CfgLoader.h"
#include "Service/LogicSvc/Cfgs/Interface/AllCfgs.h"
#include "Service/LogicSvc/Modules/Common/Status/Status.h"

bool CfgLoader::_isloaded = false;
Int32 CfgLoader::LoadAllCfgs()
{
    if (_isloaded)
        return StatusDefs::Success;

    Int32 st = StatusDefs::Success;

    g_ProtocolTypeCfgMgr = new ProtocolTypeCfgMgr;
    st = g_ProtocolTypeCfgMgr->Load();
    if (st != StatusDefs::Success)
    {
        g_Log->e<CfgLoader>(_LOGFMT_("g_ProtocolTypeCfgMgr load cfgs fail st[%d]"), st);
        return st;
    }

    g_BlackListCfgMgr = new BlackListCfgMgr;
    st = g_BlackListCfgMgr->Load();
    if (st != StatusDefs::Success)
    {
        g_Log->e<CfgLoader>(_LOGFMT_("g_BlackListCfgMgr load cfgs fail st[%d]"), st);
        return st;
    }

    _isloaded = true;
    return StatusDefs::Success;
}

Int32 CfgLoader::ReloadAllCfgs()
{
    if (!_isloaded)
    {
        g_Log->e<CfgLoader>(_LOGFMT_("have no loading before, please load firstly"));
        return StatusDefs::Cfg_NotLoadBefore;
    }

    _isloaded = false;
    Int32 st = StatusDefs::Success;

    st = g_ProtocolTypeCfgMgr->Reload();
    if (st != StatusDefs::Success)
    {
        g_Log->e<CfgLoader>(_LOGFMT_("g_ProtocolTypeCfgMgr Reload cfgs fail st[%d]"), st);
        return st;
    }

    st = g_BlackListCfgMgr->Reload();
    if (st != StatusDefs::Success)
    {
        g_Log->e<CfgLoader>(_LOGFMT_("g_BlackListCfgMgr Reload cfgs fail st[%d]"), st);
        return st;
    }

    _isloaded = true;

    return StatusDefs::Success;
}

void CfgLoader::Clear()
{
    if (!_isloaded)
        return;

    if (g_ProtocolTypeCfgMgr)
    {
        g_ProtocolTypeCfgMgr->Clear();
        Fs_SafeFree(g_ProtocolTypeCfgMgr);
    }

    if (g_BlackListCfgMgr)
    {
        g_BlackListCfgMgr->Clear();
        Fs_SafeFree(g_BlackListCfgMgr);
    }

    _isloaded = false;
}
