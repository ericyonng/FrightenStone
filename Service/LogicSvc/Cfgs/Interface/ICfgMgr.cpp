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
 * @file  : ICfgMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/19
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Cfgs/Interface/ICfgMgr.h"
#include "Service/LogicSvc/Cfgs/Defs/CommonInclude.h"

ICfgMgr::ICfgMgr()
    :_isInit(false)
    , _ini(NULL)
{

}

ICfgMgr::~ICfgMgr()
{
    Fs_SafeFree(_ini);
}

Int32 ICfgMgr::Reload()
{
    if (!_ini)
        return StatusDefs::Cfg_NotLoadBefore;

    const auto iniPath = _ini->GetPath();
    if (iniPath.empty() ||
        !fs::FS_FileUtil::IsFileExist(iniPath.c_str()))
        return StatusDefs::Cfg_PathError;

    _isInit = false;

    // reload前处理...一般清理配置
    _OnWillReload();

    _ini->Clear();
    if (!_ini->Init(iniPath.c_str()))
        return StatusDefs::Cfg_InitFail;

    if (!_ini->HasCfgs())
        return StatusDefs::Cfg_HaveNoCfgs;

    const Int32 st = _OnLoadCfgs(true);
    if (st != StatusDefs::Success)
        return st;

    _isInit = true;
    _OnAfterReload();

    return StatusDefs::Success;
}

Int32 ICfgMgr::_Load(const Byte8 *iniPath)
{
    if (_isInit)
        return StatusDefs::Success;

    if (!iniPath || ::strlen(iniPath) == 0)
        return StatusDefs::Cfg_PathError;

    if (!_ini)
        _ini = new fs::FS_IniFile();

    if (!_ini->Init(iniPath))
    {
        Fs_SafeFree(_ini);
        return StatusDefs::Cfg_InitFail;
    }

    if (!_ini->HasCfgs())
    {
        Fs_SafeFree(_ini);
        return StatusDefs::Cfg_HaveNoCfgs;
    }

    const Int32 st = _OnLoadCfgs(false);
    if (st != StatusDefs::Success)
    {
        Fs_SafeFree(_ini);
        return st;
    }

    _isInit = true;
    return StatusDefs::Success;
}

void ICfgMgr::_OnAfterReload()
{
    // 抛reload事件
    auto oldMd5 = _md5;
    _UpdateMd5();

    auto ev = new fs::FS_Event(EventIds::Cfgs_Reload);
    ev->SetParam(FS_EventParam::CfgFileType, GetCfgFileType());
    ev->SetParam(FS_EventParam::OldCfgFileMd5, oldMd5);
    ev->SetParam(FS_EventParam::NewCfgFileMd5, _md5);
    g_GlobalEventMgr->FireEvent(ev);
}

void ICfgMgr::_UpdateMd5()
{
    const auto &content = _ini->GetFileContent();
    // fs::g_OpenSSL_Locker.Lock();
    _md5.Clear();
    fs::StringUtil::MakeMd5(content, _md5);
    // fs::g_OpenSSL_Locker.Unlock();
}
