#include<BaseCode/Win/Rsa.h>
#include<BaseCode/Win/IniFileIO.h>
#include<stdio.h>
#pragma comment(lib, "Crypt32")	//加解密时用到
#define COMBINE_BYTE(High_Bit, Low_Bit)	((High_Bit&U64(0x0F))<<4)|(Low_Bit&0x0F)		//高四位低四位合并一个字节,0x0F保证只有4位数据

gtool::CRsa::CRsa()
{
	m_hCryptProv = NULL;
	m_hPublicKey = NULL;
	m_hPrivateKey = NULL;
	memset(m_szCspName, 0, sizeof(TCHAR)* RSA_CONTAINER_NAME_LEN);
	memmove(m_szCspName, CSP_CONTAINER_NAME, sizeof(TCHAR)* lstrlen(CSP_CONTAINER_NAME));
	memset(&m_stPublicKey, 0, S_KEY_T);
	memset(&m_stPrivateKey, 0, S_KEY_T);
}

gtool::CRsa::~CRsa()
{
	Cleans();
}

void gtool::CRsa::Cleans()
{
}

bool gtool::CRsa::Encrypte()
{

}

bool gtool::CRsa::Decrypte()
{

}

bool gtool::CRsa::Init(const char *szKeyFile /*= NULL*/, const char *szMainKeyName /*= NULL*/)
{
	//容器已存在
	if (m_hCryptProv) return true;

	//打开或创建CSP容器
	IF_NOT_NL(CryptAcquireContext(&m_hCryptProv, m_szCspName, NULL, PROV_RSA_FULL, 0))//默认设置为0，获取出错则新建
	{
		if (GetLastError() == NTE_BAD_KEYSET)//容器不能被打开
		{
			CHECKF_NL(CryptAcquireContext(&m_hCryptProv, m_szCspName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET));//使用指定名容器新建
		}
		else
		{
			return false;
		}
	}

	//是否使用已存在key
	if (bUseExistKeyPairs)
	{
		//已有密钥对导入
		CHECKF_NL(ReadPublicPrivateKey(szKeyFile, szKeyFile));
	}
	else
	{
		//生成密钥对
		CHECKF_NL(!GenerateKeyPairs());

		//写入文件
		ASSERT(WritePublicPrivateKey(szPublicKeyFile, szPrivateKeyFile));
	}

	//导入密钥
	CHECKF_NL(ImportKeyPairs());

	return true;
}

bool gtool::CRsa::ImportKeyPairs()
{
	if (!m_hCryptProv) return false;

	bool bImportSuccess = false;

	//导入public key
	if (m_stPublicKey.ulKeyLen != 0)
	{
		IF_NOT_NL(CryptImportKey(m_hCryptProv, (BYTE *)m_stPublicKey.ucKey, m_stPublicKey.ulKeyLen, 0, 0, &m_hPublicKey))
		{
			//TRACE("CryptImportKey error 0x%x\n", GetLastError());
			return false;
		}

		bImportSuccess = true;
	}

	//导入Private key
	if (m_stPrivateKey.ulKeyLen != 0)
	{
		IF_NOT_NL(CryptImportKey(m_hCryptProv, (BYTE *)m_stPrivateKey.ucKey, m_stPrivateKey.ulKeyLen, 0, 0, &m_hPrivateKey))
		{
			//TRACE("CryptImportKey error 0x%x\n", GetLastError());
			return false;
		}

		bImportSuccess = true;
	}

	return bImportSuccess;
}

//构造
CRsaEngine::CRsaEngine()
{
	m_hCryptProv = NULL;
	m_hPublicKey = NULL;
	m_hPrivateKey = NULL;
	memset(m_szCspName, 0, sizeof(TCHAR)* RSA_CONTAINER_NAME_LEN);
	memmove(m_szCspName, CSP_CONTAINER_NAME, sizeof(TCHAR)* lstrlen(CSP_CONTAINER_NAME));
	memset(&m_stPublicKey, 0, S_KEY_T);
	memset(&m_stPrivateKey, 0, S_KEY_T);
}

CRsaEngine::~CRsaEngine()
{
}

//加密并编码
bool CRsaEngine::Encrypte(const unsigned char *plaintext, const unsigned long ulPlainTextLen, unsigned char *&pCipherTextOut, unsigned long& pCipherSizeInLenOut, bool bFinal)
{
	CHECKF_NL(plaintext&&strlen((char*)plaintext) > 0);

	if (!CSPInit(true)) return false;

	if (!m_hPublicKey)
	{
		Release();
		return FALSE;
	}
	unsigned long ulCipherSize = *pCipherSizeInLenOut;

	//加密
	if (!EncrypteData(plaintext, ulPlainTextLen, pCipherTextOut, &ulCipherSize, bFinal))
	{
		Release();
		return FALSE;
	}

	//编码输出
	if (!HexEncodeData(*pCipherTextOut, ulCipherSize, *pCipherTextOut, pCipherSizeInLenOut))
	{
		Release();
		return FALSE;
	}

	Release();
	return TRUE;
}

//解码并解密
bool CRsaEngine::Decrypte(unsigned char *ciphertext, unsigned long cipherlen, unsigned char **plaintext, unsigned long *pPlainSizeInLenOut, bool bFinal)
{
	if (!HexDecodeData(ciphertext, cipherlen, ciphertext, &cipherlen)) return FALSE;

	if (!CSPInit(true)) return FALSE;

	if (!DecrypteData(ciphertext, cipherlen, plaintext, pPlainSizeInLenOut, bFinal))
	{
		Release();
		return FALSE;
	}
	Release();

	return TRUE;
}



//加密数据
bool CRsaEngine::EncrypteData(unsigned char *plaintext, unsigned long ulPlainTextLen, unsigned char **pCipherTextOut, unsigned long *pCipherSizeInLenOut, bool bFinal)
{
	if (!pCipherTextOut || !plaintext || !pCipherSizeInLenOut || (*pCipherSizeInLenOut <= 0)) return FALSE;

	if (!m_hPublicKey) return FALSE;

	DWORD lEncryptLen = ulPlainTextLen;	//必须是明文长度，

	// 获得加密后的数据长度
	if (!CryptEncrypt(m_hPublicKey, NULL, bFinal, 0, NULL, &lEncryptLen, 0))//加密lEncryptLen数据所需的字节
	{
		//TRACE("CryptEncrypt error 0x%x\n", GetLastError());
		return FALSE;
	}

	// Create a buffer for encrypted data
	//pbEncrypted = (BYTE*)realloc(pbEncrypted, dwEncryptedLen);
	unsigned char *strCache = new unsigned char[lEncryptLen];
	if (!strCache) return FALSE;
	memset(strCache, 0, lEncryptLen);
	memmove(strCache, plaintext, ulPlainTextLen);

	DWORD dwOutCacheSize = ulPlainTextLen;

	// Encrypt data
	if (!CryptEncrypt(m_hPublicKey, NULL, bFinal, 0, strCache, &dwOutCacheSize, lEncryptLen))//加密dwOutCacheSize字节数据，缓冲区大小lEncryptLen
	{
		//TRACE("CryptEncrypt error 0x%x\n", GetLastError());
		return FALSE;
	}

	if (*pCipherSizeInLenOut < dwOutCacheSize)
	{
		*pCipherSizeInLenOut = dwOutCacheSize;
		delete[]strCache;
		return FALSE;
	}

	memset((char *)(*pCipherTextOut), 0, *pCipherSizeInLenOut);

	memmove((char *)*pCipherTextOut, strCache, dwOutCacheSize);
	*pCipherSizeInLenOut = dwOutCacheSize;

	delete[]strCache;

	return TRUE;
}

