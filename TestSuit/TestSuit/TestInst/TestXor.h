#ifndef __Test_TestXor_H__
#define __Test_TestXor_H__

#pragma once

#include "stdafx.h"

class TestXor
{
public:
    static void Run()
    {
        fs::FS_Int64Random random(0, 127);
        char pwd[MAX_PWD_LEN / 8] = {0};
        for(Int32 i = 0; i < sizeof(pwd); ++i)
        {
            pwd[i] = static_cast<char>(random(g_RandomSeed));
            printf("%02x", pwd[i]);
        }

        char plaintext[MAX_PWD_LEN / 8] = "wo ai ni";
        for(Int32 i = 0; i < sizeof(plaintext); ++i)
        {
            printf("%d", plaintext[i]);
        }

        std::cout << std::endl;

        // ¼ÓÃÜ
        for(Int32 i = 0; i < sizeof(plaintext); ++i)
        {
            plaintext[i] = plaintext[i] ^ pwd[i];
            printf("%d", plaintext[i]);
        }

        std::cout << std::endl;

        // »¹Ô­
        for(Int32 i = 0; i < sizeof(plaintext); ++i)
            plaintext[i] = plaintext[i] ^ pwd[i];

        std::cout << plaintext << std::endl;
        
    }
};

#endif
