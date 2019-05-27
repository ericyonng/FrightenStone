#ifndef __Base_Common_Component_Defs_FS_AesDefs_H__
#define __Base_Common_Component_Defs_FS_AesDefs_H__
#pragma once

#include "base/common/basedefs/DataType/DataType.h"
#include "base/exportbase.h"

#pragma region macro define
#define FS_AES_256_KEY_LEN 32       // 256bit/8bit
#pragma endregion

// some define refer to aes
FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_AesDefs
{
public:     
    // √‹Œƒ¿‡–Õ
    enum AES_CYPHER_T_TYPE
    {
        AES_CYPHER_128 = 0,
        AES_CYPHER_192,
        AES_CYPHER_256,
    };
};
FS_NAMESPACE_END
#endif
