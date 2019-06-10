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
 * @file  : GolbalInitialize.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "assert.h"
#include "base/common/component/Impl/GlobalInitialize.h"
#include "base/common/status/status.h"

#pragma region to initialize
#include "base/common/component/Impl/SmartVar/SmartVar.h"
#include "base/common/socket/socket.h"
#pragma endregion


FS_NAMESPACE_BEGIN

GlobalInitialize::GlobalInitialize()
{

}

GlobalInitialize::~GlobalInitialize()
{

}

Int32 GlobalInitialize::Init()
{
    // TODO:some init ...
    Int32 ret = StatusDefs::Success;

    // 智能变量运行期类型识别
    SmartVarRtti::InitRttiTypeNames();

    // socket环境
    ret = SocketUtil::InitSocketEnv();
    if(ret != StatusDefs::Success)
    {
        throw std::logic_error("Socket init socket env failed");
        // assert(!"Socket init socket env failed");
        return ret;
    }

    return StatusDefs::Success;
}

FS_NAMESPACE_END
