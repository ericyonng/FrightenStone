#ifndef MY_AES_H_
#define MY_AES_H_
#include "BaseCode/MyDataType.h"
#include <BaseCode/myclassmacro.h>

/*
*   Byte-oriented AES-256 implementation.
*   All lookup tables replaced with 'on the fly' calculations.
*
*   Copyright (c) 2007-2009 Ilya O. Levin, http://www.literatecode.com
*   Other contributors: Hal Finney
*
*   Permission to use, copy, modify, and distribute this software for any
*   purpose with or without fee is hereby granted, provided that the above
*   copyright notice and this permission notice appear in all copies.
*
*   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
AES是高级加密标准。
高级加密标准（英语：Advanced Encryption Standard，缩写：AES），在密码学中又称Rijndael加密法，
是美国联邦政府采用的一种区块加密标准。这个标准用来替代原先的DES，已经被多方分析且广为全世界所使用。
经过五年的甄选流程，高级加密标准由美国国家标准与技术研究院（NIST）于2001年11月26日发布于FIPS PUB 197，
并在2002年5月26日成为有效的标准。2006年，高级加密标准已然成为对称密钥加密中最流行的算法之一。

设计者
该算法为比利时密码学家Joan Daemen和Vincent Rijmen所设计，结合两位作者的名字，以Rijndael之命名之，
投稿高级加密标准的甄选流程。Rijdael的发音近于 "Rhinedoll，两位设计者的名字，
参考荷兰语原发音可译为尤安·达蒙、文森特·莱蒙。（Joan不能译为女性化的名字“琼”。
另外，西欧的姓名很多是有相同拉丁文或希腊文源头的，故译成中文是可能相同）

加密标准
对称密码体制的发展趋势将以分组密码为重点。分组密码算法通常由密钥扩展算法和加密（解密）算法两部分组成。
密钥扩展算法将b字节用户主密钥扩展成r个子密钥。加密算法由一个密码学上的弱函数f与r个子密钥迭代r次组成。
混乱和密钥扩散是分组密码算法设计的基本原则。抵御已知明文的差分和线性攻击，可变长密钥和分组是该体制的设计要点。
AES是美国国家标准技术研究所NIST旨在取代DES的21世纪的加密标准。
AES的基本要求是，采用对称分组密码体制，密钥长度的最少支持为128、192、256，分组长度128位，
算法应易于各种硬件和软件实现。1998年NIST开始AES第一轮分析、测试和征集，共产生了15个候选算法。

1999年3月完成了第二轮AES2的分析、测试。
2000年10月2日美国政府正式宣布选中比利时密码学家Joan Daemen 和 Vincent Rijmen 提出的一种密码算法RIJNDAEL 作为 AES.
在应用方面，尽管DES在安全上是脆弱的，但由于快速DES芯片的大量生产，使得DES仍能暂时继续使用，
为提高安全强度，通常使用独立密钥的三级DES。但是DES迟早要被AES代替。流密码体制较之分组密码在理论上成熟且安全，
但未被列入下一代加密标准。

AES加密数据块分组长度必须为128比特，
密钥长度可以是128比特、192比特、256比特中的任意一个（如果数据块及密钥长度不足时，会补齐）。
AES加密有很多轮的重复和变换。大致步骤如下：1、密钥扩展（KeyExpansion），2、初始轮（Initial Round），
3、重复轮（Rounds），每一轮又包括：SubBytes、ShiftRows、MixColumns、AddRoundKey，4、最终轮（Final Round），
最终轮没有MixColumns。
*/

//#define BACK_TO_TABLES //使用现有S盒常数组

#include <BaseCode/LEN_MACRO.h>

