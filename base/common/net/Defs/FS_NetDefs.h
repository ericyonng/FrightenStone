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
 * @file  : FS_NetDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/03
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Defs_FS_NetDefs_H__
#define __Base_Common_Net_Defs_FS_NetDefs_H__
#pragma once


 //SOCKET
#ifdef _WIN32
#undef FD_SETSIZE
#define FD_SETSIZE      65535
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#else
#ifdef __APPLE__
#define _DARWIN_UNLIMITED_SELECT
#endif // !__APPLE__
#include<unistd.h> //uni std
#include<arpa/inet.h>
#include<string.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/tcp.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

//缓冲区最小单元大小
#ifndef RECV_BUFF_SZIE
#define RECV_BUFF_SZIE 8192
#define SEND_BUFF_SZIE 10240
#endif // !RECV_BUFF_SZIE

#endif
