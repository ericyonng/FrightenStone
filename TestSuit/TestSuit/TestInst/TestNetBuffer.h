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
 * @file  : TestNetBuffer.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/6/10
 * @brief : 测试网络缓冲区
 */

#pragma once

#ifndef __Test_TestNetBuffer_H__
#define __Test_TestNetBuffer_H__

#include "stdafx.h"

class TestNetBuffer
{
public:
    static void Run()
    {
        fs::AppUtil::Init();
        fs::MemoryAlloctorWithLimit memAlloc(FS_BUFF_SIZE_DEF, 128, 128 * 1024 * 1024);
        memAlloc.InitMemory();
        fs::IFS_NetBuffer recvBuffer(FS_BUFF_SIZE_DEF, &memAlloc);
        fs::LoginReq req;
        auto &loginData = req._loginData;
        strcpy(loginData._userName, "shy");
        strcpy(loginData._pwd, "123456");

        // 保证有剩余的buffer可以push
        for (Int32 i = 0; i < 41; ++i)
        {
            loginData._msgId = i + 1;
            fs::FS_String raw;
            req.SerializeTo(raw);
            recvBuffer.PushBack(raw.c_str(), raw.size());
        }

        // 消费....
        auto &len = recvBuffer.GetDataLen();
        const auto buffer = recvBuffer.GetData();
        const fs::NetMsgHeaderFmtType::PacketLenDataType *packetLen =
            reinterpret_cast<const fs::NetMsgHeaderFmtType::PacketLenDataType *>(buffer);

        // 拆包
        fs::NetMsgDecoder decoder;
        for (; *packetLen &&
            (len >= fs::NetMsgHeaderFmtType::_msgHeaderSize) &&
            (len >= *packetLen);)
        {
            // 1.解码
            if (!decoder.Decode(buffer))
                break;

            //e.FlushTime();
            //Int64 useTime = (s - e).GetTotalMilliSeconds();

            // 弹出消息
            fs::FS_String hex;
            fs::StringUtil::ToHexStringView(buffer, len, hex);
            recvBuffer.PopFront(decoder.GetMsgLen());
            hex.Clear();
            fs::StringUtil::ToHexStringView(buffer, len, hex);
            getchar();
        }

        // 消息弹出
    }
};

#endif