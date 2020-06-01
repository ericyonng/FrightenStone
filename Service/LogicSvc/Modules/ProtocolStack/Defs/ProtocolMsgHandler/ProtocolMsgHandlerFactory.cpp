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
 * @file  : ProtocolMsgHandlerFactory.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/5/19
 * @brief :
 */

#include "stdafx.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/ProtocolMsgHandler/ProtocolMsgHandlerFactory.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/ProtocolMsgHandler/ProtocolMsgType.h"
#include "Service/LogicSvc/Modules/ProtocolStack/Defs/ProtocolMsgHandler/NormalHandler.h"

fs::IDelegate<void, fs::FS_Session *> *ProtocolMsgHandlerFactory::Create(const ProtocolTypeCfg *cfg, IProtocolMsgHandlerMgr *msgHandlerMgr)
{
    switch (cfg->_handlerType)
    {
        case ProtocolMsgType::Normal:
        {
            auto newHandler = new NormalHandler(cfg, msgHandlerMgr);
            return fs::DelegatePlusFactory::CreateAndHelpDelObj(newHandler, &NormalHandler::OnSessionMsgHandle);
        }
        break;
        case ProtocolMsgType::Http:
            g_Log->w<ProtocolMsgHandlerFactory>(_LOGFMT_("http msg handler have no implement"));
            break;
        default:
        {
            g_Log->w<ProtocolMsgHandlerFactory>(_LOGFMT_("unkown protocolType[%d]"), cfg->_handlerType);
            break;
        }
    }

    return NULL;
}
