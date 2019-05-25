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
// 
// class AesHandle
// {
// public:
//     const FS_AesDefs::Aes256Context &GetKey() const;
//     static bool GenerateKey(FS_String &key);
// 
//     // 加密解密
// public:
//     void Encrypt()
// 
// private:
//     FS_AesDefs::Aes256Context _key;
// public:
//     //生成key
//     bool GenerateKey(uint8_t *szKey);
// 
//     //解密解密
// protected:
//     //加密模块 只加密一组128bit 16字节数据 且buf大小必须是128位的整数倍
//     void Encrypt_ecb(uint8_t *buf);
//     //解密模块 只解密一组128bit数据 且buf大小必须是128位的整数倍
//     void Decrypt_ecb(uint8_t *buf);
// 
//     //初始化
// protected:
//     //初始化生成256key
//     void Init(bool bResetKey);
//     //清除密钥
//     void ClearKey();
// 
//     //加密模块
// protected:
// 
//     //辅助函数
// protected:
//     //扩展加密密钥
//     bool ExpandEncKey(uint8_t *k, uint8_t *rc);
//     //轮密钥加拷贝
//     void AddRoundKey_cpy(uint8_t *buf, uint8_t *key, uint8_t *cpk);
// 
//     //字节代替
//     void SubBytes(uint8_t *buf);
//     //正向行移位
//     void ShiftRows(uint8_t *buf);
//     //正向列混淆
//     void MixColumns(uint8_t *buf);
//     //轮密码加
//     void AddRoundKey(uint8_t *buf, uint8_t *key);
// 
//     //逆向行移位
//     void ShiftRows_Inv(uint8_t *buf);
//     //逆向字节代替
//     void SubBytes_Inv(uint8_t *buf);
//     //解密密钥扩展
//     void ExpandDecKey(uint8_t *k, uint8_t *rc);
//     //逆向列混淆
//     void MixColumns_Inv(uint8_t *buf);
// 
// 
//     //伽罗瓦域上的计算
// protected:
// 
//     //s盒计算
// #ifndef BACK_TO_TABLES
// 
//         //s盒中取相应的值
//     uint8_t RJ_SBox(uint8_t x);
//     //计算在伽罗瓦域上的乘逆
//     uint8_t GF_MulInv(uint8_t x); // calculate multiplicative inverse
//     //在GF域上计算对数
//     uint8_t GF_Log(uint8_t x); // calculate logarithm gen 3
//     //在GF域上计算对数的反函数
//     uint8_t GF_ALog(uint8_t x); // calculate anti-logarithm gen 3
//     //s盒的逆中取相应的值
//     uint8_t RJ_SBox_Inv(uint8_t x);
// #endif
//     //求指数
//     uint8_t RJ_xTime(uint8_t x);
// 
// 
// 
// protected:
//     aes256_context m_stAesKey;			//保留
//     aes256_context m_stBackAesKey;		//用于加解密
// };

#endif
