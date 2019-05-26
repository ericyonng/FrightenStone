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
 * @file  : TestAes.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/05/27
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestAes_H__
#define __Test_TestAes_H__

#pragma once
#include "stdafx.h"
#include "rijndael.h"

class TestAes
{
public:
    static void Run()
    {
        U8 plaintext[17] = "3456789123456780";
        U8 key[16] = "fasdfasdfadfas9";
        aes_encrypt_ecb(AES_CYPHER_256, plaintext, 16, key);
        fs::FS_String strcach;
        strcach.AppendBitData(reinterpret_cast<const char *>( plaintext), 16);
        std::cout << "plaintext:" << "3456789123456780" << std::endl;
        std::cout << "cypher:" << strcach.ToHexString().c_str() << std::endl;

        aes_decrypt_ecb(AES_CYPHER_256, plaintext, 16, key);
        strcach.Clear();
        strcach.AppendBitData(reinterpret_cast<const char *>(plaintext), 16);
        std::cout << "plaintext:" << "3456789123456780" << std::endl;
        std::cout << "plaintext:" << strcach.ToHexString().c_str() << std::endl;
    }
};

#endif
