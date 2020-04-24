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
 * @file  : TestStream.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestStream_H__
#define __Test_TestStream_H__
#pragma once

#include "stdafx.h"

struct TestStreamMessage
{
    // 序列化反序列化（ObjType 必须有SerialzieTo(FS_Stream *), DeserializeFrom(FS_Stream *)接口）
public:
    TestStreamMessage();
    bool SerializeTo(fs::FS_Stream *stream) const;
    bool DeserializeFrom(fs::FS_Stream *stream);

public:
    BUFFER256 _buffer;
};


inline TestStreamMessage::TestStreamMessage()
{
    memset(_buffer, 0, sizeof(_buffer));
}

inline bool TestStreamMessage::SerializeTo(fs::FS_Stream *stream) const
{
    auto writeSteam = stream->CastTo<fs::FS_MsgWriteStream>();
    return writeSteam->Write(_buffer);
}

inline bool TestStreamMessage::DeserializeFrom(fs::FS_Stream *stream)
{
    auto readStream = stream->CastTo<fs::FS_MsgReadStream>();
    return readStream->Read(_buffer);
}

class TestStream
{
public:
    static void Run()
    {
        g_Log->InitModule();
        TestStreamMessage testMessage;
        strcpy(testMessage._buffer, "ni hao test tream!");
        fs::FS_Stream *newStream = new fs::FS_Stream(sizeof(testMessage));
        newStream->SerializeFrom(testMessage);

        fs::FS_String str;
        newStream->SerializeTo(str);
        fs::FS_Stream cache;
        cache.DeserializeFrom(str);
        TestStreamMessage messageCache;
        cache.DeserializeTo(messageCache);
        fs::FS_String md5str;
        str = "ni hao test tream!";
        fs::StringUtil::MakeMd5(str, md5str);
        g_Log->any<TestStream>("deserialize message cache %s md5:%s", messageCache._buffer, md5str.ToHexString().c_str());
        getchar();
    }
};

#endif
