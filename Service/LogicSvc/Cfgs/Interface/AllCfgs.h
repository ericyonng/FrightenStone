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
 * @file  : AllCfgs.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/18
 * @brief : 所有配置 
            添加配置注意:需要添加cfg子项,mgr管理类,mgr是继承于ICfgMgr,且必须在CfgFileuniqueType添加各自的类型
 */
#ifndef __Service_LogicSvc_Cfgs_Interface_AllCfgs_H__
#define __Service_LogicSvc_Cfgs_Interface_AllCfgs_H__

#pragma once

#include "Service/LogicSvc/Cfgs/Impl/ProtocolTypeCfg.h"
#include "Service/LogicSvc/Cfgs/Impl/ProtocolTypeCfgMgr.h"
#include "Service/LogicSvc/Cfgs/Impl/BlackListCfg.h"
#include "Service/LogicSvc/Cfgs/Impl/BlackListCfgMgr.h"

#endif
