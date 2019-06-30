/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
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
 * @file  : IEasyGlobal.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/28
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Global_Interface_EasyGlobal_H__
#define __Base_Common_Global_Interface_EasyGlobal_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN
class ILog;

class IEasyGlobal
{
public:
    IEasyGlobal();
    virtual ~IEasyGlobal();

    #pragma region init/finish
public:
    virtual Int32 Init() = 0;
    virtual void Finish() = 0;
    #pragma endregion

    // 获取资源
public:
    virtual ILog *GetLog() = 0;
};

inline IEasyGlobal::IEasyGlobal()
{
}

inline IEasyGlobal::~IEasyGlobal() 
{
}

FS_NAMESPACE_END


#endif
