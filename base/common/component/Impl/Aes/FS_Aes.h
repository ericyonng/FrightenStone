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
 * @file  : FS_Aes.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 *          base on the aes of openssl
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_FS_Aes_H__
#define __Base_Common_Component_Impl_FS_Aes_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/component/Defs/FS_AesDefs.h"
#include "base/common/objpool/objpool.h"

typedef struct aes_key_st AES_KEY;

FS_NAMESPACE_BEGIN
class FS_String;
class BASE_EXPORT FS_Aes
{
    OBJ_POOL_CREATE(_objPoolHelper)
public:
    FS_Aes();
    virtual ~FS_Aes();

    // 产生密钥
    static Int32 GenerateKey(Int32 mode, FS_String &key);
    // 加密 需要是128bit 16字节的倍数
    Int32 Encrypt_Data(Int32 mode, const FS_String &key, const FS_String &plaintext, FS_String &cyphertext);
    // 解密 需要是128bit 16字节的倍数
    Int32 Decrypt_Data(Int32 mode, const FS_String &key,  const FS_String &cyphertext, FS_String &plaintext);

    // openssl加密算法线程不安全需要加锁
    void Lock();
    void Unlock();

private:
    AES_KEY *_GetKeyCache();
private:
    AES_KEY *_innerKey;
};

inline void FS_Aes::Lock()
{
    g_OpenSSL_Locker.Lock();
}

inline void FS_Aes::Unlock()
{
    g_OpenSSL_Locker.Unlock();
}

FS_NAMESPACE_END

#endif
