#ifndef LEN_MACRO_H_
#define LEN_MACRO_H_

#pragma once

#define	LEN_AES_256_KEY			(256/8)

#define AES_PLAINTXT_PART_LEN_LIMIT		16			//明文分组长度

//宏定义
#define		PLAIN_TEXT_MAX_LEN					64						//明文长度限制（采用分组加密）117（1024/8-11）是rsa1024对明文的限制
#define		CIPHER_TEXT_OUT_ENCODE_LEN			256						//加密并编码后的数据长度（无论明文多长 不超过对明文的限制）
#define		CIPHER_TEXT_OUT_DECODE_LEN			128						//为编码的加密后的数据长度
#define		RSA_CONTAINER_NAME_LEN				128						//CSP名长度
#define		CSP_CONTAINER_NAME				"FengYanchenCsp"		//定义CSP容器名
#define		PUBLIC_KEY_APPNAME				"PUBLIC_KEY"			//公钥节点名
#define		PRIVATE_KEY_APPNAME				"PRIVATE_KEY"			//私钥节点名
#define		RSA_KEY_NAME					"key"					//键名
#define		RSA_KEY_STR_LEN					8192						//公钥长度

#define		INI_FILE_NAME_LEN		512

#undef MD5_VERIFY_SIZE
#define MD5_VERIFY_SIZE	(MD5_DIGEST_LENGTH * 2 + 1)	//md5包校验码

#endif
