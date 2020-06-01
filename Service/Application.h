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
 * @file  : Application.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/15
 * @brief :
 */
#ifndef __Service_Application_H__
#define __Service_Application_H__

#pragma once

#include "FrightenStone/exportbase.h"

class FS_Application : public fs::IFS_NetEngine
{
public:
    FS_Application();
    virtual ~FS_Application();

    // Æô¶¯
public:
    void Run();

protected:
    virtual fs::IFS_ConfigMgr *_NewConfigMgr();
    virtual fs::IFS_ConfigMgr *_PreparConfigFile() const;
    virtual Int32 _ReadCustomCfgs();
    virtual Int32 _OnInitFinish();
    virtual fs::IFS_BusinessLogic *_NewSvc(Int32 serviceId);
    virtual void _GetLogics(std::vector<fs::IFS_BusinessLogic *> &logics);
    virtual bool _CommonInit();

protected:
    fs::IFS_ConfigMgr * _config;
    std::vector<fs::IFS_BusinessLogic *> _logics;
};

extern FS_Application *g_ThisApp;
#endif
