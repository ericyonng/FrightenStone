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
 * @file  : IFS_Facade.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Logicsys_Impl_IFS_Facade_H__
#define __Frame_Include_FrightenStone_Common_Logicsys_Impl_IFS_Facade_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"

FS_NAMESPACE_BEGIN

class IFS_BusinessLogic;
class IFS_MsgDispatcher;

class BASE_EXPORT IFS_Facade
{
public:
    IFS_Facade();
    virtual ~IFS_Facade() {}
    // initialize在businesslogic的start做,但在IFS_Facade的beforestart与start之前,在主线程做
    virtual Int32 OnInitialize() = 0;
    // BeforeStart与Start在businesslogic的Start做
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void WillClose() {}
    virtual void BeforeClose() {}
    virtual void Close() {}
    virtual void BindLogic(IFS_BusinessLogic *logic);
    IFS_MsgDispatcher *GetDispatcher();
    
protected:
    friend class IFS_BusinessLogic;

    bool _inited;
    bool _started;
    IFS_MsgDispatcher *_dispatcher;
    IFS_BusinessLogic *_logic;
};

FS_NAMESPACE_END

#include "FrightenStone/common/logicsys/Impl/IFS_FacadeImpl.h"

#endif
