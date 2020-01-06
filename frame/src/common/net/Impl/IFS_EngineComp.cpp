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
 * @file  : IFS_EngineComp.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/6
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"

FS_NAMESPACE_BEGIN
Int32 IFS_EngineComp::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    Int32 st = _myCompMq->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<IFS_EngineComp>(_LOGFMT_("_myCompMq->BeforeStart fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 IFS_EngineComp::Start()
{
    Int32 st = _myCompMq->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<IFS_EngineComp>(_LOGFMT_("_myCompMq->BeforeStart fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

FS_NAMESPACE_END