//解密数据
bool CRsaEngine::DecrypteData(unsigned char *ciphertext, unsigned long cipherlen, unsigned char **plaintext, unsigned long *pPlainSizeInLenOut, bool bFinal)
{
	if (!ciphertext || !plaintext || !pPlainSizeInLenOut || (*pPlainSizeInLenOut <= 0)) return FALSE;
	if (!m_hPrivateKey) return FALSE;

	//备份密文
	unsigned char *pCipherText = new unsigned char[cipherlen];
	if (!pCipherText) return FALSE;
	memset(pCipherText, 0, cipherlen);
	memmove(pCipherText, ciphertext, cipherlen);

	// 解密后覆盖原数据信息
	if (!CryptDecrypt(m_hPrivateKey, NULL, bFinal, 0, pCipherText, &cipherlen))
	{
		delete[]pCipherText;

		//TRACE("CryptDecrypt error 0x%x\n", GetLastError());
		return FALSE;
	}

	if (*pPlainSizeInLenOut < cipherlen)
	{
		delete[]pCipherText;
		*pPlainSizeInLenOut = cipherlen;
		return FALSE;
	}

	memset(*plaintext, 0, *pPlainSizeInLenOut);
	memmove(*plaintext, pCipherText, cipherlen);

	*pPlainSizeInLenOut = cipherlen;


	//恢复密文清理空间
	delete[]pCipherText;

	return TRUE;
}

//编码（16进制）
bool CRsaEngine::HexEncodeData(unsigned char *pucDataSrc, unsigned long ulDataSrcDataLen, unsigned char *pucDataOut, unsigned long *pulDataSizeInLenOut)
{
	//参数校验
	if (!pucDataSrc || !pucDataOut || !pulDataSizeInLenOut) return FALSE;
	if (*pulDataSizeInLenOut < 2 * ulDataSrcDataLen)
	{
		*pulDataSizeInLenOut = 2 * ulDataSrcDataLen;
		return FALSE;
	}

	//缓冲区
	unsigned long n = 0;
	unsigned char ucCache[3] = { 0 };
	unsigned char *ucDataCache = new unsigned char[2 * ulDataSrcDataLen];
	if (!ucDataCache) return FALSE;
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

	return TRUE;
}

//反编码（16进制）
bool CRsaEngine::HexDecodeData(unsigned char *pucDataSrc, unsigned long ulDataSrcDataLen, unsigned char *pucDataOut, unsigned long *pulDataSizeInLenOut)
{
	//参数校验
	if (!pucDataSrc || !pucDataOut || !pulDataSizeInLenOut) return FALSE;

	unsigned char strtmp[3] = { 0 };
	int strcount = ((ulDataSrcDataLen) % 2 == 0 ? (ulDataSrcDataLen) / 2 : ((ulDataSrcDataLen) / 2 + 1));
	char *decodetmp = new char[strcount];
	if (!decodetmp) return FALSE;
	memset(decodetmp, 0, strcount);

	if (*pulDataSizeInLenOut <  strcount)
	{
		*pulDataSizeInLenOut = strcount;
		delete[]decodetmp;
		return FALSE;
	}

	//转换
	for (int i = 0; i<strcount; i++)
	{
		memset(strtmp, 0, 3);
		memmove(strtmp, pucDataSrc + 2 * i, 2);
		if (!HexToInt(&strtmp[0]))
			return false;
		if (!HexToInt(&strtmp[1]))
			return false;
		decodetmp[i] = COMBINE_BYTE(strtmp[0], strtmp[1]);
	}

	//转移数据
	memset(pucDataOut, 0, *pulDataSizeInLenOut);
	memmove(pucDataOut, decodetmp, strcount);
	*pulDataSizeInLenOut = strcount;

	delete[]decodetmp;

	return TRUE;
}

//16进制转十进制
bool CRsaEngine::HexToInt(unsigned char *HexData)
{
	//参数校验
	if (!HexData) return FALSE;

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
		return FALSE;
	}

	return TRUE;
}

//Csp初始化（初始化容器，导入密钥（公钥和私钥），或者生成密钥）
bool CRsaEngine::CSPInit(bool bUseExistKeyPairs, const char * szPublicKeyFile, const char * szPrivateKeyFile)
{
	//容器已存在
	if (m_hCryptProv) return true;

	//打开或创建CSP容器
	IF_NOT_NL(CryptAcquireContext(&m_hCryptProv, m_szCspName, NULL, PROV_RSA_FULL, 0))//默认设置为0，获取出错则新建
	{
		if (GetLastError() == NTE_BAD_KEYSET)//容器不能被打开
		{
			CHECKF_NL(CryptAcquireContext(&m_hCryptProv, m_szCspName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET));//使用指定名容器新建
		}
		else
		{
			return false;
		}
	}

	//是否使用已存在key
	if (bUseExistKeyPairs)
	{
		//已有密钥对导入
		CHECKF_NL(ReadPublicPrivateKey(szPublicKeyFile, szPrivateKeyFile));
	}
	else
	{
		//生成密钥对
		CHECKF_NL (!GenerateKeyPairs());

		//写入文件
		ASSERT(WritePublicPrivateKey(szPublicKeyFile, szPrivateKeyFile));
	}

	//导入密钥
	CHECKF_NL(ImportKeyPairs());

	return true;
}

//设置公钥 经过编码的
bool CRsaEngine::SetPublicKey(unsigned char *ucKey)
{
	m_stPublicKey.ulKeyLen = RSA_KEY_STR_LEN;

	//反编码
	CHECKF_NL(HexDecodeData(ucKey, strlen((char *)ucKey), (m_stPublicKey.ucKey), &m_stPublicKey.ulKeyLen));

	return TRUE;
}

//设置私钥 经过编码的
bool CRsaEngine::SetPrivateKey(unsigned char *ucKey)
{
	m_stPrivateKey.ulKeyLen = RSA_KEY_STR_LEN;

	//反编码
	if (!HexDecodeData(ucKey, strlen((char *)ucKey), (m_stPrivateKey.ucKey), &m_stPrivateKey.ulKeyLen)) return FALSE;
}

