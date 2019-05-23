/**
* @file StringUtil.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/
* @brief
*/

#include "stdafx.h"
#include "base/common/assist/utils/Impl/StringUtil.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/common.h"

#pragma region 3rd
#define __USE_FS_3RD_OPENSSL__
#include "3rd/3rd.h"
#pragma endregion

FS_NAMESPACE_BEGIN

bool StringUtil::MakeMd5(const FS_String &src, FS_String &outMd5)
{
    if(UNLIKELY(!src.GetLength()))
        return false;

    Byte8 BufferMd5[MD5_DIGEST_LENGTH] = {0};

    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, src.c_str(), static_cast<size_t>(src.GetLength()));
    MD5_Final((U8 *)(&(BufferMd5[0])), &c);
    outMd5.AppendBitData(BufferMd5, MD5_DIGEST_LENGTH);
    return true;
}

bool StringUtil::ToHexString(const FS_String &src, FS_String &outHexString)
{
    if(UNLIKELY(!src.GetLength()))
        return false;

    char cache[16] = {0};
    Int64 cacheLen = 0;
    const Int64 len = static_cast<Int64>(src.GetRaw().size());
    for(Int64 i = 0; i < len; ++i)
    {
        cacheLen = static_cast<Int64>(sprintf(cache, "%02x", static_cast<U8>(src[static_cast<Int32>(i)])));
        if(cacheLen < 0LL)
        {
            printf("sprintf fail src[%lld]:%d", i, src[static_cast<Int32>(i)]);
            continue;
        }
        outHexString << cache;
    }

    return true;
}

FS_NAMESPACE_END
