#include "stdafx.h"
#include "FrightenStone/common/component/Impl/XorEncrypt.h"
#include "FrightenStone/common/status/status.h"

FS_NAMESPACE_BEGIN

Int32 XorEncrypt::Encrypt(const Byte8 *plainText, Int32 plainSize, Byte8 *cypherText, Int32 cypherSize, const Byte8 *key, Int32 keySize)
{
    if(plainSize < keySize || plainSize % keySize != 0)
        return StatusDefs::XorEncrypt_PlainTextLenNotEnough;

    if(cypherSize < plainSize)
        return StatusDefs::XorEncrypt_CypherTextSizeNotEnough;

    Int32 j = 0;
    for(Int32 i = 0; i < plainSize; ++i)
    {
        cypherText[i] = plainText[i] ^ key[j];
        j = (j + 1) >= keySize ? 0 : (j + 1);
    }

    return StatusDefs::Success;
}

Int32 XorEncrypt::Decrypt(const Byte8 *cypherText, Int32 cypherSize, Byte8 *plainText, Int32 plainSize, const Byte8 *key, Int32 keySize)
{
    if(cypherSize < keySize || cypherSize % keySize != 0)
        return StatusDefs::XorEncrypt_CypherTextLenNotEnough;

    if(plainSize < cypherSize)
        return StatusDefs::XorEncrypt_PlainTextSizeNotEnough;

    Int32 j = 0;
    for(Int32 i = 0; i < plainSize; ++i)
    {
        plainText[i] = cypherText[i] ^ key[j];
        j = (j + 1) >= keySize ? 0 : (j + 1);
    }

    return StatusDefs::Success;
}

FS_NAMESPACE_END
