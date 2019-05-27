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

class TestAes
{
public:
    static void Run()
    {
        fs::FS_String plaintext, cyphertext, key;
        plaintext = "Œ“∞¶ni";
        plaintext.GetRaw().resize(16, 0);
        cyphertext = plaintext;
        std::cout << fs::FS_Aes::GenerateKey(fs::FS_AesDefs::AES_CYPHER_192, key) << std::endl;
        fs::FS_Aes aesHandler;
        fs::Time nowTime, nextTime;
        nowTime.FlushTime();
        for(Int32 i = 0; i < 1000000; ++i)
        {
            aesHandler.Encrypt_Data(fs::FS_AesDefs::AES_CYPHER_192, key, plaintext, cyphertext);
            aesHandler.Decrypt_Data(fs::FS_AesDefs::AES_CYPHER_192, key, cyphertext, plaintext);
        }
        nextTime.FlushTime();
        std::cout << "before:" << nowTime.ToStringOfMillSecondPrecision() << std::endl;
        std::cout << "after:" << nextTime.ToStringOfMillSecondPrecision() << std::endl;
        std::cout << "cyphertext:" << cyphertext.ToHexString() << ",len:" << cyphertext.size() << std::endl;
        std::cout << "plaintext:" << plaintext << ",len:" << plaintext.size() << std::endl;
    }
};

#endif
