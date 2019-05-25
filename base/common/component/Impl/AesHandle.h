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
 * @file  : AesHandle.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief : not export this class.use it as a inner class
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_AesHandle_H__
#define __Base_Common_Component_Impl_AesHandle_H__

#pragma once

#include "base/common/component/Defs/FS_AesDefs.h"

class FS_String;

class AesHandle
{
public:
    static bool GenerateKey(FS_String &key);

    // 加密解密
public:
    void Encrypt_ecb(const FS_String &key, const FS_String &plaintext, FS_String &cyphertext);
    void Decrypt_ecb(const FS_String &key, const FS_String &cyphertext, FS_String &plaintext);

private:
    // 扩展加密密钥
    bool _ExpandEncKey(uint8_t *k, uint8_t *rc);
    // 轮密钥加拷贝
    void _AddRoundKey_cpy(uint8_t *buf, uint8_t *key, uint8_t *cpk);

    // 字节代替
    void _SubBytes(uint8_t *buf);
    // 正向行移位
    void _ShiftRows(uint8_t *buf);
    // 正向列混淆
    void _MixColumns(uint8_t *buf);
    // 轮密码加
    void _AddRoundKey(uint8_t *buf, uint8_t *key);

    // 逆向行移位
    void _ShiftRows_Inv(uint8_t *buf);
    // 逆向字节代替
    void _SubBytes_Inv(uint8_t *buf);
    // 解密密钥扩展
    void _ExpandDecKey(uint8_t *k, uint8_t *rc);
    // 逆向列混淆
    void _MixColumns_Inv(uint8_t *buf);

    // s盒计算
#ifndef BACK_TO_TABLES
    // s盒中取相应的值
    uint8_t _RJ_SBox(uint8_t x);
    // 计算在伽罗瓦域上的乘逆
    uint8_t _GF_MulInv(uint8_t x); // calculate multiplicative inverse
    // 在GF域上计算对数
    uint8_t _GF_Log(uint8_t x); // calculate logarithm gen 3
    // 在GF域上计算对数的反函数
    uint8_t _GF_ALog(uint8_t x); // calculate anti-logarithm gen 3
    // s盒的逆中取相应的值
    uint8_t _RJ_SBox_Inv(uint8_t x);
#endif
    // 求指数
    uint8_t _RJ_xTime(uint8_t x);
};

#endif
