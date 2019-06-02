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
#include "base/common/asyn/asyn.h"
#include "base/common/component/Impl/FS_Random.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/component/Impl/Aes/FS_Aes.h"
#include "base/common/status/status.h"

#define __USE_FS_3RD_OPENSSL__
#include "3rd/3rd.h"

#pragma region defines
static const int g_aes_key_bytes[] = {
    /* AES_CYPHER_128 */  16,
    /* AES_CYPHER_192 */  24,
    /* AES_CYPHER_256 */  32,
};

///////////////////////////////////////////////////////////////////////////

FS_NAMESPACE_BEGIN

Int32 FS_Aes::GenerateKey(Int32 mode, FS_String &key)
{
    Int64 m = 0;
    FS_Int64Random genRandom(0, 127);
    g_RandomLocker.Lock();
    for(int i = 0; i < g_aes_key_bytes[mode]; i++)
        key.GetRaw().push_back(static_cast<U8>(genRandom(g_RandomSeed)));

    g_RandomLocker.Unlock();

    return StatusDefs::Success;
}

Int32 FS_Aes::Encrypt_Data(Int32 mode, const FS_String &key, const FS_String &plaintext, FS_String &cyphertext)
{
    const Int32 textSize = static_cast<Int32>(plaintext.size());
    if(UNLIKELY(plaintext.empty()))
        return StatusDefs::Aes_PlaintextIsEmpty;

    if(UNLIKELY(textSize < 16))
        return StatusDefs::Aes_TextLengthNotEnough;

    if(UNLIKELY(textSize % 16 != 0))
        return StatusDefs::Aes_Not16BytesMultiple;

    Int32 i = 0;
    if(cyphertext.size() < textSize)
        cyphertext.GetRaw().resize(textSize, 0);

    static AES_KEY innerKey;
    AES_set_encrypt_key(reinterpret_cast<const unsigned char *>(key.c_str()), g_aes_key_bytes[mode] * 8, &innerKey);
    while(i < textSize)
    {
        AES_encrypt(reinterpret_cast<const unsigned char *>(&plaintext[i]), reinterpret_cast<unsigned char *>(&cyphertext[i]), &innerKey);
        i += AES_BLOCK_SIZE;
    }

    return StatusDefs::Success;
}

Int32 FS_Aes::Decrypt_Data(Int32 mode, const FS_String &key,  const FS_String &cyphertext, FS_String &plaintext)
{
    const Int32 textSize = static_cast<Int32>(cyphertext.size());
    if(UNLIKELY(cyphertext.empty()))
        return StatusDefs::Aes_CyphertextIsEmpty;

    if(UNLIKELY(textSize < 16))
        return StatusDefs::Aes_TextLengthNotEnough;

    if(UNLIKELY(textSize % 16 != 0))
        return StatusDefs::Aes_Not16BytesMultiple;

    Int32 i = 0;
    if(UNLIKELY(plaintext.size() < textSize))
        plaintext.GetRaw().resize(textSize, 0);

    static AES_KEY innerKey;
    AES_set_decrypt_key(reinterpret_cast<const unsigned char *>(key.c_str()), g_aes_key_bytes[mode] * 8, &innerKey);
    while(i < textSize)
    {
        AES_decrypt(reinterpret_cast<const unsigned char *>(&cyphertext[i]), reinterpret_cast<unsigned char *>(&plaintext[i]), &innerKey);
        i += AES_BLOCK_SIZE;
    }

    return StatusDefs::Success;
}

FS_NAMESPACE_END

