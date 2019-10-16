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
 * @file  : FS_IPUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Assist_Utils_Impl_FS_IPUtil_H__
#define __Base_Common_Assist_Utils_Impl_FS_IPUtil_H__
#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/basedefs/Macro/ForAll/ForAllMacro.h"

FS_NAMESPACE_BEGIN
class FS_String;

class BASE_EXPORT FS_IPUtil
{
public:
    static Int32 GetLocalIP(FS_String &ip, Int32 netCardNo = 0, bool isToBind = true, bool isStreamSock = true, bool isIpv4 = true);
    static Int32 GetIPByDomain(
        const char *domain                                                // 域名或主机名
        , const char *service                                             /* 端口号“80”等、服务名 如"ftp", "http"等 */
        , FS_String &ipAddrString                                           // 输出ip
        , Int32 netCardNo = 0                                                // 网卡序号若获取的是本地的地址，为选择网址列表的某一个网址
        , Int32 eFlags = FS_IPUtilDefs::AI_FLAGS_TYPE_AI_PASSIVE                 // FS_IPUtilDefs::AI_FLAGS_TYPE 各个位的组合 默认AI_PASSIVE 即用于bind绑定 不设置则用于connect
        , FS_IPUtilDefs::SOCK_TYPE eSockType = FS_IPUtilDefs::SOCK_TYPE_SOCK_STREAM	        // 默认流数据
        , FS_IPUtilDefs::FAMILY_TYPE eFamily = FS_IPUtilDefs::FAMILY_TYPE_AF_INET           // 默认ipv4
        , FS_IPUtilDefs::PROTOCOL_TYPE eProtocol = FS_IPUtilDefs::PROTOCOL_TYPE_IPPROTO_IP  // 默认任意协议 即ip协议
    );
};

FS_NAMESPACE_END

#endif
