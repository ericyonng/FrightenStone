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
 * @file  : IUserSys.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/15
 * @brief :
 */
#include "stdafx.h"
#include "Service/LogicSvc/Modules/User/Impl/IUserSys.h"

UInt64 IUserSys::GetSessionId() const
{
    return _user->GetSessionId();
}

UInt64 IUserSys::GetUserId() const
{
    return _user->GetUseId();
}

IUserSys *IUserSys::GetSys(const fs::FS_String &sysName)
{
    return _user != NULL ? _user->GetSys(sysName) : NULL;
}

const IUserSys *IUserSys::GetSys(const fs::FS_String &sysName) const
{
    return _user != NULL ? _user->GetSys(sysName) : NULL;
}