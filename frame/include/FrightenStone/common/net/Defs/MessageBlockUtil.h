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
 * @file  : MessageBlockUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/29
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_MessageBlockUtil_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_MessageBlockUtil_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"

FS_NAMESPACE_BEGIN

struct FS_MessageBlock;
struct BriefSessionInfo;
struct IoEvent;
class IFS_NetModule;
class FS_EpollPoller;

class BASE_EXPORT MessageBlockUtil
{
public:
    static FS_MessageBlock *BuildTransferMonitorArrivedMessageBlock(UInt32 compId, UInt32 generatorId, IoEvent *ev, Int32 transferReason);
    static FS_MessageBlock *BuildTransferWillConnectMessageBlock(UInt32 compId
                                                                 , UInt32 generatorId
                                                                 , IFS_NetModule *netModule
                                                                 , BriefSessionInfo *newSessionInfo);
    static FS_MessageBlock *BuildConnectorPostConnectOpFinish(UInt32 compId
                                                              , UInt32 generatorId);


    #pragma region epoll
    #ifndef _WIN32
    static FS_MessageBlock *BuildTransferEpollEvSessionWillConnectMsg(UInt32 compId
                                                                      , UInt32 generatorId
                                                                      , FS_EpollPoller *poller
                                                                      , BriefSessionInfo *newSessionInfo);

    static FS_MessageBlock *BuildEpollInEvMessageBlock(UInt64 sessionId);
    static FS_MessageBlock *BuildEpollOutEvMessageBlock(UInt64 sessionId);
    #endif    
    #pragma endregion
};

FS_NAMESPACE_END

#endif
