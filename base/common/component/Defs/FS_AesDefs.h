#ifndef __Base_Common_Component_Defs_FS_AesDefs_H__
#define __Base_Common_Component_Defs_FS_AesDefs_H__
#pragma once

#include "base/common/basedefs/DataType/DataType.h"

#pragma region macro define
#define FS_AES_256_KEY_LEN 32       // 256bit/8bit
#pragma endregion

// some define refer to aes
class FS_AesDefs
{
public:
    typedef struct {
        U8 key[FS_AES_256_KEY_LEN];
        U8 enckey[FS_AES_256_KEY_LEN];
        U8 deckey[FS_AES_256_KEY_LEN];
    } Aes256Context;
};

#endif
