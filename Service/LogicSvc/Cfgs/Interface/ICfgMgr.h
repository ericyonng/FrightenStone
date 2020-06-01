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
 * @file  : ICfgMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/19
 * @brief :
 */
#ifndef __Service_LogicSvc_Cfgs_Interface_ICfgMgr_H__
#define __Service_LogicSvc_Cfgs_Interface_ICfgMgr_H__

#pragma once

#include "FrightenStone/exportbase.h"

// 执行reload原则，
// reload执行器在afterreload的时候更新md5并比较新旧md5,判断是否有变化,
// 无轮是否有变化都需要抛reload事件，以便每个系统记录本系统是否需要reload配置，因为reload后旧配置会全部失效,
// 在所有配置reload完成之后必须抛reloadend事件,以便各个系统更新配置并恢复功能
class ICfgMgr
{
public:
    ICfgMgr();
    virtual ~ICfgMgr();

public:
    virtual Int32 Load() = 0;
    virtual Int32 Reload();
    virtual void Clear() = 0;

    bool IsInit() const;
    virtual Int32 GetCfgFileType() = 0;

    const fs::FS_String &GetMd5() const;

    // load: Load->_Load->_OnLoadCfgs
    // reload: Reload->_OnWillReload->_OnLoadCfgs->_OnAfterReload
protected:
    virtual Int32 _Load(const Byte8 *iniPath);
    virtual void _OnWillReload() = 0;
    virtual void _OnAfterReload();
    virtual Int32 _OnLoadCfgs(bool isReload) = 0;
    void _UpdateMd5();

protected:
    bool _isInit;
    fs::FS_IniFile *_ini;
    fs::FS_String _md5;
};

#pragma region Inline
inline bool ICfgMgr::IsInit() const
{
    return _isInit;
}

inline const fs::FS_String & ICfgMgr::GetMd5() const
{
    return _md5;
}
#pragma endregion

#endif
