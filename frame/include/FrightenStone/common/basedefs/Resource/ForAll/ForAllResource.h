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
 * @file  : ForAllResource.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *    只能放第三方资源，或者标准库，不能将自己的资源包含在内
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_BaseDefs_Resource_ForAll_ForAllResource_H__
#define __Frame_Include_FrightenStone_Common_BaseDefs_Resource_ForAll_ForAllResource_H__

#pragma once

#pragma region c++ stl
#include <limits>
#include <stdio.h>
#include <atomic>
#include <thread>
#include<algorithm>
#include <stdexcept>
#include <deque>
#include <set>
#include <list>
#include <map>
#include "string.h"
#pragma endregion

// TODO:不可加在resource中
#pragma region base lib 
// #include "FrightenStone/common/memorypool/memorypool.h"
// #include "FrightenStone/common/objpool/objpool.h"
#pragma endregion

#ifdef _WIN32
#include<WinSock2.h>
#include <ws2def.h>
#include<ws2tcpip.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#pragma comment(lib,"ws2_32.lib")
#include<MSWSock.h>
#pragma comment (lib, "MSWSock.lib")
#else
#include <unistd.h>
#include <limits.h> // 含有PATH_MAX
#include<errno.h>

 // linux socket环境 以及相关网络接口
#include <sys/types.h>
#include <sys/socket.h>     // 含有 getaddrinfo等
#include <netdb.h>          //

#include<semaphore.h>
#include <pthread.h>
#include<sys/time.h>
#endif


#endif // !__Frame_Include_FrightenStone_Common_BaseDefs_Resource_ForAll_ForAllResource_H__
