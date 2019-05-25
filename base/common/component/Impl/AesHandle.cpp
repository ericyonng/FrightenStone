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
 * @file  : AesHandle.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/AesHandle.h"
#include <random>

bool AesHandle::GenerateKey(FS_String &key)
{

    char key[LEN_AES_256_KEY] = {0};
    DOTRY_B_NL
        //选取3位数字

        if(!szKey)
        {
            srand(U32(time(NULL)));
            int m = 0;
            for(int i = 0; i < LEN_AES_256_KEY; i++)
            {
                //选区种类
                m = rand() % AES_KEY_KIND_COUNT;
                switch(m)
                {
                    case AES_NUM_KIND:
                    {
                        //选取数字
                        key[i] = AesKeyNumTable[rand() % AES_KEY_NUM_LEN];
                        break;
                    }
                    case AES_CHAR_KIND:
                    {
                        //选取英文
                        key[i] = AesKeyCharTable[rand() % AES_KEY_CHAR_LEN];
                        break;
                    }
                    case AES_SYMBOL_KIND:
                    {
                        //选取字符
                        key[i] = AesKeySymbolTable[rand() % AES_KEY_SYMBOL_LEN];
                        //key[i] = DesKeyNumLib[rand() % DES_NUM_LEN];
                        break;
                    }
                }
            }
        }
        else
        {
            memmove(key, szKey, LEN_AES_256_KEY);
        }

    int iKeyLenCount = 0;

    //拷贝结果
    if(szKey)
    {
        iKeyLenCount = LEN_AES_256_KEY;
        memset(szKey, 0, LEN_AES_256_KEY);
        memmove(szKey, key, LEN_AES_256_KEY);
    }
    else
    {
        iKeyLenCount = LEN_AES_256_KEY;
    }

    memset(&m_stAesKey, 0, sizeof(m_stAesKey));
    memmove(m_stAesKey.key, key, iKeyLenCount);
    memmove(m_stAesKey.enckey, m_stAesKey.key, sizeof(m_stAesKey.enckey));
    memmove(m_stAesKey.deckey, m_stAesKey.key, sizeof(m_stAesKey.deckey));

    memset(&m_stBackAesKey, 0, sizeof(m_stBackAesKey));
    memmove(&m_stBackAesKey, &m_stAesKey, sizeof(m_stAesKey));

    DOCATCH_E1_NL(szKey);
    return true;
}

void AesHandle::Encrypt_ecb(const FS_String &key, const FS_String &plaintext, FS_String &cyphertext)
{

}

void AesHandle::Decrypt_ecb(const FS_String &key, const FS_String &cyphertext, FS_String &plaintext)
{

}

bool AesHandle::_ExpandEncKey(uint8_t *k, uint8_t *rc)
{

}

void AesHandle::_AddRoundKey_cpy(uint8_t *buf, uint8_t *key, uint8_t *cpk)
{

}

void AesHandle::_SubBytes(uint8_t *buf)
{

}

void AesHandle::_ShiftRows(uint8_t *buf)
{

}

void AesHandle::_MixColumns(uint8_t *buf)
{

}

void AesHandle::_AddRoundKey(uint8_t *buf, uint8_t *key)
{

}

void AesHandle::_ShiftRows_Inv(uint8_t *buf)
{

}

void AesHandle::_SubBytes_Inv(uint8_t *buf)
{

}

void AesHandle::_ExpandDecKey(uint8_t *k, uint8_t *rc)
{

}

void AesHandle::_MixColumns_Inv(uint8_t *buf)
{

}

uint8_t AesHandle::_RJ_SBox(uint8_t x)
{

}

uint8_t AesHandle::_GF_MulInv(uint8_t x)
{

}

uint8_t AesHandle::_GF_Log(uint8_t x)
{

}

uint8_t AesHandle::_GF_ALog(uint8_t x)
{

}

uint8_t AesHandle::_RJ_SBox_Inv(uint8_t x)
{

}

uint8_t AesHandle::_RJ_xTime(uint8_t x)
{

}

