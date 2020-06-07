/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : StringUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/assist/utils/Impl/StringUtil.h"
#include "FrightenStone/common/common.h"

#pragma region 3rd
#define __USE_FS_3RD_OPENSSL__
#include "3rd/3rd.h"
#pragma endregion

FS_NAMESPACE_BEGIN

FS_String StringUtil::I64toA(Int64 value, Int32 radix)
{
    char *p;
    char *firstDigit;
    char temp;
    unsigned int digval;
    char buf[64] = {0};

    p = buf;
    firstDigit = p;

    if(value < 0)
    {
        p[0] = '-';
        firstDigit = ++p;

        value = -value;
    }

    do
    {
        digval = (unsigned int)(value % radix);
        value /= radix;

        if(digval > 9)
            *p++ = (char)(digval - 10 + 'a');
        else
            *p++ = (char)(digval + '0');
    } while(value > 0);

    *p-- = '\0';

    do
    {
        temp = *p;
        *p = *firstDigit;
        *firstDigit = temp;

        --p;
        ++firstDigit;
    } while(firstDigit < p);

    return buf;
}

FS_String StringUtil::UI64toA(UInt64 value, Int32 radix)
{
    char *p;
    char *firstDigit;
    char temp;
    unsigned int digval;
    char buf[64] = {0};

    p = buf;
    firstDigit = p;

    do
    {
        digval = (unsigned int)(value % radix);
        value /= radix;

        if(digval > 9)
            *p++ = (char)(digval - 10 + 'a');
        else
            *p++ = (char)(digval + '0');
    } while(value > 0);

    *p-- = '\0';

    do
    {
        temp = *p;
        *p = *firstDigit;
        *firstDigit = temp;

        --p;
        ++firstDigit;
    } while(firstDigit < p);

    return buf;
}

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

bool StringUtil::ToHexString(const Byte8 *buff, Int64 len, FS_String &outHexString)
{
    if (UNLIKELY(!len))
        return false;

    // 每16字节一行
    char cache[16] = { 0 };
    Int64 cacheLen = 0;
    for (Int64 i = 0; i < len; ++i)
    {
        cacheLen = static_cast<Int64>(sprintf(cache, "%02x%s"
            , static_cast<U8>(buff[static_cast<Int32>(i)]), ((i + 1) % 16 == 0) ? "\n" : " "));
        if (cacheLen < 0LL)
        {
            printf("sprintf fail src[%lld]:%d", i, buff[static_cast<Int32>(i)]);
            continue;
        }
        outHexString << cache;
    }

    return true;
}

void StringUtil::PreInstertTime(const Time &time, FS_String &src)
{
    src << time.ToStringOfMillSecondPrecision();
}

void StringUtil::SplitString(const FS_String &str, const FS_String &separator, std::vector<FS_String> &destStrList, bool justSplitFirst /*= false*/, char escapeChar /*= '\0'*/)
{
    if(UNLIKELY(str.empty()))
    {
        return;
    }

    if(UNLIKELY(separator.empty()))
    {
        destStrList.push_back(str);
    }

    FS_String::size_type curPos = 0;
    FS_String::size_type prevPos = 0;

    FS_String strInternal = str;
    while((curPos = strInternal.GetRaw().find(separator.GetRaw(), curPos)) != std::string::npos)
    {
        if(curPos != 0 && strInternal[curPos - 1] == escapeChar)
        {
            strInternal.GetRaw().erase(--curPos, 1);
            curPos += separator.size();
            continue;
        }

        FS_String temp = strInternal.GetRaw().substr(prevPos, curPos - prevPos);
        destStrList.push_back(temp);

        if(justSplitFirst)
        {
            destStrList.push_back(strInternal.GetRaw().substr(curPos + separator.size()));
            return;
        }

        curPos += separator.size();
        prevPos = curPos;
    }

    FS_String temp = strInternal.GetRaw().substr(prevPos);
    if(!temp.empty())
    {
        destStrList.push_back(temp);
    }
}

FS_String StringUtil::FilterOutString(const FS_String &str, const FS_String &filterStr)
{
    std::vector<FS_String> strings;
    SplitString(str, filterStr, strings);

    FS_String retStr;
    for(size_t i = 0; i < strings.size(); i++)
    {
        retStr += strings[i];
    }

    return retStr;
}

FS_NAMESPACE_END
