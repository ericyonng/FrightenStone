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
 * @file  : FS_SessionFactory.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/14
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Impl/FS_Session.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"

FS_NAMESPACE_BEGIN

FS_Session *FS_SessionFactory::Create(BuildSessionInfo &buildData)
{
    return new FS_Session(buildData);
}


// IFS_Session *FS_SessionFactory::Create(const BriefSessionInfo &sessionInfo, IMemoryAlloctor *memAlloctor, Int64 heartbeatIntervalMicroSeconds)
// {
// #ifdef _WIN32
//     return new FS_IocpSession(sessionInfo._sessionId, sessionInfo._sock, &(sessionInfo._addrInfo), memAlloctor, heartbeatIntervalMicroSeconds);
// #else
//     // TODO:linux
//     return NULL;
// #endif
// }

FS_NAMESPACE_END

