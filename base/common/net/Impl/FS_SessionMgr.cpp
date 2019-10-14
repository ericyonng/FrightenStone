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
 * @file  : FS_SessionMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_SessionMgr.h"
#include "base/common/net/Impl/IFS_Session.h"

#include "base/common/status/status.h"

FS_NAMESPACE_BEGIN

FS_SessionMgr::FS_SessionMgr()
{

}

FS_SessionMgr::~FS_SessionMgr()
{

}

Int32 FS_SessionMgr::BeforeStart()
{
    return StatusDefs::Success;
}

Int32 FS_SessionMgr::Start()
{
    return StatusDefs::Success;
}

Int32 FS_SessionMgr::AfterStart()
{
    return StatusDefs::Success;
}

void FS_SessionMgr::BeforeClose()
{
}

void FS_SessionMgr::Close()
{

}

void FS_SessionMgr::AfterClose()
{
}

void FS_SessionMgr::AddNewSession(UInt64 sessionId, IFS_Session *session)
{

}

void FS_SessionMgr::EraseSession(UInt64 sessionId)
{

}

FS_NAMESPACE_END
