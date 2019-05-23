/**
* @file ThreadUtil.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/
#include "stdafx.h"
#include "base/common/assist/utils/Impl/ThreadUtil.h"
#include "base/common/common.h"
#include "base/common/basedefs/Resource/Resource.h"

FS_NAMESPACE_BEGIN

void ThreadUtil::YieldScheduler()
{
    std::this_thread::yield();
}

FS_NAMESPACE_END