// #ifdef __cplusplus
// extern "C" {
// #endif


	class  CAes
	{
		NO_COPY_NEED_RVALUE(CAes);
		HEAP_CREATE_NEED_CLEANS_DEFINE(CAes);
	public:
		typedef struct {
			uint8_t key[LEN_AES_256_KEY];
			uint8_t enckey[LEN_AES_256_KEY];
			uint8_t deckey[LEN_AES_256_KEY];
		} aes256_context;

	public:
		CAes(bool bGenerateKey);
		CAes(uint8_t *key);

	public:
		//获取key
		CAes::aes256_context GetKey(){ return m_stAesKey; }
		//生成key
		bool GenerateKey(uint8_t *szKey);

		//解密解密
	protected:
		//加密模块 只加密一组128bit 16字节数据 且buf大小必须是128位的整数倍
		void Encrypt_ecb(uint8_t *buf);
		//解密模块 只解密一组128bit数据 且buf大小必须是128位的整数倍
		void Decrypt_ecb(uint8_t *buf);

		//初始化
	protected:
		//初始化生成256key
		void Init(bool bResetKey);
		//清除密钥
		void ClearKey();

		//加密模块
	protected:

		//辅助函数
	protected:
		//扩展加密密钥
		bool ExpandEncKey(uint8_t *k, uint8_t *rc);
		//轮密钥加拷贝
		void AddRoundKey_cpy(uint8_t *buf, uint8_t *key, uint8_t *cpk);
		
		//字节代替
		void SubBytes(uint8_t *buf);
		//正向行移位
		void ShiftRows(uint8_t *buf);
		//正向列混淆
		void MixColumns(uint8_t *buf);
		//轮密码加
		void AddRoundKey(uint8_t *buf, uint8_t *key);

		//逆向行移位
		void ShiftRows_Inv(uint8_t *buf);
		//逆向字节代替
		void SubBytes_Inv(uint8_t *buf);
		//解密密钥扩展
		void ExpandDecKey(uint8_t *k, uint8_t *rc);
		//逆向列混淆
		void MixColumns_Inv(uint8_t *buf);


		//伽罗瓦域上的计算
	protected:

//s盒计算
#ifndef BACK_TO_TABLES
		
		//s盒中取相应的值
		uint8_t RJ_SBox(uint8_t x);
		//计算在伽罗瓦域上的乘逆
		uint8_t GF_MulInv(uint8_t x); // calculate multiplicative inverse
		//在GF域上计算对数
		uint8_t GF_Log(uint8_t x); // calculate logarithm gen 3
		//在GF域上计算对数的反函数
		uint8_t GF_ALog(uint8_t x); // calculate anti-logarithm gen 3
		//s盒的逆中取相应的值
		uint8_t RJ_SBox_Inv(uint8_t x);
#endif
		//求指数
		uint8_t RJ_xTime(uint8_t x);



	protected:
		aes256_context m_stAesKey;			//保留
		aes256_context m_stBackAesKey;		//用于加解密
	};


class CAesDataGroup :public CAes
{
	NO_COPY_NEED_RVALUE(CAesDataGroup);
	HEAP_CREATE_NEED_CLEANS_DEFINE(CAesDataGroup);
public:
	CAesDataGroup(bool bGenerateKey);
	CAesDataGroup(uint8_t *key);

	//加密解密
public:
	//加密	需要是128bit 16字节的倍数
	bool Encrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize);
	//加密	需要是128bit 16字节的倍数
	bool Encrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize, bool bHexEncode);
	//解密 需要是 128bit 16字节的倍数
	bool Decrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize);
	//解密 需要是 128bit 16字节的倍数
	bool Decrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize, bool bHexDecode);

	//16进制编码反编码
public:
	//编码（16进制）
	bool HexEncodeData(uint8_t *pucDataSrc, unsigned long ulDataSrcDataLen, uint8_t *pucDataOut, unsigned long *pulDataSizeInLenOut);
	//反编码（16进制）
	bool HexDecodeData(uint8_t *pucDataSrc, unsigned long ulDataSrcDataLen, uint8_t *pucDataOut, unsigned long *pulDataSizeInLenOut);
protected:
	//16进制转十进制
	bool HexToInt(uint8_t *HexData);


};

// #ifdef __cplusplus
// }
// #endif

#endif