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
 * @file  : ClientSocket.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#pragma once

#include <base/exportbase.h>

class ClientSocket
{
public:
    explicit ClientSocket(MYSOCKET socket = MYINVALID_SOCKET, bool blockSocket = false);
    virtual ~ClientSocket();

    MYSOCKET GetSocket() const;
    char *GetMsgBuf();
    Int32 GetLastPos() const;
    void SetLastPos(Int32 pos);

private:
    #pragma region 
    // socket fd_set  file desc set
    MYSOCKET _socket = MYINVALID_SOCKET;
    char _msgBuf[SOCKET_CACHE_SIZE * 10] = {0};
    Int32 _lastPos = 0;
    #pragma region
};

#pragma region Inline
inline MYSOCKET ClientSocket::GetSocket() const
{
    return _socket;
}

inline char *ClientSocket::GetMsgBuf()
{
    return _msgBuf;
}

inline Int32 ClientSocket::GetLastPos() const
{
    return _lastPos;
}

inline void ClientSocket::SetLastPos(Int32 pos)
{
    _lastPos = pos;
}

#pragma endregion
