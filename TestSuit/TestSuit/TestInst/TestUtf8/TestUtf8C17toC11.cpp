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
 * @file  : TestUtf8C17toC11.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/17
 * @brief :
 */
#include "stdafx.h"
#include "TestInst/TestUtf8/TestUtf8C17toC11.h"
#include "TestInst/TestJson.h"

void TestFallthrough()
{
    Int32 cp_bytes = 0;
    UInt64 cp = 0x55;
    Byte8 dest[100] = {};
    for(Int32 i = 0; i < 10; ++i)
    {
        cp_bytes = i;
        switch(cp_bytes) {
            case 7: dest[cp_bytes - 6] = 0x80 | ((cp >> 30) & 0x3F); // [[fallthrough]];
            case 6: dest[cp_bytes - 5] = 0x80 | ((cp >> 24) & 0x3F);// [[fallthrough]];
            case 5: dest[cp_bytes - 4] = 0x80 | ((cp >> 18) & 0x3F);// [[fallthrough]];
            case 4: dest[cp_bytes - 3] = 0x80 | ((cp >> 12) & 0x3F);// [[fallthrough]];
            case 3: dest[cp_bytes - 2] = 0x80 | ((cp >> 6) & 0x3F);// [[fallthrough]];
            case 2: dest[cp_bytes - 1] = 0x80 | ((cp >> 0) & 0x3F);
                dest[0] = static_cast<char>((std::uint_least16_t(0xFF00uL) >> cp_bytes) | (cp >> (6 * cp_bytes - 6)));
                break;
            case 1: dest[0] = static_cast<char>(cp);
        }
    }
}

void TestUtf8C17toC11::Run()
{
    TestJson::Run();
}