//签名	20170323	shy	128字节
std::string CRsaEngine::SignatureData(char * strData, long lDataSize)
{
	//校验参数
	if (!strData || lDataSize <= 0 || lDataSize >= 128) return "";
	//if (strData.empty()) return "";

	string sRet("");

	HCRYPTPROV hProv = NULL;	//容器
	HCRYPTKEY hKey = NULL;		//密钥句柄
	HCRYPTHASH hHash = NULL;			//哈希句柄

	if (!ImportRsaSignatureKey(&hProv, &hKey, szYCSignaturePubKey, false))
	{
		printf("ImportRsaKey for public key failed with error 0x%.8X\n", GetLastError());
		goto Exit;
	}

	BYTE bData[128];
	ZeroMemory(bData, sizeof(BYTE)* 128);

	//2016-05-05 modified by songhuiya
	//memcpy_s(bData,sizeof(BYTE)*128, strSrc.c_str(), strSrc.size());
	memmove(bData, strData, lDataSize);
	//////////////////////////////////////////////////////////
	DWORD dwDataLen = (DWORD)lDataSize;

	//创建哈希句柄，指定哈希算法，这里采用CALG_SHA1,即sha1算法
	if (CryptCreateHash(
		hProv,
		CALG_SHA1,
		0,
		0,
		&hHash))
	{
		printf("创建哈希句柄成功。\n");
	}
	else
	{
		goto Exit;
		//HandleError("调用CryptCreateHash失败.");
	}

	//--------对数据计算哈希获得摘要-----------------------------------
	if (CryptHashData(
		hHash,
		bData, //对输入的数据进行hash运算,保存输出后的hash摘要数据
		dwDataLen,
		0))
	{
		printf("计算哈希成功\n");
	}
	else
	{
		goto Exit;
		//HandleError("调用CryptHashData失败。");
	}

	//--------对摘要计算签名，第一次调用获得签名后数据的长度-----------
	DWORD dwSigLen = 0;
	BYTE *pbSignature = NULL;  //保存签名值的缓冲区指针
	if (CryptSignHash(
		hHash,
		AT_SIGNATURE,
		NULL,
		0,
		NULL,
		&dwSigLen))
	{
		printf("签名值的长度为%d \n", dwSigLen);
	}
	else
	{
		goto Exit;
		//HandleError("调用CryptSignHash失败。");
	}
	if (pbSignature = (BYTE*)malloc(dwSigLen))
	{
		;
	}
	else
	{
		goto Exit;
		//HandleError("内存不够啦。");
	}

	//-----------对hash对象签名------------------------------------------
	if (CryptSignHash(
		hHash,
		AT_SIGNATURE,
		NULL,
		0,
		pbSignature,
		&dwSigLen))
	{
		printf("数据签名成功！\n");
	}
	else
	{
		printf("调用cryptSignHash失败。！");
		goto Exit;
	}


	//------------输出签名数据-------------------------------------------
	printf("签名值:\n");
	for (int i = 0; i<dwSigLen; i++)
	{
		if ((i == 0) && (i != 0))
		{
			printf("\n");
		}
		printf("%2.2x", pbSignature[i]);
	}
	printf("\n");
	printf("签名成功。长度：%d\n\n", dwSigLen * 2);

	//---------释放资源--------------



	/*
	*  2016-04-21 Added by rinmb
	*  字节翻转（为兼容其他平台的RSA算法，openssl、java、c#）
	*  因本系统只需兼容C++及C#，所以可去掉这个处理.
	*  在C#端，解密时也需要先翻转后才能正常解密.
	*/
	//char c = 0;
	//   for (int i = 0; i < (dwDataLen / 2); i++) 
	//   {
	//       c = bData[i];
	//       bData[i] = bData[dwDataLen - 1 - i];
	//       bData[dwDataLen - 1 - i] = c;
	//   }

	char *pDataCache = new char[dwSigLen * 2 + 1];
	if (!pDataCache) return "";
	memset(pDataCache, 0, dwSigLen * 2 + 1);
	unsigned long lDataCache = dwSigLen * 2 + 1;
	HexEncodeData(pbSignature, dwSigLen, (unsigned char *)pDataCache, &lDataCache);
	sRet = pDataCache;
	if (pDataCache) delete[]pDataCache;

Exit:
	//释放资源
	if (pbSignature) free(pbSignature);
	//销毁哈希对象
	if (hHash)
		CryptDestroyHash(hHash);
	if (hKey) CryptDestroyKey(hKey);
	if (hProv) CryptReleaseContext(hProv, 0);

	return sRet;
}

//验签	20170323	shy 128字节
bool CRsaEngine::VerifySignature(char * strUnSignData, long lUnSignDataSize, char * strSignData, long lSignDataSize)
{
	//校验参数
	if (!strUnSignData || lUnSignDataSize <= 0 || !strSignData || lSignDataSize <= 0 || lUnSignDataSize >= 128 || lSignDataSize >= 128) return false;
	//if (strData.empty()) return "";

	string sRet("");

	HCRYPTPROV hProv = NULL;	//容器
	HCRYPTKEY hKey = NULL;		//密钥句柄
	HCRYPTHASH hHash = NULL;			//哈希句柄

	if (!ImportRsaSignatureKey(&hProv, &hKey, szYCSignaturePubKey, false))
	{
		printf("ImportRsaKey for private key failed with error 0x%.8X\n", GetLastError());
		goto Exit;
	}

	BYTE bData[128];
	ZeroMemory(bData, sizeof(BYTE)* 128);
	unsigned long lDataSize = 128 * sizeof(BYTE);
	int nRetVal = HexDecodeData((unsigned char *)strSignData, lSignDataSize, bData, &lDataSize);
	if (nRetVal != 0) return false;
	DWORD dwDataLen = lSignDataSize / 2;
	printf("dwDatalen:%d, \r\n%s\r\n", dwDataLen, strSignData);

	BYTE bUnsignData[128] = { 0 };
	memmove(bUnsignData, strUnSignData, lUnSignDataSize);
	//sprintf((char *)bUnsignData, "%s", strUnSignData.c_str());
	DWORD dwUnsignDataLen = lUnSignDataSize;



	//--------------创建hash对象--------------------------------------------
	if (CryptCreateHash(
		hProv,
		CALG_SHA1,
		0,
		0,
		&hHash))
	{
		printf("创建哈希对象成功 \n");
	}
	else
	{
		printf("调用CryptCreateHash失败\r\n");
		goto Exit;
	}

	//--------------计算哈希-----------------------------------
	if (CryptHashData(
		hHash,
		bUnsignData,
		dwUnsignDataLen,
		0))
	{
		printf("数据哈希完成.\n");
	}
	else
	{
		printf("调用CryptHashData失败\r\n");
		goto Exit;
	}

	//--------------验证签名-----------------------------------
	bool bRet = false;
	if (bRet = CryptVerifySignature(
		hHash,
		bData,
		dwDataLen,
		hKey,
		NULL,
		0))
	{
		printf("验证签名成功。\n");
	}
	else
	{
		printf("签名验证失败，签名无效0x%x\r\n", GetLastError());
		goto Exit;
		//HandleError("签名验证失败，签名无效");
	}

Exit:
	if (hHash) CryptDestroyHash(hHash);
	if (hKey) CryptDestroyKey(hKey);
	if (hProv) CryptReleaseContext(hProv, 0);

	return bRet;
}

