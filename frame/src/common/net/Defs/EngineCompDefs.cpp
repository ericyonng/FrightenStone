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
 * @file  : EngineCompDefs.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/10
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/EngineCompDefs.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"
#include "FrightenStone/common/log/Log.h"

FS_NAMESPACE_BEGIN

FS_String EngineCompType::GetStr(Int32 compType)
{
    switch(compType)
    {
        case EngineCompType::Connector:
            return "Connector";
        case EngineCompType::Acceptor:
            return "Acceptor";
        case EngineCompType::Transfer:
            return "Transfer";
        case EngineCompType::Dispatcher:
            return "Dispatcher";
        default:
        {
            g_Log->w<EngineCompType>(_LOGFMT_("unknown comp type[%d]"), compType);
        }
        break;
    }

    return "unknown type";
}

void EngineCompsMethods::WaitForAllCompsReady(const IFS_NetEngine *engine)
{
    while(true)
    {
        if(engine->IsCompAllReady())
            break;

        SystemUtil::Sleep(1000);
    }
}

void EngineCompsMethods::WaitForAllCompsFinish(const IFS_NetEngine *engine)
{
    while(true)
    {
        if(engine->IsCompAllFinish())
            break;

        SystemUtil::Sleep(1000);
    }
}

void EngineCompsMethods::WaitForAllTransferFinish(const IFS_NetEngine *engine)
{
    while(true)
    {
        if(engine->IsAllTransferFinish())
            break;

        SystemUtil::Sleep(1000);
    }
}

void EngineCompsMethods::WaitForAllTransferIoFinish(const IFS_NetEngine *engine)
{
    while(true)
    {
        if(engine->IsAllTransferIoFinish())
            break;

        SystemUtil::Sleep(1000);
    }
}

FS_NAMESPACE_END

