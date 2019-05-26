#include "base/common/basedefs/BaseDefs.h"

enum AES_CYPHER_T_ 
{
    AES_CYPHER_128 = 0,
    AES_CYPHER_192,
    AES_CYPHER_256,
};

int aes_encrypt_ecb(Int32 mode, U8 *data, int len, U8 *key);
int aes_decrypt_ecb(Int32 mode, U8 *data, int len, U8 *key);
int aes_encrypt_cbc(Int32 mode, U8 *data, int len, U8 *key, U8 *iv);
int aes_decrypt_cbc(Int32 mode, U8 *data, int len, U8 *key, U8 *iv);
