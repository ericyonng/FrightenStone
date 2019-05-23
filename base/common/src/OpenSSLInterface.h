#ifndef OPENSSL_INTERFACE_H_
#define OPENSSL_INTERFACE_H_

#pragma once

#include<openssl/md5.h>

	#ifndef _DEBUG
		#pragma comment(lib, "StaticLib\\Release\\lib\\libeay32.lib")
		#pragma comment(lib, "StaticLib\\Release\\lib\\ssleay32.lib")
	#else
		#pragma comment(lib, "StaticLib\\Debug\\lib\\libeay32.lib")
		#pragma comment(lib, "StaticLib\\Debug\\lib\\ssleay32.lib")
	#endif

#endif

