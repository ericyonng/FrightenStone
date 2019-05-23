#ifndef __Base_Common_Socket_Impl_SocketUtil_H__
#define __Base_Common_Socket_Impl_SocketUtil_H__
/**
* @file SocketUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/24
* @brief
*/

#pragma once

#include <base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/net/net.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT SocketUtil
{
public:
    static int InitSocketEnv();
    static int ClearSocketEnv();
    static bool SetNoBlock(MYSOCKET socket);
    static bool SetBlock(MYSOCKET socket);

private:
    static bool _isInitEnv;
};

FS_NAMESPACE_END

#endif // !__Base_Common_Socket_Impl_SocketUtil_H__
