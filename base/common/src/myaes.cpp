#include "stdafx.h"
#if !(defined(linux)|defined(__CYGWIN__))
//#include"StdAfx.h"
#endif

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
#include"BaseCode/myaes.h"
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include <BaseCode/CheckNoLog.h>

//////////////////////////////////////////////////////////////////////////

#define AES_KEY_NUM_LEN				10
#define AES_KEY_CHAR_LEN			52
#define AES_KEY_SYMBOL_LEN			33
#define AES_NUM_KIND				0			//数字
#define AES_CHAR_KIND				1			//英文
#define AES_SYMBOL_KIND				2			//字符
#define AES_KEY_KIND_COUNT			3

//key 表

//数值表
static const char AesKeyNumTable[AES_KEY_NUM_LEN]			=	{ '0', '1', '2', '3', '4'
																	, '5', '6', '7', '8', '9' };

//英文表
static const char AesKeyCharTable[AES_KEY_CHAR_LEN]		=	{ 'a', 'b', 'c', 'd', 'e'
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

//其他符号
static const char AesKeySymbolTable[AES_KEY_SYMBOL_LEN]	=	{ ' ', '`', '!', '@', '#', '$', '%', '^'
																	, '&', '*', '(', ')', '-', '=', '~', '_'
																	, '+', '[', ']', '{', '}', ';', '\'','\\'
																	, ':', '"', '|', ',', '.', '/', '<', '>'
																	, '?'};
///////////////////////////////////////////////////////////////////////////


#define F(x)   (((x)<<1) ^ ((((x)>>7) & 1) * 0x1b))
#define FD(x)  (((x) >> 1) ^ (((x) & 1) ? 0x8d : 0))

#define COMBINE_BYTE(High_Bit, Low_Bit)	( (( (High_Bit)&0x0F)<<4)|( (Low_Bit)&0x0F) )		//高四位低四位合并一个字节,0x0F保证只有4位数据

////////////////////////////////////////////////////////////////////
//AES类

CAes::CAes()
{
	DOTRY_B_NL
	ClearKey();
	DOCLASSCATCH_E_NL;
}

CAes::CAes(bool bGenerateKey)
{
	DOTRY_B_NL
	ClearKey();

	if (bGenerateKey)
	{
		CHECK_NL(GenerateKey(NULL));
	}
	DOCLASSCATCH_E1_NL(bGenerateKey);
}

CAes::CAes(uint8_t *key)
{
	DOTRY_B_NL
	ClearKey();

	if (key)
	{
		GenerateKey(key);
	}
	DOCLASSCATCH_E1_NL(key);
}

CAes::CAes(CAes&&rValue)
{
	this->m_stAesKey = rValue.m_stAesKey;
	this->m_stBackAesKey = rValue.m_stBackAesKey;
	memset(&rValue.m_stAesKey, 0, sizeof(rValue.m_stAesKey));
	memset(&rValue.m_stBackAesKey, 0, sizeof(rValue.m_stBackAesKey));
}

CAes::~CAes()
{

}

void CAes::Cleans()
{
	DOTRY_B_NL
		ClearKey();
	DOCLASSCATCH_E_NL;
}

// #define BACK_TO_TABLES
#ifdef BACK_TO_TABLES

const uint8_t AES_SBox[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};
const uint8_t AES_SBoxInv[256] = {
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
	0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
	0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
	0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
	0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
	0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
	0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
	0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
	0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
	0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
	0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
	0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
	0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
	0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
	0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
	0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
	0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

#define RJ_SBox(x)     AES_SBox[(x)]
#define RJ_SBox_Inv(x) AES_SBoxInv[(x)]

#else /* tableless subroutines */


//s盒中取相应的值
uint8_t CAes::RJ_SBox(uint8_t x)
{
	uint8_t y=0, sb=0;

	DOTRY_B_NL
	sb = y = GF_MulInv(x);
	y = (y << 1) | (y >> 7); sb ^= y;  y = (y << 1) | (y >> 7); sb ^= y;
	y = (y << 1) | (y >> 7); sb ^= y;  y = (y << 1) | (y >> 7); sb ^= y;
	DOCATCH_E1_NL(x);

	return (sb ^ 0x63);
}

//计算在伽罗瓦域上的乘逆
uint8_t CAes::GF_MulInv(uint8_t x) // calculate multiplicative inverse
{
	return (x) ? GF_ALog(255 - GF_Log(x)) : 0;
	//return (x) ? gf_alog(AES_ALGORITHM_256_BIT-1 - gf_log(x)) : 0;
}

//在GF域上计算对数
uint8_t CAes::GF_Log(uint8_t x) // calculate logarithm gen 3
{
	uint8_t atb = 1, i = 0, z;

	DOTRY_B_NL
	do {
		if (atb == x) break;
		z = atb; atb <<= 1; if (z & 0x80) atb ^= 0x1b; atb ^= z; //防止左移溢出
	} while (++i > 0);
	DOCATCH_E1_NL(x);
	return i;
}


/*
AES算法是基于GF(2^8)数域的,其中的0x02与其他数相乘时,不大于0x80左移,大于要与0x1b异或,0x1b哪来的
如题,这个0x1b有什么理论依据么?0x80又是以什么作为标准的呢?

你看的很细嘛,其实不用理解那么详细,我跟你说一下
大于不大于0x80其实指的最高位是不是1,因为小于1x80的数只要左移（乘2）就可以了,不涉及到进位,而大于或等于0x80的数,最高位是1,左移就会溢出了,而溢出后的数就不在有限域里了,为了实现进位溢出和仍然停留在有限域里的矛盾就要找个方法
结果就是溢出的时候与0x1b异或,0x1b怎么来的呢,你肯定知道GF（28）的不可约多项式吧,m（x）=x8+x4+x3+x+1,它的二进制就是0x1b
*/

//在GF域上计算对数的反函数
uint8_t CAes::GF_ALog(uint8_t x) // calculate anti-logarithm gen 3
{
	uint8_t atb = 1, z;

	DOTRY_B_NL
	while (x--) { z = atb; atb <<= 1; if (z & 0x80) atb ^= 0x1b; atb ^= z; }
	DOCATCH_E1_NL(x);

	return atb;
}


//s盒的逆中取相应的值
uint8_t CAes::RJ_SBox_Inv(uint8_t x)
{
	uint8_t y=0, sb=0;
	DOTRY_B_NL
	y = x ^ 0x63;
	sb = y = (y << 1) | (y >> 7);
	y = (y << 2) | (y >> 6); sb ^= y; y = (y << 3) | (y >> 5); sb ^= y;
	DOCATCH_E1_NL(x);
	return GF_MulInv(sb);
}

#endif



void CAes::Init(bool bResetKey)
{
	DOTRY_B_NL
	//生成密钥key
	if (bResetKey)
	{
		GenerateKey(NULL);
	}
	else
	{
		GenerateKey(m_stAesKey.key);//只生成一次
	}
	
	uint8_t rcon = 1;

	//密钥扩展
	register uint8_t i;
	for (i = 8; --i;)
		ExpandEncKey(m_stBackAesKey.deckey, &rcon);

	DOCATCH_E1_NL(bResetKey);
}

//生成key
bool CAes::GenerateKey(uint8_t * szKey)
{
	
	char key[LEN_AES_256_KEY] = { 0 };
	DOTRY_B_NL
	//选取3位数字

	if (!szKey)
	{
		srand(U32(time(NULL)));
		int m = 0;
		for (int i = 0; i < LEN_AES_256_KEY; i++)
		{
			//选区种类
			m = rand() % AES_KEY_KIND_COUNT;
			switch (m)
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
	if (szKey)
	{
		iKeyLenCount = LEN_AES_256_KEY;
		memset(szKey, 0, LEN_AES_256_KEY);
		memmove(szKey, key, LEN_AES_256_KEY);
	}
	else
	{
		iKeyLenCount = LEN_AES_256_KEY ;
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

//清除密钥
void CAes::ClearKey()
{
	DOTRY_B_NL
	memset(&m_stAesKey, 0, sizeof(m_stAesKey));
	memset(&m_stBackAesKey, 0, sizeof(m_stBackAesKey));
	DOCATCH_E_NL;
}

//扩展加密密钥
bool CAes::ExpandEncKey(uint8_t *k, uint8_t *rc)
{
	
	register uint8_t i;
	DOTRY_B_NL
	k[0] ^= RJ_SBox(k[29]) ^ (*rc);
	k[1] ^= RJ_SBox(k[30]);
	k[2] ^= RJ_SBox(k[31]);
	k[3] ^= RJ_SBox(k[28]);
	*rc = F(*rc);

	for (i = 4; i < 16; i += 4)  k[i] ^= k[i - 4], k[i + 1] ^= k[i - 3],
		k[i + 2] ^= k[i - 2], k[i + 3] ^= k[i - 1];
	k[16] ^= RJ_SBox(k[12]);
	k[17] ^= RJ_SBox(k[13]);
	k[18] ^= RJ_SBox(k[14]);
	k[19] ^= RJ_SBox(k[15]);

	for (i = 20; i < 32; i += 4) k[i] ^= k[i - 4], k[i + 1] ^= k[i - 3],
		k[i + 2] ^= k[i - 2], k[i + 3] ^= k[i - 1];

	DOCATCH_E2_NL(k, rc);
	return true;
}

//轮密钥加
void CAes::AddRoundKey_cpy(uint8_t *buf, uint8_t *key, uint8_t *cpk)
{
	
	register uint8_t i = 16;
	DOTRY_B_NL
	while (i--)  buf[i] ^= (cpk[i] = key[i]), cpk[16 + i] = key[16 + i];
	DOCATCH_E3_NL(buf, key, cpk);
} /* aes_addRoundKey_cpy */

//字节代替
void CAes::SubBytes(uint8_t *buf)
{
	
	register uint8_t i = 16;
	DOTRY_B_NL
	while (i--) buf[i] = RJ_SBox(buf[i]);

	DOCATCH_E1_NL(buf);
}

//行移位
void CAes::ShiftRows(uint8_t *buf)
{
	
	register uint8_t i, j; /* to make it potentially parallelable :) */
	DOTRY_B_NL
	i = buf[1]; buf[1] = buf[5]; buf[5] = buf[9]; buf[9] = buf[13]; buf[13] = i;
	i = buf[10]; buf[10] = buf[2]; buf[2] = i;
	j = buf[3]; buf[3] = buf[15]; buf[15] = buf[11]; buf[11] = buf[7]; buf[7] = j;
	j = buf[14]; buf[14] = buf[6]; buf[6] = j;
	DOCATCH_E1_NL(buf);
}

//列混淆
void CAes::MixColumns(uint8_t *buf)
{
	
	register uint8_t i, a, b, c, d, e;
	DOTRY_B_NL
	for (i = 0; i < 16; i += 4)
	{
		a = buf[i]; b = buf[i + 1]; c = buf[i + 2]; d = buf[i + 3];
		e = a ^ b ^ c ^ d;
		buf[i] ^= e ^ RJ_xTime(a^b);   buf[i + 1] ^= e ^ RJ_xTime(b^c);
		buf[i + 2] ^= e ^ RJ_xTime(c^d); buf[i + 3] ^= e ^ RJ_xTime(d^a);
	}
	DOCATCH_E1_NL(buf);
}

//轮密码加
void CAes::AddRoundKey(uint8_t *buf, uint8_t *key)
{
	
	register uint8_t i = 16;
	DOTRY_B_NL
	while (i--) buf[i] ^= key[i];
	DOCATCH_E2_NL(buf, key);
}

//逆向行移位
void CAes::ShiftRows_Inv(uint8_t *buf)
{
	
	register uint8_t i, j; /* same as above :) */
	DOTRY_B_NL
	i = buf[1]; buf[1] = buf[13]; buf[13] = buf[9]; buf[9] = buf[5]; buf[5] = i;
	i = buf[2]; buf[2] = buf[10]; buf[10] = i;
	j = buf[3]; buf[3] = buf[7]; buf[7] = buf[11]; buf[11] = buf[15]; buf[15] = j;
	j = buf[6]; buf[6] = buf[14]; buf[14] = j;
	DOCATCH_E1_NL(buf);
}

//逆向字节代替
void CAes::SubBytes_Inv(uint8_t *buf)
{
	register uint8_t i = 16;
	DOTRY_B_NL
	while (i--) buf[i] = RJ_SBox_Inv(buf[i]);
	DOCATCH_E1_NL(buf);
}

//解密密钥扩展
void CAes::ExpandDecKey(uint8_t *k, uint8_t *rc)
{
	uint8_t i;
	DOTRY_B_NL
	for (i = 28; i > 16; i -= 4) k[i + 0] ^= k[i - 4], k[i + 1] ^= k[i - 3],
		k[i + 2] ^= k[i - 2], k[i + 3] ^= k[i - 1];

	k[16] ^= RJ_SBox(k[12]);
	k[17] ^= RJ_SBox(k[13]);
	k[18] ^= RJ_SBox(k[14]);
	k[19] ^= RJ_SBox(k[15]);

	for (i = 12; i > 0; i -= 4)  k[i + 0] ^= k[i - 4], k[i + 1] ^= k[i - 3],
		k[i + 2] ^= k[i - 2], k[i + 3] ^= k[i - 1];

	*rc = FD(*rc);
	k[0] ^= RJ_SBox(k[29]) ^ (*rc);
	k[1] ^= RJ_SBox(k[30]);
	k[2] ^= RJ_SBox(k[31]);
	k[3] ^= RJ_SBox(k[28]);
	DOCATCH_E2_NL(k, rc);
}

//逆向列混淆
void CAes::MixColumns_Inv(uint8_t *buf)
{
	register uint8_t i, a, b, c, d, e, x, y, z;
	DOTRY_B_NL
	for (i = 0; i < 16; i += 4)
	{
		a = buf[i]; b = buf[i + 1]; c = buf[i + 2]; d = buf[i + 3];
		e = a ^ b ^ c ^ d;
		z = RJ_xTime(e);
		x = e ^ RJ_xTime(RJ_xTime(z^a^c));  y = e ^ RJ_xTime(RJ_xTime(z^b^d));
		buf[i] ^= x ^ RJ_xTime(a^b);   buf[i + 1] ^= y ^ RJ_xTime(b^c);
		buf[i + 2] ^= x ^ RJ_xTime(c^d); buf[i + 3] ^= y ^ RJ_xTime(d^a);
	}

	DOCATCH_E1_NL(buf);
}


//求指数
uint8_t CAes::RJ_xTime(uint8_t x)
{
	return (x & 0x80) ? ((x << 1) ^ 0x1b) : (x << 1);
}


//加密模块
void CAes::Encrypt_ecb(uint8_t *buf)
{
	if (!buf) return;
	DOTRY_B_NL
	//密钥初始化
	Init(false);

	uint8_t i, rcon;

	//轮密钥拷贝
	AddRoundKey_cpy(buf, m_stBackAesKey.enckey, m_stBackAesKey.key);
	for (i = 1, rcon = 1; i < 14; ++i)
	{
		//字节代替
		SubBytes(buf);

		//正向行移位
		ShiftRows(buf);

		//正向列混淆
		MixColumns(buf);


		if (i & 1) AddRoundKey(buf, &m_stBackAesKey.key[16]);
		else ExpandEncKey(m_stBackAesKey.key, &rcon), AddRoundKey(buf, m_stBackAesKey.key);
	}
	SubBytes(buf);
	ShiftRows(buf);
	ExpandEncKey(m_stBackAesKey.key, &rcon);
	AddRoundKey(buf, m_stBackAesKey.key);

	DOCATCH_E1_NL(buf);
}

//解密模块 只解密一组128bit数据 且buf大小必须是128位的整数倍
void CAes::Decrypt_ecb(uint8_t *buf)
{
	if (!buf) return;
	DOTRY_B_NL
	//密钥初始化
	Init(false);

	uint8_t i, rcon;

	//与加密的相反

	//轮密钥拷贝
	AddRoundKey_cpy(buf, m_stBackAesKey.deckey, m_stBackAesKey.key);
	//逆向行移位
	ShiftRows_Inv(buf);
	//逆向字节代替
	SubBytes_Inv(buf);

	for (i = 14, rcon = 0x80; --i;)
	{
		if ((i & 1))
		{
			//解密密钥扩展
			ExpandDecKey(m_stBackAesKey.key, &rcon);
			//轮密钥加
			AddRoundKey(buf, &m_stBackAesKey.key[16]);
		}
		else AddRoundKey(buf, m_stBackAesKey.key);

		//逆向列混淆
		MixColumns_Inv(buf);

		//逆向行移位
		ShiftRows_Inv(buf);
		//逆向字节代替
		SubBytes_Inv(buf);
	}
	AddRoundKey(buf, m_stBackAesKey.key);
	DOCATCH_E1_NL(buf);
}


///分组加密
CAesDataGroup::CAesDataGroup()
:CAes()
{
}

CAesDataGroup::CAesDataGroup(bool bGenerateKey)
: CAes(bGenerateKey)
{
}

CAesDataGroup::CAesDataGroup(uint8_t *key)
:CAes(key)
{
}

CAesDataGroup::~CAesDataGroup()
{
}

CAesDataGroup::CAesDataGroup(CAesDataGroup&& rValue)
{
	m_stAesKey = rValue.m_stAesKey;
	m_stBackAesKey = rValue.m_stBackAesKey;
	memset(&rValue.m_stAesKey, 0, sizeof(rValue.m_stAesKey));
	memset(&rValue.m_stBackAesKey, 0, sizeof(rValue.m_stBackAesKey));
}

void CAesDataGroup::Cleans()
{
}

//加密	需要是128bit 16字节的倍数
bool CAesDataGroup::Encrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize)
{
	//参数校验
	if (!buf || ulBufSize == 0 || ulBufSize % 16 != 0 || !BufOut || ulBufOutSize == 0 || ulBufOutSize % 16 != 0 || ulBufOutSize<ulBufSize) return false;
	//密钥不为空
	if (m_stBackAesKey.key[0] == 0) return false;

	DOTRY_B_NL

	if (buf != BufOut)
	{
		memset(BufOut, 0, ulBufOutSize);
		memmove(BufOut, buf, ulBufSize);
	}

	uint8_t *pBuffer = BufOut;
	int iCount = ulBufSize / 16;
	for (int i = 0; i < iCount; i++)
	{
		//一次加密16字节
		Encrypt_ecb(pBuffer);
		pBuffer += 16;
	}

	DOCATCH_E4_NL(buf, ulBufSize, BufOut, ulBufOutSize);

	return true;
}

//加密	需要是128bit 16字节的倍数
bool CAesDataGroup::Encrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize, bool bHexEncode)
{
	//参数校验
	if (!buf || ulBufSize == 0 || ulBufSize % 16 != 0 || !BufOut || ulBufOutSize == 0 || ulBufOutSize % 16 != 0 || ulBufOutSize<(2 * ulBufSize)) return false;
	//密钥不为空
	if (m_stBackAesKey.key[0] == 0) return false;

	DOTRY_B_NL

	//加密
	if (!Encrypt_Data(buf, ulBufSize, BufOut, ulBufOutSize)) return false;

	//编码
	if (bHexEncode)
	{
		if (!HexEncodeData(BufOut, ulBufSize, BufOut, &ulBufOutSize)) return false;
	}

	DOCATCH_E5_NL(buf, ulBufSize, BufOut, ulBufOutSize, bHexEncode);
	return true;
}

//解密 需要是 128bit 16字节的倍数
bool CAesDataGroup::Decrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize)
{
	//参数校验
	if (!buf || ulBufSize == 0 || ulBufSize % 16 != 0 || !BufOut || ulBufOutSize == 0 || ulBufOutSize % 16 != 0 || ulBufOutSize<ulBufSize) return false;
	//密钥不为空
	if (m_stBackAesKey.key[0] == 0) return false;

	DOTRY_B_NL

	if (buf != BufOut)
	{
		memset(BufOut, 0, ulBufOutSize);
		memmove(BufOut, buf, ulBufSize);
	}

	uint8_t *pBuffer = BufOut;
	int iCount = ulBufSize / 16;
	for (int i = 0; i < iCount; i++)
	{
		//一次加密16字节
		Decrypt_ecb(pBuffer);
		pBuffer += 16;
	}

	DOCATCH_E4_NL(buf, ulBufSize, BufOut, ulBufOutSize);
	return true;
}

//解密 需要是 128bit 16字节的倍数
bool CAesDataGroup::Decrypt_Data(uint8_t *buf, unsigned long ulBufSize, uint8_t *BufOut, unsigned long ulBufOutSize, bool bHexDecode)
{
	//参数校验
	if (!buf || ulBufSize == 0 || ulBufSize % 16 != 0 || !BufOut || ulBufOutSize == 0 || ulBufOutSize % 16 != 0 || ulBufOutSize<ulBufSize) return false;
	//密钥不为空
	if (m_stBackAesKey.key[0] == 0) return false;

	DOTRY_B_NL

	//解码
	if (BufOut != buf)
	{
		memset(BufOut, 0, ulBufOutSize);
		memmove(BufOut, buf, ulBufOutSize);		
	}
	unsigned long ulBufOutSizeTmp = 0;

	if (bHexDecode)
	{
		ulBufOutSizeTmp = ulBufOutSize;
		if (!HexDecodeData(BufOut, ulBufSize, BufOut, &ulBufOutSizeTmp)) return false;
	}
	else
	{
		ulBufOutSizeTmp = ulBufSize;
	}

	//解密
	if (!Decrypt_Data(BufOut, ulBufOutSizeTmp, BufOut, ulBufOutSize)) return false;

	DOCATCH_E5_NL(buf, ulBufSize, BufOut, ulBufOutSize, bHexDecode);

	return true;
}

//编码（16进制）
bool CAesDataGroup::HexEncodeData(uint8_t *pucDataSrc, unsigned long ulDataSrcDataLen, uint8_t *pucDataOut, unsigned long *pulDataSizeInLenOut)
{
	//参数校验
	if (!pucDataSrc || !pucDataOut || !pulDataSizeInLenOut) return false;
	if (*pulDataSizeInLenOut < 2 * ulDataSrcDataLen)
	{
		*pulDataSizeInLenOut = 2 * ulDataSrcDataLen;
		return false;
	}

	DOTRY_B_NL

	//缓冲区
	unsigned long n = 0;
	unsigned char ucCache[3] = { 0 };
	unsigned char *ucDataCache = new unsigned char[2 * ulDataSrcDataLen];
	CHECKF_NL(ucDataCache);
	memset((void *)ucDataCache, 0, 2 * ulDataSrcDataLen);

	unsigned char *pDataCacheTmp = ucDataCache;
	unsigned char *pDataSrcTmp = pucDataSrc;

	//编码
	for (n = 0; n < ulDataSrcDataLen; n++)
	{
		memset(ucCache, 0, 3);
		sprintf((char *)ucCache, "%02X", *pDataSrcTmp);
		memmove(pDataCacheTmp, ucCache, 2);
		pDataSrcTmp++;
		pDataCacheTmp += 2;
	}

	//数据迁移并释放空间
	memset(pucDataOut, 0, *pulDataSizeInLenOut);
	memmove(pucDataOut, ucDataCache, 2 * ulDataSrcDataLen);
	char miwend[8192] = { 0 };
	memmove(miwend, ucDataCache, 2 * ulDataSrcDataLen);
	*pulDataSizeInLenOut = 2 * ulDataSrcDataLen;
	delete[]ucDataCache;

	DOCATCH_E4_NL(pucDataSrc, ulDataSrcDataLen, pucDataOut, pulDataSizeInLenOut);

	return true;
}

//反编码（16进制）
bool CAesDataGroup::HexDecodeData(uint8_t *pucDataSrc, unsigned long ulDataSrcDataLen, uint8_t *pucDataOut, unsigned long *pulDataSizeInLenOut)
{
	//参数校验
	if (!pucDataSrc || !pucDataOut || !pulDataSizeInLenOut) return false;

	DOTRY_B_NL

	unsigned char strtmp[3] = { 0 };
	int strcount = ((ulDataSrcDataLen) % 2 == 0 ? (ulDataSrcDataLen) / 2 : ((ulDataSrcDataLen) / 2 + 1));
	char *decodetmp = new char[strcount];
	CHECKF_NL(decodetmp);
	memset(decodetmp, 0, strcount);

	if (*pulDataSizeInLenOut <  U32(strcount))
	{
		*pulDataSizeInLenOut = U32(strcount);
		delete[]decodetmp;
		return false;
	}

	//转换
	for (int i = 0; i<strcount; i++)
	{
		memset(strtmp, 0, 3);
		memmove(strtmp, pucDataSrc + 2 * i, 2);
		if (!HexToInt(&strtmp[0]))
		{
			delete[]decodetmp;
			return false;
		}
			
		if (!HexToInt(&strtmp[1]))
		{
			delete[]decodetmp;
			return false;
		}
		decodetmp[i] = COMBINE_BYTE(strtmp[0], strtmp[1]);
	}

	//转移数据
	memset(pucDataOut, 0, *pulDataSizeInLenOut);
	memmove(pucDataOut, decodetmp, strcount);
	*pulDataSizeInLenOut = strcount;

	delete[]decodetmp;

	DOCATCH_E4_NL(pucDataSrc, ulDataSrcDataLen, pucDataOut, pulDataSizeInLenOut);

	return true;
}

//16进制转十进制
bool CAesDataGroup::HexToInt(uint8_t *HexData)
{
	//参数校验
	if (!HexData) return false;

	DOTRY_B_NL

	if (*HexData >= '0'&&*HexData <= '9')
	{
		*HexData = *HexData - '0' + 0;
	}
	else if (*HexData >= 'A'&&*HexData <= 'F')
	{
		*HexData = *HexData - 'A' + 10;
	}
	else if (*HexData >= 'a'&&*HexData <= 'f')
	{
		*HexData = *HexData - 'a' + 10;
	}
	else
	{
		return false;
	}

	DOCATCH_E1_NL(HexData);

	return true;
}
