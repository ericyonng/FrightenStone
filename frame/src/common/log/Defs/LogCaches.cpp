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
 * @file  : LogCaches.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/06/27
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "frame/src/common/log/Defs/LogCaches.h"
#include "FrightenStone/common/log/Defs/LogData.h"
#include "FrightenStone/common/component/Impl/File/LogFile.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_ANCESTOR_IMPL(LogDataCache, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_ANCESTOR_IMPL(LogCaches, __DEF_OBJ_POOL_OBJ_NUM__);

LogDataCache::LogDataCache()
    :_cache{new std::list<LogData *>}
{
}

fs::LogDataCache::~LogDataCache()
{
    STLUtil::DelListContainer(*_cache);
    Fs_SafeFree(_cache);
}

LogCaches::LogCaches()
    :_swapCache(NULL)
    ,_cache4RealLog(NULL)
{
}

LogCaches::~LogCaches()
{
}

FS_NAMESPACE_END
