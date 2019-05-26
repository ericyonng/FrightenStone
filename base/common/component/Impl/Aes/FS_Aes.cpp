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
 * @file  : FS_Aes.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/FS_Random.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/component/Impl/Aes/AesHandle.h"
#include "base/common/component/Impl/Aes/FS_Aes.h"

#define AES_KEY_NUM_LEN             10
#define AES_KEY_CHAR_LEN            52
#define AES_KEY_SYMBOL_LEN          32
#define AES_NUM_KIND                0           // 数字
#define AES_CHAR_KIND               1           // 英文
#define AES_SYMBOL_KIND             2           // 字符
#define AES_KEY_KIND_COUNT          3

// key 表
// 数值表
static const char AesKeyNumTable[AES_KEY_NUM_LEN] = {'0', '1', '2', '3', '4'
, '5', '6', '7', '8', '9'};

// 英文表
static const char AesKeyCharTable[AES_KEY_CHAR_LEN] = {'a', 'b', 'c', 'd', 'e'
, 'f', 'g', 'h', 'i', 'j'
, 'k', 'l', 'm', 'n', 'o'
, 'p', 'q', 'r', 's', 't'
, 'u', 'v', 'w', 'x', 'y'
, 'z'
, 'A', 'B', 'C', 'D', 'E'
, 'F', 'G', 'H', 'I', 'J'
, 'K', 'L', 'M', 'N', 'O'
, 'P', 'Q', 'R', 'S', 'T'
, 'U', 'V', 'W', 'X', 'Y'
, 'Z'};

// 其他符号
static const char AesKeySymbolTable[AES_KEY_SYMBOL_LEN] = { '`', '!', '@', '#', '$', '%', '^'
, '&', '*', '(', ')', '-', '=', '~', '_'
, '+', '[', ']', '{', '}', ';', '\'', '\\'
, ':', '"', '|', ',', '.', '/', '<', '>'
, '?'};
///////////////////////////////////////////////////////////////////////////

FS_NAMESPACE_BEGIN

FS_Aes::FS_Aes()
{
    _handle = new AesHandle;
}

FS_Aes::~FS_Aes()
{
    delete _handle;
}

bool FS_Aes::GenerateKey(FS_AesDefs::Aes256Context &key)
{
    memset(&key, 0, sizeof(key));

    Int64 m = 0;
    FS_Int64Random randomCh(0, AES_KEY_CHAR_LEN - 1);
    FS_Int64Random randomNum(0, AES_KEY_NUM_LEN - 1);
    FS_Int64Random randomSym(0, AES_KEY_SYMBOL_LEN - 1);
    FS_Int64Random genRandom(0, 2);
    g_RandomLocker.Lock();
    for(int i = 0; i < FS_AES_256_KEY_LEN; i++)
    {
        //选区种类
        m = genRandom(g_RandomSeed);
        switch(m)
        {
            case AES_NUM_KIND:
            {
                //选取数字
                char ch = AesKeyNumTable[randomNum(g_RandomSeed)];
                key.key[i] = ch;
                key.enckey[i] = ch;
                key.deckey[i] = ch;
                break;
            }
            case AES_CHAR_KIND:
            {
                // 选取英文
                char ch = AesKeyCharTable[randomCh(g_RandomSeed)];
                key.key[i] = ch;
                key.enckey[i] = ch;
                key.deckey[i] = ch;
                break;
            }
            case AES_SYMBOL_KIND:
            {
                //选取字符
                char ch = AesKeySymbolTable[randomSym(g_RandomSeed)];
                key.key[i] = ch;
                key.enckey[i] = ch;
                key.deckey[i] = ch;
                break;
            }
        }
    }
    g_RandomLocker.Unlock();

    return true;
}

bool FS_Aes::Encrypt_Data(const FS_AesDefs::Aes256Context &key, const FS_String &plaintext, FS_String &cyphertext)
{
    // 参数校验
    const Int64 plaintextSize = static_cast<Int64>(plaintext.size());
    if(plaintext.empty() || plaintext.size() % 16 != 0)
        return false;
    
    Int64 blockCnt = plaintextSize / 16;
    for(Int64 i = 0; i < blockCnt; ++i)
    {
        // 一次加密16字节
        _cache.Clear();
        _cache2.Clear();
        _cache.AppendBitData(plaintext.c_str() + i * 16, 16);
        _handle->Encrypt(key, _cache, _cache2);
        cyphertext.AppendBitData(_cache2.c_str(), _cache2.size());
    }

    return true;
}

bool FS_Aes::Decrypt_Data(const FS_AesDefs::Aes256Context &key, const FS_String &cyphertext, FS_String &plaintext)
{
    //参数校验
    const Int64 cypherSize = static_cast<Int64>(cyphertext.size());
    if(cyphertext.empty() || cypherSize % 16 != 0)
        return false;

    Int64 blockCnt = cypherSize / 16;
    for(Int64 i = 0; i < blockCnt; i++)
    {
        // 一次解密16字节
        _cache.Clear();
        _cache2.Clear();
        _cache.AppendBitData(cyphertext.c_str() + i * 16, 16);
        _handle->Decrypt(key, _cache, _cache2);
        plaintext.AppendBitData(_cache2.c_str(), 16);
    }

    return true;
}
FS_NAMESPACE_END
