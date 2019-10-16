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
 * @file  : SocketDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Socket_Defs_SocketDefs_H__
#define __Base_Common_Socket_Defs_SocketDefs_H__
#pragma once

#include"base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT SocketDefs
{
public:
    enum SOCKET_CACHE_TYPE
    {
        SOCKET_CACHE_TYPE_NONE = 0, //无效
        SOCKET_CACHE_TYPE_RECV,     // recv缓冲区
        SOCKET_CACHE_TYPE_SEND,     // send缓冲区
        SOCKET_CACHE_TYPE_END,
    };
};

FS_NAMESPACE_END

#ifndef INVALID_SOCKET_IDD
#define INVALID_SOCKET_IDD  ~0ULL       // 无效套接字
#endif

#ifndef IS_VALID_SOCKET_IDD
#define IS_VALID_SOCKET_IDD(x)  (((x)!=INVALID_SOCKET_IDD)&&((x)!=0))   // 是否有效的socketIDD
#endif

#ifndef __FS_DATA_1KB__
#define __FS_DATA_1KB__     1024 
#endif

#ifndef __FS_DATA_1MB__
#define __FS_DATA_1MB__     1048576
#endif

#ifndef __FS_DATA_1GB__
#define __FS_DATA_1GB__     1073741824
#endif

#endif

