#ifndef __Base_Common_Assist_Utils_Impl_FS_CpuUtil_H__
#define __Base_Common_Assist_Utils_Impl_FS_CpuUtil_H__
/**
* @file FS_CpuUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/20
* @brief
*/

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_CpuUtil
{
public:
    static bool Initialize();
    static Double GetUsage();
    static Int32 GetCpuCoreCnt();

private:
    static Int64 _CompareFileTime(FILETIME time1, FILETIME time2);

private:
    static FILETIME _preidleTime;
    static FILETIME _preKernalTime;
    static FILETIME _preUserTime;
    static bool _isInit;
};

#pragma region Inline

#pragma endregion

FS_NAMESPACE_END

#endif
