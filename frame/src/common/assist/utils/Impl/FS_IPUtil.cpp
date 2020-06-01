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
 * @file  : FS_IPUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#pragma region winsock 2

#pragma endregion

#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/assist/utils/Defs/FS_IPUtilDefs.h"
#include "FrightenStone/common/assist/utils/Impl/FS_IPUtil.h"
#include "FrightenStone/common/status/status.h"

#ifndef _WIN32
#include <arpa/inet.h>
#endif

FS_NAMESPACE_BEGIN

Int32 FS_IPUtil::GetLocalIP(FS_String &ip, Int32 netCardNo, bool isToBind, bool isStreamSock, bool isIpv4)
{
    // 获得本机主机名+
    char hostname[MAX_PATH] = {0};
    if(gethostname(hostname, MAX_PATH) != 0)
        return false;

    //获取ip
    auto ret = GetIPByDomain(hostname, NULL
                             , ip
                             , netCardNo
                             , isToBind ? FS_IPUtilDefs::AI_FLAGS_TYPE_AI_PASSIVE : FS_IPUtilDefs::AI_FLAGS_TYPE_NONE
                             , isStreamSock ? FS_IPUtilDefs::SOCK_TYPE_SOCK_STREAM : FS_IPUtilDefs::SOCK_TYPE_SOCK_DGRAM
                             , isIpv4 ? FS_IPUtilDefs::FAMILY_TYPE_AF_INET : FS_IPUtilDefs::FAMILY_TYPE_AF_INET6
                             , FS_IPUtilDefs::PROTOCOL_TYPE_IPPROTO_IP
    );

    if(ret != StatusDefs::Success)
        return ret;

    return StatusDefs::Success;
}

Int32 FS_IPUtil::GetIPByDomain(
    const char *domain         // 域名或主机名
    , const char *service      /* 端口号“80”等、服务名 如"ftp", "http"等 */
    , FS_String &ipAddrString       // 输出ip
    , Int32 netCardNo           // 网卡序号若获取的是本地的地址，为选择网址列表的某一个网址
    , Int32 eFlags                  /* = FS_IPUtilDefs::AI_FLAGS_TYPE 各个位的组合
                                       默认AI_PASSIVE 即用于bind绑定 不设置则用于connect */
    , FS_IPUtilDefs::SOCK_TYPE eSockType /*= FS_IPUtilDefs::SOCK_TYPE_SOCK_STREAM /* 默认流数据 */
    , FS_IPUtilDefs::FAMILY_TYPE eFamily /*= FS_IPUtilDefs::FAMILY_TYPE_AF_INET /* 默认ipv4 */
    , FS_IPUtilDefs::PROTOCOL_TYPE eProtocol /*= FS_IPUtilDefs::PROTOCOL_TYPE_IPPROTO_IP /* 默认任意协议 即ip协议 */)
{
    // 不可同时为NULL
    if(UNLIKELY(!domain && !service))
        return StatusDefs::FS_IPUtil_ParamError;

#ifndef _WIN32
    BUFFER64 ipcache = {};
#endif
    
    // 获取地址的参数设置
    struct addrinfo hints = {0};
    hints.ai_family = eFamily;
    hints.ai_flags = eFlags;
    hints.ai_protocol = eProtocol;
    hints.ai_socktype = eSockType;
    struct addrinfo *netCardRes = NULL;
    if(getaddrinfo(domain, service, &hints, &netCardRes) != 0)
    {
        throw std::logic_error("getaddrinfo failed");
#ifndef _WIN32
        perror("getaddrinfo fail");
#endif
        return StatusDefs::FS_IPUtil_GetAddrInfoFailed;
    }

    // 遍历网卡
    Int32 cnt = 0;
    struct sockaddr_in *addr = NULL;
    struct addrinfo *cur = NULL;
    for(cur = netCardRes; cur != NULL; cur = cur->ai_next)
    {
        addr = (struct sockaddr_in *)cur->ai_addr;
        if(!addr)
            continue;

        if(netCardNo != cnt)
        {
            ++cnt;
            continue;
        }

#ifdef _WIN32
        ipAddrString.AppendFormat("%d.%d.%d.%d"
                , (*addr).sin_addr.S_un.S_un_b.s_b1
                , (*addr).sin_addr.S_un.S_un_b.s_b2
                , (*addr).sin_addr.S_un.S_un_b.s_b3
                , (*addr).sin_addr.S_un.S_un_b.s_b4);
#else

        
        if(inet_ntop(eFamily, &(addr->sin_addr.s_addr), ipcache, sizeof(ipcache)) == NULL)
        {
            perror("inet_ntop fail");
            continue;
        }

        ipAddrString.AppendFormat("%s", ipcache);
#endif
        freeaddrinfo(netCardRes);
        return StatusDefs::Success;
    }

    //释放资源
    freeaddrinfo(netCardRes);
    return StatusDefs::FS_IPUtil_NotFound;
}

FS_NAMESPACE_END
