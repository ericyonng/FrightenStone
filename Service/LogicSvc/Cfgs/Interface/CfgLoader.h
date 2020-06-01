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
 * @file  : CfgLoader.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/5/19
 * @brief : 配置加载器
 */
#ifndef __Service_LogicSvc_Cfgs_Interface_CfgLoader_H__
#define __Service_LogicSvc_Cfgs_Interface_CfgLoader_H__

#pragma once

#include "FrightenStone/exportbase.h"

class CfgLoader
{
public:
    // load失败后请调用clear
    static Int32 LoadAllCfgs();
    // reload不可重入,必须先load后才可reload，reload失败后请clear,再调用load
    static Int32 ReloadAllCfgs();
    // 可重入
    static void Clear();
    
private:
    static bool _isloaded;
};

#endif
