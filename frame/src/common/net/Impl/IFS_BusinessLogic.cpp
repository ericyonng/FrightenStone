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
 * @file  : IFS_BusinessLogic.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_BusinessLogic.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"

FS_NAMESPACE_BEGIN
IFS_BusinessLogic::~IFS_BusinessLogic()
{
    STLUtil::DelMapContainer(_protocolCmdRefHandlers);
}

// ¶©ÔÄÐ­Òé
void IFS_BusinessLogic::SubscribeProtocol(Int32 protocolCmd, IDelegate<void, UInt64, NetMsg_DataHeader *> *handler)
{
    auto iterHandler = _protocolCmdRefHandlers.find(protocolCmd);
    if(iterHandler != _protocolCmdRefHandlers.end())
    {
        g_Log->w<IFS_BusinessLogic>(_LOGFMT_("protocol cmd[%d] handler[%p] already exists, please check if cover it or has wrong operation!")
                                    , protocolCmd, iterHandler->second);
        Fs_SafeFree(iterHandler->second);
        _protocolCmdRefHandlers.erase(iterHandler);
    }

    _protocolCmdRefHandlers.insert(std::make_pair(protocolCmd, handler));
}

void IFS_BusinessLogic::InvokeProtocolHandler(Int32 protocolCmd, UInt64 userId, NetMsg_DataHeader *msgData)
{
    auto iterHandler = _protocolCmdRefHandlers.find(protocolCmd);
    if(iterHandler == _protocolCmdRefHandlers.end())
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("userId[%llu], protocolCmd[%d] have no handler!"), userId, protocolCmd);
        return;
    }

    auto handler = iterHandler->second;
    handler->Invoke(userId, msgData);
}

FS_NAMESPACE_END
