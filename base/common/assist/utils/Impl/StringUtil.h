/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
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
 * @file  : StringUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Assist_Utils_Impl_StringUtil_H__
#define __Base_Common_Assist_Utils_Impl_StringUtil_H__
/**
* @file StringUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/
* @brief
*/

#pragma once
#include<base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"
#include <stdlib.h>
#include "base/common/component/Impl/FS_String.h"

FS_NAMESPACE_BEGIN

class Time;

class BASE_EXPORT StringUtil
{
public:
    static Int32 StringToInt32(const char *str);
    static UInt32 StringToUInt32(const char *str);
    static Long StringToLong(const char *str);
    static ULong StringToULong(const char *str);
    static Int64 StringToInt64(const char *str);
    static UInt64 StringToUInt64(const char *str);
    static Double StringToDouble(const char *str);
    static FS_String ItoA(Int32 value, Int32 radix);
    static FS_String UItoA(UInt32 value, Int32 radix);
    static FS_String I64toA(Int64 value, Int32 radix);
    static FS_String UI64toA(UInt64 value, Int32 radix);

    template <typename T>
    static FS_String Num2Str(T val, Int32 radix = 10);
    static bool MakeMd5(const FS_String &src, FS_String &outMd5);
    static bool ToHexString(const FS_String &src, FS_String &outHexString);
    static void PreInstertTime(const Time &time, FS_String &src);

    static void SplitString(const FS_String &str,
                     const FS_String &separator,
                     std::vector<FS_String> &destStrList,
                     bool justSplitFirst = false,
                     char escapeChar = '\0');
    static FS_String FilterOutString(const FS_String &str, const FS_String &filterStr);
};

FS_NAMESPACE_END

#include "base/common/assist/utils/Impl/StringUtilImpl.h"

#endif
