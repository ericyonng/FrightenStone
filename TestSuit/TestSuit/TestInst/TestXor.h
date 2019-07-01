#ifndef __Test_TestXor_H__
#define __Test_TestXor_H__

#pragma once

#include "stdafx.h"

class TestXor
{
public:
    static void Run()
    {
//         fs::FS_Int64Random random(0, 127);
//         char pwd[MAX_PWD_LEN / 8] = {0};
//         for(Int32 i = 0; i < sizeof(pwd); ++i)
//         {
//             pwd[i] = static_cast<char>(random(g_RandomSeed));
//             printf("%02x", pwd[i]);
//         }
// 
//         char plaintext[MAX_PWD_LEN / 8] = "wo ai ni";
//         for(Int32 i = 0; i < sizeof(plaintext); ++i)
//         {
//             printf("%d", plaintext[i]);
//         }
// 
//         std::cout << std::endl;
// 
//         // 加密
//         for(Int32 i = 0; i < sizeof(plaintext); ++i)
//         {
//             plaintext[i] = plaintext[i] ^ pwd[i];
//             printf("%d", plaintext[i]);
//         }
// 
//         std::cout << std::endl;
// 
//         // 还原
//         for(Int32 i = 0; i < sizeof(plaintext); ++i)
//             plaintext[i] = plaintext[i] ^ pwd[i];
// 
//         std::cout << plaintext << std::endl;

        // 生成key
        char key[KEY_128BIT] = {0};
        fs::KeyGeneratorUtil::GetKey_128Bit(key);

        // 明文
        char plainText[1024] = "我 安利的激发了可是大家 爱上的看法静安寺jdsalkdjkf***@@dalk545";
        char cypherText[1024];

        // 加密
        fs::XorEncrypt::Encrypt(plainText, 1024, cypherText, 1024, key, KEY_128BIT);

        // 解密
        memset(plainText, 0, sizeof(plainText));
        fs::XorEncrypt::Decrypt(cypherText, 1024, plainText, 1024, key, KEY_128BIT);

        // 打印
        std::cout << plainText << std::endl;
        
    }
};

#endif
