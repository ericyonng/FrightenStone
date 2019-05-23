#ifndef __Base_Common_Common_H__
#define __Base_Common_Common_H__
/**
* @file net.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/18
* @brief
*/

#pragma once

// defs ...
// Impl ...
// Interface ...
#include "base/common/basedefs/Resource/Resource.h"
#pragma region 基础库代码(对内部模块屏蔽)
#ifndef FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL
#include<base/common/socket/socket.h>
#include <base/common/net/net.h>
#include <base/common/status/status.h>
#include "base/common/asyn/asyn.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/assist/assist.h"
#include "base/common/component/component.h"
#endif
#pragma endregion

#endif // !__Base_Common_Common_H__
