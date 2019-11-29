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
 * @file  : FS_IPUtilDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_Defs_FS_IPUtilDefs_H__
#define __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_Defs_FS_IPUtilDefs_H__
#pragma once

class FS_IPUtilDefs
{
public:
    // 族类型
    enum FAMILY_TYPE
    {
        FAMILY_TYPE_UNSPEC = 0,     //  protocal nonrelationship
        FAMILY_TYPE_AF_INET = 2,    //  IPV4 AF_INET
        FAMILY_TYPE_AF_INET6 = 23,	//  IPV6 AF_INET6
    };

    // 协议类型
    enum PROTOCOL_TYPE
    {
        PROTOCOL_TYPE_IPPROTO_IP = 0,       //  ip protocal
        PROTOCOL_TYPE_IPPROTO_IPV4 = 4,     //  ipv4
        PROTOCOL_TYPE_IPPROTO_TCP = 6,      //  tcp
        PROTOCOL_TYPE_IPPROTO_UDP = 17,     //  udp
        PROTOCOL_TYPE_IPPROTO_IPV6 = 41,    //  IPV6
    };

    //  传输的套接字类型
    enum SOCK_TYPE
    {
        SOCK_TYPE_SOCK_STREAM = 1,          // stream
        SOCK_TYPE_SOCK_DGRAM = 2,           // Datagram
    };

    // ai_flags
    enum AI_FLAGS_TYPE
    {
        AI_FLAGS_TYPE_NONE = 0,             // begin
        AI_FLAGS_TYPE_AI_PASSIVE = 1,       // passive，use to bind，usually in server socket case
        AI_FLAGS_TYPE_AI_CANONNAME = 2,     // use to get computer standard name
        AI_FLAGS_TYPE_AI_NUMERICHOST = 4,   // addr is numeric string
        AI_FLAGS_TYPE_END = 8,              // ai_flags value range[0,8)
    };

    // ai_flags的值的范围为0~7，取决于程序如何设置3个标志位，
    // 比如设置ai_flags为“AI_PASSIVE | AI_CANONNAME”，ai_flags值就为3。
    // 三个参数的含义分别为：
    // (1)AI_PASSIVE 当此标志置位时，表示调用者将在bind()函数调用中使用返回的地址结构,
    // 当此标志不置位时，表示将在connect()函数调用中使用。
    // 当节点名位NULL，且此标志置位，则返回的地址将是通配地址。
    // 如果节点名NULL，且此标志不置位，则返回的地址将是回环地址。
    // (2)AI_CANNONAME 当此标志置位时，在函数所返回的第一个addrinfo结构中的ai_cannoname成员中，
    // 应该包含一个以空字符结尾的字符串，字符串的内容是节点名的正规名。
    // (3)AI_NUMERICHOST 当此标志置位时，此标志表示调用中的节点名必须是一个数字地址字符串。
};
#endif