//生成签名密钥对	20170323	shy
bool CRsaEngine::GenRsaSignatureKeyPairs(bool bGenerateKeyFile)	//禁用功能
{
	HCRYPTKEY key = NULL;
	HCRYPTPROV provider = NULL;

	DWORD pubKeyLen = 0;
	BYTE* pbPubKey = NULL;

	DWORD privKeyLen = 0;
	BYTE* pbPrivKey = NULL;

	// 1.生成RSA句柄
	if (!CryptAcquireContext(&provider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
	{
		if (GetLastError() == NTE_BAD_KEYSET)
		{
			if (!CryptAcquireContext(&provider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	if (!provider)
		return false;

	// 2.生成密钥对
	if (!CryptGenKey(provider, AT_SIGNATURE, RSA1024BIT_KEY | CRYPT_EXPORTABLE, &key))
	{
		return false;
	}

	// 3.导出公钥
	if (!CryptExportKey(key, 0, PUBLICKEYBLOB, 0, NULL, &pubKeyLen))
	{
		if (key) CryptDestroyKey(key);
		return false;
	}
	pbPubKey = (unsigned char *)malloc(pubKeyLen * sizeof(unsigned char));
	if (pbPubKey == NULL)
	{
		if (key) CryptDestroyKey(key);
		return false;
	}
	SecureZeroMemory(pbPubKey, pubKeyLen * sizeof(unsigned char));

	if (!CryptExportKey(key, 0, PUBLICKEYBLOB, 0, pbPubKey, &pubKeyLen))
	{
		if (key) CryptDestroyKey(key);
		return false;
	}

	// buffer
	DWORD cbEncoded;
	// length of the encoded string
	DWORD cbDecoded;
	// length of the decoded buffer
	BYTE *pbEncoded = NULL;
	// encoded buffer
	BYTE *pbDecoded = NULL;


	if (CryptEncodeObjectEx(
		MY_ENCODE_TYPE,        // the encoding/decoding type
		RSA_CSP_PUBLICKEYBLOB,
		pbPubKey,
		0,
		NULL,
		NULL,
		&cbEncoded))    // fill in the length needed for
	{
		printf("The number of bytes needed is %d \n", cbEncoded);
	}
	else
	{
		return false;
	}

	if (pbEncoded = (BYTE*)malloc(cbEncoded))
	{
		printf("Memory for pvEncoded has been allocated.\n");
	}
	else
	{
		return false;
	}

	if (CryptEncodeObjectEx(
		MY_ENCODE_TYPE,
		RSA_CSP_PUBLICKEYBLOB,
		pbPubKey,
		0,
		NULL,
		pbEncoded,
		&cbEncoded))
	{
		printf("The public key encoded! \n");
	}
	else
	{
		return false;
	}

	DWORD dwEncodeLen = 0;
	LPSTR szEncodeStr;

	if (!CryptBinaryToStringA(pbEncoded, cbEncoded, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &dwEncodeLen))
		return false;

	szEncodeStr = (char *)malloc(dwEncodeLen * sizeof(char));
	if (szEncodeStr == NULL) return false;
	SecureZeroMemory(szEncodeStr, dwEncodeLen * sizeof(char));

	if (!CryptBinaryToStringA(pbEncoded, cbEncoded, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, szEncodeStr, &dwEncodeLen))
	{
		return false;
	}


	// 4.导出私钥
	if (!CryptExportKey(key, 0, PRIVATEKEYBLOB, 0, NULL, &privKeyLen))
	{
		if (key) CryptDestroyKey(key);
		return false;
	}

	pbPrivKey = (unsigned char *)malloc(privKeyLen * sizeof(unsigned char));
	if (pbPrivKey == NULL)
	{
		if (key) CryptDestroyKey(key);
		return false;
	}
	SecureZeroMemory(pbPrivKey, privKeyLen * sizeof(unsigned char));

	if (!CryptExportKey(key, 0, PRIVATEKEYBLOB, 0, pbPrivKey, &privKeyLen))
	{
		if (key) CryptDestroyKey(key);
		return false;
	}

	// buffer
	DWORD cbEncoded2;
	// length of the encoded string
	DWORD cbDecoded2;
	// length of the decoded buffer
	BYTE *pbEncoded2;
	// encoded buffer
	BYTE *pbDecoded2;

	if (CryptEncodeObjectEx(
		MY_ENCODE_TYPE,        // the encoding/decoding type
		PKCS_RSA_PRIVATE_KEY,
		pbPrivKey,
		0,
		NULL,
		NULL,
		&cbEncoded2))    // fill in the length needed for
	{
		printf("The number of bytes needed is %d \n", cbEncoded2);
	}
	else
	{
		return false;
	}

	if (pbEncoded2 = (BYTE*)malloc(cbEncoded2))
	{
		printf("Memory for pvEncoded has been allocated.\n");
	}
	else
	{
		return false;
	}

	if (CryptEncodeObjectEx(
		MY_ENCODE_TYPE,
		PKCS_RSA_PRIVATE_KEY,
		pbPrivKey,
		0,
		NULL,
		pbEncoded2,
		&cbEncoded2))
	{
		printf("The private key encoded! \n");
	}
	else
	{
		return false;
	}

	DWORD dwEncodeLen2 = 0;
	LPSTR szEncodeStr2;

	if (!CryptBinaryToStringA(pbEncoded2, cbEncoded2, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &dwEncodeLen2))
		return false;

	szEncodeStr2 = (char *)malloc(dwEncodeLen2 * sizeof(char));
	if (szEncodeStr2 == NULL) return false;
	SecureZeroMemory(szEncodeStr2, dwEncodeLen2 * sizeof(char));

	if (!CryptBinaryToStringA(pbEncoded2, cbEncoded2, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, szEncodeStr2, &dwEncodeLen2))
	{
		return false;
	}

	if (bGenerateKeyFile)
	{
		printf("公钥：\r\n");
		printf("%s\r\n", szEncodeStr);
		FILE *fpPub = fopen("Pub.txt", "w+");
		if (fpPub)
		{
			fwrite(szEncodeStr, strlen(szEncodeStr), 1, fpPub);
			fflush(fpPub);
			fclose(fpPub);
		}
		printf("私钥：\r\n");
		printf("%s\r\n", szEncodeStr2);
		FILE *fpPriv = fopen("Priv.txt", "w+");
		if (fpPriv)
		{
			fwrite(szEncodeStr2, strlen(szEncodeStr2), 1, fpPriv);
			fflush(fpPriv);
			fclose(fpPriv);
		}
	}

	return true;
}

//导入签名密钥	20170323	shy
bool CRsaEngine::ImportRsaSignatureKey(HCRYPTPROV* hProv, HCRYPTKEY* hKey, const char* szRSAKey, bool isPrivKey)
{
	DWORD dwBufferLen = 0, cbKeyBlob = 0;
	LPBYTE pbBuffer = NULL, pbKeyBlob = NULL;
	bool hasError = FALSE;
	LPCSTR  keyType;

	if (!CryptStringToBinaryA(szRSAKey, 0, CRYPT_STRING_BASE64, NULL, &dwBufferLen, NULL, NULL))
	{
		printf("Failed to convert BASE64 public key. Error 0x%.8X\n", GetLastError());
		hasError = TRUE;
		goto Exit;
	}

	pbBuffer = (LPBYTE)LocalAlloc(0, dwBufferLen);
	if (!CryptStringToBinaryA(szRSAKey, 0, CRYPT_STRING_BASE64, pbBuffer, &dwBufferLen, NULL, NULL))
	{
		printf("Failed to convert BASE64 public key. Error 0x%.8X\n", GetLastError());
		hasError = TRUE;
		goto Exit;
	}

	// 判断导入的是私钥，还是公钥
	if (isPrivKey)
	{
		keyType = PKCS_RSA_PRIVATE_KEY;
	}
	else
	{
		keyType = RSA_CSP_PUBLICKEYBLOB;
	}

	if (!CryptDecodeObjectEx(MY_ENCODE_TYPE, keyType, pbBuffer, dwBufferLen, 0, NULL, NULL, &cbKeyBlob))
	{
		printf("Failed to parse public key. Error 0x%.8X\n", GetLastError());
		hasError = TRUE;
		goto Exit;
	}

	pbKeyBlob = (LPBYTE)LocalAlloc(0, cbKeyBlob);
	if (!CryptDecodeObjectEx(MY_ENCODE_TYPE, keyType, pbBuffer, dwBufferLen, 0, NULL, pbKeyBlob, &cbKeyBlob))
	{
		printf("Failed to parse public key. Error 0x%.8X\n", GetLastError());
		hasError = TRUE;
		goto Exit;
	}

	if (!CryptAcquireContext(hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
	{
		printf("CryptAcquireContext failed with error 0x%.8X\n", GetLastError());
		hasError = TRUE;
		goto Exit;
	}

	if (!CryptImportKey(*hProv, pbKeyBlob, cbKeyBlob, NULL, 0, hKey))
	{
		printf("CryptImportKey for public key failed with error 0x%.8X\n", GetLastError());
		hasError = TRUE;
		goto Exit;
	}

	printf("ImportRsaKey successfully!\n\n");

Exit:
	if (pbBuffer) LocalFree(pbBuffer);
	if (pbKeyBlob) LocalFree(pbKeyBlob);

	if (hasError) {
		return FALSE;
	}
	return TRUE;
}

//销毁容器和释放key
bool CRsaEngine::Release()
{
	int iAbort = 0;
	try
	{
		//异常1
		iAbort++;

		if (m_hPublicKey) CryptDestroyKey(m_hPublicKey);// printf("Error: %d\n", GetLastError());

		//异常2
		iAbort++;

		if (m_hPrivateKey) CryptDestroyKey(m_hPrivateKey);// printf("Error: %d\n", GetLastError());

		//异常3
		iAbort++;

		if (m_hCryptProv) CryptReleaseContext(m_hCryptProv, 0); //printf("Error: %d\n", GetLastError());//不能销毁容器要永久保存否则无法导入成功

		//异常4
		iAbort++;
	}
	catch (...)
	{
		//TCHAR yichang[200] = { 0 };
		//_sntprintf(yichang,200, TEXT("异常：%d\r\n"), iAbort);

		//m_IniFileIO.WriteString(TEXT("异常"), TEXT("yichang"), yichang, TEXT(".\\yichang.txt"));
		//	printf("%s", yichang);
		//getchar();
	}

	m_hPublicKey = NULL;
	m_hPrivateKey = NULL;
	m_hCryptProv = NULL;

	return TRUE;
}

//生成密钥对
bool CRsaEngine::GenerateKeyPairs()
{
	if (!m_hCryptProv) return FALSE;

	HCRYPTKEY hKey = NULL;

	// Generate new key pair
	if (!CryptGenKey(m_hCryptProv, AT_KEYEXCHANGE, RSA1024BIT_KEY | CRYPT_EXPORTABLE, &hKey))
	{
		//TRACE(_T("CryptGenKey error 0x%x\n"), GetLastError());
		return FALSE;
	}

	// Get public key size
	if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, NULL, &m_stPublicKey.ulKeyLen))
	{
		//TRACE(_T("CryptExportKey error 0x%x\n"), GetLastError());
		return FALSE;
	}

	// Create a buffer for the public key
	//pbPublicKey = (BYTE *)malloc(dwPublicKeyLen);

	// Get public key
	if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, m_stPublicKey.ucKey, &m_stPublicKey.ulKeyLen))
	{
		//TRACE(_T("CryptExportKey error 0x%x\n"), GetLastError());
		return FALSE;
	}

	// Get private key size
	if (!CryptExportKey(hKey, NULL, PRIVATEKEYBLOB, 0, NULL, &m_stPrivateKey.ulKeyLen))
	{
		//TRACE(_T("CryptExportKey error 0x%x\n"), GetLastError());
		return FALSE;
	}

	// Create a buffer for the private key
	//pbPrivateKey = (BYTE *)malloc(dwPrivateKeyLen);

	// Get private key
	//TRACE(_T("CryptExportKey...\n"));
	if (!CryptExportKey(hKey, NULL, PRIVATEKEYBLOB, 0, m_stPrivateKey.ucKey, &m_stPrivateKey.ulKeyLen))
	{
		//TRACE(_T("CryptExportKey error 0x%x\n"), GetLastError());
		return FALSE;
	}

	if (hKey)
	{
		CryptDestroyKey(hKey);
	}

	return TRUE;
}

//导入密钥对（若已经存在密钥对则先销毁后导入）
bool CRsaEngine::ImportKeyPairs()
{
	if (!m_hCryptProv) return FALSE;

	bool bImportSuccess = FALSE;

	//导入public key
	if (m_stPublicKey.ulKeyLen != 0)
	{
		if (!CryptImportKey(m_hCryptProv, (BYTE *)m_stPublicKey.ucKey, m_stPublicKey.ulKeyLen, 0, 0, &m_hPublicKey))
		{
			//TRACE("CryptImportKey error 0x%x\n", GetLastError());
			return FALSE;
		}

		bImportSuccess = TRUE;
	}

	//导入Private key
	if (m_stPrivateKey.ulKeyLen != 0)
	{
		if (!CryptImportKey(m_hCryptProv, (BYTE *)m_stPrivateKey.ucKey, m_stPrivateKey.ulKeyLen, 0, 0, &m_hPrivateKey))
		{
			//TRACE("CryptImportKey error 0x%x\n", GetLastError());
			return FALSE;
		}

		bImportSuccess = TRUE;
	}

	return bImportSuccess;
}

//字符转换
bool CRsaEngine::UnicodeToAnsi(LPTSTR szUnicodeSrc, char *pAnsiStrOut, INT nAnsiStrSize, INT *pAnsiOutStrlenByte)
{
	//参数校验
	if (szUnicodeSrc == NULL || pAnsiStrOut == NULL || nAnsiStrSize < 0) return FALSE;

	ZeroMemory(pAnsiStrOut, nAnsiStrSize);
	INT nLength = 0;

	//获取转换所需字节数 
	nLength = WideCharToMultiByte(CP_ACP, 0, szUnicodeSrc, -1, NULL, 0, NULL, NULL);

	//转换空间合法性判断
	if (nLength > nAnsiStrSize)
	{
		//所需的字节数
		if (pAnsiOutStrlenByte)	*pAnsiOutStrlenByte = nLength;

		return FALSE;
	}

	INT nTransFormResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szUnicodeSrc, -1, pAnsiStrOut, nAnsiStrSize, NULL, NULL);//Unicode转ANSI，第一个参数当前系统ANSI,第二个参数是默认0，最后第二个是不能转换用于替换的字符，最后一个是默认true

	if (nTransFormResult == 0) return FALSE;

	//转换字节数返回
	if (pAnsiOutStrlenByte)	*pAnsiOutStrlenByte = nTransFormResult;

	return TRUE;
}


//字符转换
bool CRsaEngine::AnsiToUnicode(char *pAnsiStrIn, LPTSTR szUnicodeStrOut, INT nUnicodeCacheSize, INT *nUnicodeStrLenOutCalculateZeroTail)
{
	//参数校验
	if (pAnsiStrIn == NULL || szUnicodeStrOut == NULL || nUnicodeCacheSize <= 0) return FALSE;

	//清空输出
	ZeroMemory(szUnicodeStrOut, nUnicodeCacheSize);

	//获取转换所需宽字符个数
	INT nLength = 0;
	nLength = MultiByteToWideChar(CP_ACP, 0, pAnsiStrIn, -1, NULL, 0);	//ccWideByte为0计算所需个数

	//转换空间合法性判断
	if (nLength*sizeof(TCHAR) > nUnicodeCacheSize)
	{
		//所需宽字符个数
		if (nUnicodeStrLenOutCalculateZeroTail)	*nUnicodeStrLenOutCalculateZeroTail = nLength;

		return FALSE;
	}

	//转换
	INT nTransFormResult = MultiByteToWideChar(CP_ACP, 0, pAnsiStrIn, -1, szUnicodeStrOut, nUnicodeCacheSize);
	if (nTransFormResult == 0) return FALSE;

	//转换字节数返回
	if (nUnicodeStrLenOutCalculateZeroTail)	*nUnicodeStrLenOutCalculateZeroTail = nTransFormResult;

	return TRUE;
}

//从文件中读取public key 和private key
bool CRsaEngine::ReadPublicPrivateKey(const char * szPublicKeyFile, const char * szPrivateKeyFile)
{
	CIniFileIO      IniFileIO;		//读写配置文件	

	//读取public	key
	char  szKey[RSA_KEY_STR_LEN] = { 0 };
	unsigned char ucKey[RSA_KEY_STR_LEN] = { 0 };

	bool bReadSuccess = FALSE;

	if (szPublicKeyFile)
	{
		IniFileIO.SetFilePath(szPublicKeyFile);
		IniFileIO.ReadString(PUBLIC_KEY_APPNAME, RSA_KEY_NAME, TEXT(""), szKey, RSA_KEY_STR_LEN);
		if (lstrlen(szKey) == 0)
		{
			//读入失败
			return FALSE;
		}
		//m_szPublicKey[RSA_KEY_STR_LEN - 1] = 0;

		//TCHAR 转换为 char*
		if (!UnicodeToAnsi(szKey, (char *)ucKey, sizeof(char)*RSA_KEY_STR_LEN)) return FALSE;

		m_stPublicKey.ulKeyLen = RSA_KEY_STR_LEN;

		//反编码
		if (!HexDecodeData(ucKey, strlen((char *)ucKey), (m_stPublicKey.ucKey), &m_stPublicKey.ulKeyLen)) return FALSE;
		bReadSuccess = TRUE;
	}

	if (szPrivateKeyFile)
	{
		//私钥
		memset(szKey, 0, sizeof(TCHAR)*RSA_KEY_STR_LEN);
		memset(ucKey, 0, sizeof(char)*RSA_KEY_STR_LEN);
		IniFileIO.SetFilePath(szPrivateKeyFile);
		IniFileIO.ReadString(PRIVATE_KEY_APPNAME, RSA_KEY_NAME, TEXT(""), szKey, RSA_KEY_STR_LEN);
		if (lstrlen(szKey) == 0)
		{
			//读入失败
			return FALSE;
		}
		//m_szPublicKey[RSA_KEY_STR_LEN - 1] = 0;

		//TCHAR 转换为 char*
		if (!UnicodeToAnsi(szKey, (char *)ucKey, sizeof(char)*RSA_KEY_STR_LEN)) return FALSE;

		//空间大小
		m_stPrivateKey.ulKeyLen = RSA_KEY_STR_LEN;

		//反编码
		if (!HexDecodeData(ucKey, strlen((char *)ucKey), (m_stPrivateKey.ucKey), &m_stPrivateKey.ulKeyLen)) return FALSE;

		bReadSuccess = TRUE;
	}

	//存在常量
	if (strlen(YCRsaPublicKey) != 0)
	{
		memset(ucKey, 0, sizeof(char)*RSA_KEY_STR_LEN);
		memmove(ucKey, YCRsaPublicKey, strlen(YCRsaPublicKey));

		m_stPublicKey.ulKeyLen = RSA_KEY_STR_LEN;

		//反编码
		if (!HexDecodeData(ucKey, strlen((char *)ucKey), (m_stPublicKey.ucKey), &m_stPublicKey.ulKeyLen)) return FALSE;

		bReadSuccess = TRUE;
	}

	//存在常量
	if (strlen(YCRsaPrivateKey) != 0)
	{
		memset(ucKey, 0, sizeof(char)*RSA_KEY_STR_LEN);
		memmove(ucKey, YCRsaPrivateKey, strlen(YCRsaPrivateKey));

		m_stPrivateKey.ulKeyLen = RSA_KEY_STR_LEN;

		//反编码
		if (!HexDecodeData(ucKey, strlen((char *)ucKey), (m_stPrivateKey.ucKey), &m_stPrivateKey.ulKeyLen)) return FALSE;

		bReadSuccess = TRUE;
	}

	//密钥存在
	if (strlen((char *)m_stPublicKey.ucKey)!=0||strlen((char *)m_stPrivateKey.ucKey)!=0) bReadSuccess = TRUE;

	return bReadSuccess;
}

//写入配置文件
bool CRsaEngine::WritePublicPrivateKey(LPTSTR szPublicKeyFile, LPTSTR szPrivateKeyFile)
{
	bool bWriteSuccess = FALSE;
	CIniFileIO      IniFileIO;		//读写配置文件	

	//缓冲
	unsigned char szKey[RSA_KEY_STR_LEN] = { 0 };
	TCHAR szUnicodeKey[RSA_KEY_STR_LEN] = { 0 };
	unsigned long ulKeyLen = 0;

	if (szPublicKeyFile)
	{
		memset(szKey, 0, sizeof(char)*RSA_KEY_STR_LEN);
		memset(szUnicodeKey, 0, sizeof(TCHAR)*RSA_KEY_STR_LEN);
		ulKeyLen = RSA_KEY_STR_LEN;

		//编码
		if (!HexEncodeData(m_stPublicKey.ucKey, m_stPublicKey.ulKeyLen, szKey, &ulKeyLen)) return FALSE;

		//字符串转换
		if (!AnsiToUnicode((char *)szKey, szUnicodeKey, sizeof(TCHAR)*RSA_KEY_STR_LEN)) return FALSE;

		//写入配置文件
		if (!IniFileIO.WriteString(PUBLIC_KEY_APPNAME, RSA_KEY_NAME, szUnicodeKey, szPublicKeyFile)) return FALSE;

		bWriteSuccess = TRUE;
	}

	ulKeyLen = 0;

	if (szPrivateKeyFile)
	{
		memset(szKey, 0, sizeof(char)*RSA_KEY_STR_LEN);
		memset(szUnicodeKey, 0, sizeof(TCHAR)*RSA_KEY_STR_LEN);
		ulKeyLen = RSA_KEY_STR_LEN;

		//编码
		if (!HexEncodeData(m_stPrivateKey.ucKey, m_stPrivateKey.ulKeyLen, szKey, &ulKeyLen)) return FALSE;

		//字符串转换
		if (!AnsiToUnicode((char *)szKey, szUnicodeKey, sizeof(TCHAR)*RSA_KEY_STR_LEN)) return FALSE;

		//写入配置文件
		if (!IniFileIO.WriteString(PRIVATE_KEY_APPNAME, RSA_KEY_NAME, szUnicodeKey, szPrivateKeyFile)) return FALSE;

		bWriteSuccess = TRUE;
	}

	return bWriteSuccess;
}

///////////////////////////////////////////////////////////////////////////////

//分组加密解密

//分组循环加密数据 每组数据固定64字节明文
bool CBlockRsa::DataBlockCircleEncrypte(unsigned char *plaintext, unsigned long ulPlainTextLen, unsigned char **pCipherTextOut, unsigned long *pCipherSizeInLenOut)
{
	if (!pCipherTextOut || !plaintext || !pCipherSizeInLenOut || (*pCipherSizeInLenOut <= 0)) return FALSE;

	//明文长度分析
	int iCircleCount = (ulPlainTextLen%PLAIN_TEXT_MAX_LEN == 0) ? (ulPlainTextLen / PLAIN_TEXT_MAX_LEN) : (ulPlainTextLen / PLAIN_TEXT_MAX_LEN + 1);

	//所需密文空间
	int iCipherMemoryNeeded = iCircleCount*CIPHER_TEXT_OUT_ENCODE_LEN;

	//输入指针	明文不够PLAIN_TEXT_MAX_LEN的整数倍补0
	unsigned char *pPlainTextTmp = new unsigned char[iCipherMemoryNeeded];
	if (!pPlainTextTmp) return FALSE;
	memset(pPlainTextTmp, 0, iCipherMemoryNeeded);
	memmove(pPlainTextTmp, plaintext, ulPlainTextLen);

	//输出的密文长度分析（密文输出空间是否足够）
	if (*pCipherSizeInLenOut < iCipherMemoryNeeded)
	{
		*pCipherSizeInLenOut = iCipherMemoryNeeded;
		return FALSE;
	}

	//输出指针
	unsigned char *pCipherOutTmp = *pCipherTextOut;
	unsigned long ulCipherOut = *pCipherSizeInLenOut;
	unsigned long	ulCipherLenOutTmp = 0;

	bool bFinal = FALSE;
	//分组加密循环体
	for (int i = 0; i < iCircleCount; i++)
	{
		if (i == iCircleCount - 1) bFinal = TRUE;

		if (!Encrypte(pPlainTextTmp, PLAIN_TEXT_MAX_LEN, &pCipherOutTmp, &ulCipherOut, bFinal))
		{
			delete[]pPlainTextTmp;
			*pCipherSizeInLenOut = ulCipherLenOutTmp;
			return FALSE;
		}
		pCipherOutTmp += ulCipherOut;
		pPlainTextTmp += PLAIN_TEXT_MAX_LEN;
		ulCipherLenOutTmp += ulCipherOut;
		ulCipherOut = *pCipherSizeInLenOut - ulCipherOut;
	}

	//输出密文，密文长度
	*pCipherSizeInLenOut = ulCipherLenOutTmp;

	//清理空间
	delete[]pPlainTextTmp;

	return TRUE;

}

//分组解密数据
bool CBlockRsa::DataBlockCircleDecrypte(unsigned char *ciphertext, unsigned long cipherlen, unsigned char **plaintext, unsigned long *pPlainSizeInLenOut)
{
	if (!ciphertext || !plaintext || !pPlainSizeInLenOut || (*pPlainSizeInLenOut <= 0)) return FALSE;

	//分析解密循环次数
	int iCircleCount = (cipherlen%CIPHER_TEXT_OUT_ENCODE_LEN == 0) ? (cipherlen / CIPHER_TEXT_OUT_ENCODE_LEN) : (cipherlen / CIPHER_TEXT_OUT_ENCODE_LEN + 1);

	//所需空间
	int iDecrypteSpaceNeeded = iCircleCount*PLAIN_TEXT_MAX_LEN;

	//判断空间释放足够
	if (*pPlainSizeInLenOut < iDecrypteSpaceNeeded)
	{
		*pPlainSizeInLenOut = iDecrypteSpaceNeeded;
	}

	//输入指针
	unsigned char *pCipherData = ciphertext;

	//输出指针
	unsigned char *pPlainTextOut = *plaintext;
	unsigned long	ulPlainLenOut = *pPlainSizeInLenOut;
	unsigned long	ulPlainLenOutTmp = 0;

	bool bFinal = FALSE;

	//解密循环体
	for (int i = 0; i < iCircleCount; i++)
	{
		if (i == iCircleCount - 1) bFinal = TRUE;

		if (!Decrypte(pCipherData, CIPHER_TEXT_OUT_ENCODE_LEN, &pPlainTextOut, &ulPlainLenOut, bFinal))
		{
			*pPlainSizeInLenOut = ulPlainLenOutTmp;
			return FALSE;
		}

		pCipherData += CIPHER_TEXT_OUT_ENCODE_LEN;
		pPlainTextOut += ulPlainLenOut;

		ulPlainLenOutTmp += ulPlainLenOut;
		ulPlainLenOut = *pPlainSizeInLenOut - ulPlainLenOut;

	}

	//输出解密数据大小
	*pPlainSizeInLenOut = ulPlainLenOutTmp;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////

//des加密解密
#include<time.h>

CDESEngine::CDESEngine()
{
	memset(&m_stKey, 0, S_KEY_BLOB_T);
	m_stKey.hdr.bType = PLAINTEXTKEYBLOB;
	m_stKey.hdr.bVersion = CUR_BLOB_VERSION;
	m_stKey.hdr.reserved = 0;
	m_stKey.hdr.aiKeyAlg = CALG_DES;
	m_stKey.cbKeySize = DES_KEY_BIT;
	ZeroMemory(m_stKey.rgbKeyData, DES_KEY_BIT);

	m_hCryptProv = NULL;
	m_hKey = NULL;
	memset(m_szCspName, 0, sizeof(TCHAR)* RSA_CONTAINER_NAME_LEN);
	memmove(m_szCspName, CSP_DES_NAME, sizeof(TCHAR)* lstrlen(CSP_DES_NAME));
}


CDESEngine::~CDESEngine()
{
	Release();
}

//容器初始化
bool	CDESEngine::CspInit()
{
	//打开或创建CSP容器
	if (!CryptAcquireContext(&m_hCryptProv, m_szCspName, MS_DEF_PROV, PROV_RSA_FULL, 0))//默认设置为0，获取出错则新建
	{
		if (GetLastError() == NTE_BAD_KEYSET)//容器不能被打开
		{
			if (!CryptAcquireContextW(&m_hCryptProv, m_szCspName, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))//使用指定名容器新建
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;

}

//产生密钥
bool CDESEngine::GenerateKey(BYTE * pbKey, DWORD *pnKeyLen)
{
	//	if (nKeySize < DES_KEY_BIT+1) return FALSE;

	BYTE key[DES_KEY_BIT + 1] = { 0 };

	//选取3位数字

	if (!pbKey || strlen((char *)pbKey) == 0)
	{
		srand(time(NULL));
		int m = 0;
		for (int i = 0; i < DES_KEY_BIT; i++)
		{
			//选区种类
			m = rand() % DES_KEY_TYPE_NUM;
			switch (m)
			{
			case DES_NUM_TYPE:
			{
								 //选取数字
								 key[i] = DesKeyNumLib[rand() % DES_NUM_LEN];
								 break;
			}
			case DES_CHAR_TYPE:
			{
								  //选取英文
								  key[i] = DesKeyCharLib[rand() % DES_CHAR_LEN];
								  break;
			}
			case DES_SYMBOL_TYPE:
			{
									//选取字符
									key[i] = DesKeySymbolLib[rand() % DES_SYMBOL_LEN];
									//key[i] = DesKeyNumLib[rand() % DES_NUM_LEN];
									break;
			}
			}
		}
	}
	else
	{
		if (pnKeyLen)
			memmove(key, pbKey, *pnKeyLen);
	}

	int iKeyLenCount = 0;

	//拷贝结果
	if (pbKey&&strlen((char *)pbKey) == 0)
	{

		if (pnKeyLen)
		{
			iKeyLenCount = (*pnKeyLen > DES_KEY_BIT ? DES_KEY_BIT : *pnKeyLen);
			memmove(pbKey, key, iKeyLenCount);
		}
		else
		{
			iKeyLenCount = DES_KEY_BIT;
			memmove(pbKey, key, DES_KEY_BIT);
		}
	}
	else
	{
		iKeyLenCount = strlen((char *)key) > DES_KEY_BIT ? DES_KEY_BIT : strlen((char *)key);
	}

	//	memset(pbKey, 0, nKeySize);
	//memmove(pbKey, key, DES_KEY_BIT);
	memmove(m_stKey.rgbKeyData, key, iKeyLenCount);
	if (pnKeyLen)
		*pnKeyLen = strlen((char *)m_stKey.rgbKeyData);
	//printf("GenerateKey 产生key:%s\r\n", (char *)m_stKey.rgbKeyData);

	return TRUE;
}

//导入密钥
bool	CDESEngine::ImportKey()
{
	if (!m_hCryptProv) return FALSE;

	if (!CryptImportKey(m_hCryptProv, (BYTE*)&m_stKey, sizeof(m_stKey), 0, 0, &m_hKey))
	{
		CryptReleaseContext(m_hCryptProv, 0);
		m_hCryptProv = NULL;
		return FALSE;
	}

	return TRUE;
}

//释放清理
bool	CDESEngine::Release()
{
	if (m_hKey) CryptDestroyKey(m_hKey);
	if (m_hCryptProv) CryptReleaseContext(m_hCryptProv, 0);

	m_hKey = NULL;
	m_hCryptProv = NULL;

	memset(&m_stKey, 0, S_KEY_BLOB_T);
	m_stKey.hdr.bType = PLAINTEXTKEYBLOB;
	m_stKey.hdr.bVersion = CUR_BLOB_VERSION;
	m_stKey.hdr.reserved = 0;
	m_stKey.hdr.aiKeyAlg = CALG_DES;
	m_stKey.cbKeySize = DES_KEY_BIT;
	ZeroMemory(m_stKey.rgbKeyData, DES_KEY_BIT);
	memset(m_szCspName, 0, sizeof(TCHAR)* RSA_CONTAINER_NAME_LEN);
	memmove(m_szCspName, CSP_DES_NAME, sizeof(TCHAR)* lstrlen(CSP_DES_NAME));

	return TRUE;
}

//加密
bool	CDESEngine::EncrypteData(__inout const BYTE* pbKey,
	__inout DWORD *pcbKeyLen,
	__in const BYTE* pbPlainText,
	__in DWORD cbPlainDataLen,
	__out BYTE* pbCipherBuf,
	__inout DWORD* pcbCipherBufLen)
{
	//初始化
	if (!CspInit()) return FALSE;

	//产生密钥
	BYTE cbKey[DES_KEY_BIT] = { 0 };
	if (!GenerateKey((BYTE *)pbKey, pcbKeyLen)) return FALSE;

	//导入密钥
	ImportKey();

	//加密
	bool bRet;
	BYTE pbBlock[2 * DES_KEY_BIT];
	DWORD dwBlock, dwOut = 0;
	bool bEOF;
	for (DWORD i = 0; i < cbPlainDataLen; i += DES_KEY_BIT)
	{
		bEOF = cbPlainDataLen - i <= DES_KEY_BIT;    //是否为最后一组
		dwBlock = bEOF ? cbPlainDataLen - i : DES_KEY_BIT;

		CopyMemory(pbBlock, pbPlainText + i, dwBlock);    //分组加密、解密，每组 DES_KEY_BIT 字节

		bRet = CryptEncrypt(m_hKey, NULL, bEOF, 0, pbBlock, &dwBlock, sizeof(pbBlock));

		if (!bRet)
			break;

		if (pbCipherBuf)
		{
			if (dwOut + dwBlock > *pcbCipherBufLen)
			{
				bRet = FALSE;
				break;
			}
			else
			{
				CopyMemory(pbCipherBuf + dwOut, pbBlock, dwBlock);
				dwOut += dwBlock;
			}
		}
		else    //当pbBuf为NULL时，仅计算需要多大的pbBuf
			dwOut += dwBlock;
	}

	*pcbCipherBufLen = dwOut;

	//释放
	Release();

	return TRUE;
}

//解密
bool	CDESEngine::DecrypteData(__in const BYTE* pbKey,
	__in DWORD cbKeyLen,
	__in const BYTE* pbCipherData,
	__in DWORD cbCipherDataLen,
	__out BYTE* pbPlainTextBuf,
	__inout DWORD* pcbPlainTextBufLen)
{
	//初始化
	if (!CspInit()) return FALSE;

	//产生密钥
	BYTE cbKey[DES_KEY_BIT] = { 0 };
	if (!GenerateKey((BYTE *)pbKey, &cbKeyLen)) return FALSE;

	//导入密钥
	if (!ImportKey()) return FALSE;

	//解密
	bool bRet;
	BYTE pbBlock[2 * DES_KEY_BIT];
	DWORD dwBlock, dwOut = 0;
	bool bEOF;
	for (DWORD i = 0; i < cbCipherDataLen; i += DES_KEY_BIT)
	{
		bEOF = cbCipherDataLen - i <= DES_KEY_BIT;    //是否为最后一组
		dwBlock = bEOF ? cbCipherDataLen - i : DES_KEY_BIT;

		CopyMemory(pbBlock, pbCipherData + i, dwBlock);    //分组加密、解密，每组8字节

		bRet = CryptDecrypt(m_hKey, NULL, bEOF, 0, pbBlock, &dwBlock);

		if (!bRet)
			break;

		if (pbPlainTextBuf)
		{
			if (dwOut + dwBlock > *pcbPlainTextBufLen)
			{
				bRet = FALSE;
				break;
			}
			else
			{
				CopyMemory(pbPlainTextBuf + dwOut, pbBlock, dwBlock);
				dwOut += dwBlock;
			}
		}
		else    //当pbBuf为NULL时，仅计算需要多大的pbBuf
			dwOut += dwBlock;
	}

	*pcbPlainTextBufLen = dwOut;

	//释放
	Release();

	return TRUE;
}

//加密所需要的空间
int  CDESEngine::EncrypteMemoryNeeded(__inout const BYTE* pbKey,
	__inout DWORD *pcbSizeInKeyLenOut,
	__in const BYTE* pbPlainText,
	__in DWORD cbPlainDataLen)
{
	DWORD dwNeeded = 0;

	if (!EncrypteData(pbKey, pcbSizeInKeyLenOut, pbPlainText, cbPlainDataLen, NULL, &dwNeeded)) return -1;

	return dwNeeded * 2;/*包含编码*/
}

//解密所需要的空间
int		CDESEngine::DecrypteMemoryNeeded(__in const BYTE* pbKey,
	__in DWORD cbKeyLen,
	__in const BYTE* pbCipherData,
	__in DWORD cbCipherDataLen)
{
	DWORD dwNeeded = 0;

	if (!DecrypteData(pbKey, cbKeyLen, pbCipherData, cbCipherDataLen, NULL, &dwNeeded)) return -1;

	return dwNeeded;/*包含编码*/
}


//编码（16进制）
bool CDESEngine::HexEncodeData(unsigned char *pucDataSrc, unsigned long ulDataSrcDataLen, unsigned char *pucDataOut, unsigned long *pulDataSizeInLenOut)
{
	//参数校验
	if (!pucDataSrc || !pucDataOut || !pulDataSizeInLenOut) return FALSE;
	if (*pulDataSizeInLenOut < 2 * ulDataSrcDataLen)
	{
		*pulDataSizeInLenOut = 2 * ulDataSrcDataLen;
		return FALSE;
	}

	//缓冲区
	unsigned long n = 0;
	unsigned char ucCache[3] = { 0 };
	unsigned char *ucDataCache = new unsigned char[2 * ulDataSrcDataLen];
	if (!ucDataCache) return FALSE;
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

	return TRUE;
}

//反编码（16进制）
bool CDESEngine::HexDecodeData(unsigned char *pucDataSrc, unsigned long ulDataSrcDataLen, unsigned char *pucDataOut, unsigned long *pulDataSizeInLenOut)
{
	//参数校验
	if (!pucDataSrc || !pucDataOut || !pulDataSizeInLenOut) return FALSE;

	unsigned char strtmp[3] = { 0 };
	int strcount = ((ulDataSrcDataLen) % 2 == 0 ? (ulDataSrcDataLen) / 2 : ((ulDataSrcDataLen) / 2 + 1));
	char *decodetmp = new char[strcount];
	if (!decodetmp) return FALSE;
	memset(decodetmp, 0, strcount);

	if (*pulDataSizeInLenOut <  strcount)
	{
		*pulDataSizeInLenOut = strcount;
		delete[]decodetmp;
		return FALSE;
	}

	//转换
	for (int i = 0; i<strcount; i++)
	{
		memset(strtmp, 0, 3);
		memmove(strtmp, pucDataSrc + 2 * i, 2);
		if (!HexToInt(&strtmp[0]))
			return false;
		if (!HexToInt(&strtmp[1]))
			return false;
		decodetmp[i] = COMBINE_BYTE(strtmp[0], strtmp[1]);
	}

	//转移数据
	memset(pucDataOut, 0, *pulDataSizeInLenOut);
	memmove(pucDataOut, decodetmp, strcount);
	*pulDataSizeInLenOut = strcount;

	delete[]decodetmp;

	return TRUE;
}

//16进制转十进制
bool CDESEngine::HexToInt(unsigned char *HexData)
{
	//参数校验
	if (!HexData) return FALSE;

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
		return FALSE;
	}

	return TRUE;
}


//加密并编码
bool CDESEngine::Encrypte(__inout const BYTE* pbKey,
	__inout DWORD *pcbKeyLen,
	__in const BYTE* pbPlainText,
	__in DWORD cbPlainDataLen,
	__out BYTE* pbCipherBuf,
	__inout DWORD* pcbCipherBufLen)
{
	DWORD dwCipherSize = *pcbCipherBufLen;

	//加密
	if (!EncrypteData(pbKey, pcbKeyLen, pbPlainText, cbPlainDataLen, pbCipherBuf, pcbCipherBufLen)) return FALSE;

	//编码输出
	if (!HexEncodeData(pbCipherBuf, *pcbCipherBufLen, pbCipherBuf, &dwCipherSize)) return FALSE;

	*pcbCipherBufLen = dwCipherSize;

	return TRUE;
}

//解码并解密
bool CDESEngine::Decrypte(__in const BYTE* pbKey,
	__in DWORD cbKeyLen,
	__in const BYTE* pbCipherData,
	__in DWORD cbCipherDataLen,
	__out BYTE* pbPlainTextBuf,
	__inout DWORD* pcbPlainTextBufLen)
{
	//解码
	if (!HexDecodeData((unsigned char *)pbCipherData, cbCipherDataLen, (unsigned char *)pbCipherData, &cbCipherDataLen)) return FALSE;

	//解密
	if (!DecrypteData(pbKey, cbKeyLen, pbCipherData, cbCipherDataLen, pbPlainTextBuf, pcbPlainTextBufLen)) return FALSE;

	return TRUE;
}
