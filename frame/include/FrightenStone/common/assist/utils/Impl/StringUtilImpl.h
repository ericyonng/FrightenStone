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
 * @file  : StringUtilImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/06/03
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Assist_Utils_Impl_StringUtil_H__
#pragma once

FS_NAMESPACE_BEGIN

#pragma region Inline
inline Int32 StringUtil::StringToInt32(const char *str)
{
    return ::atoi(str);
}

inline UInt32 StringUtil::StringToUInt32(const char *str)
{
    return static_cast<UInt32>(StringToInt32(str));
}

inline Int16 StringUtil::StringToInt16(const char *str)
{
    return static_cast<Int16>(StringToInt32(str));
}

inline UInt16 StringUtil::StringToUInt16(const char *str)
{
    return static_cast<UInt16>(StringToInt32(str));
}

inline Long StringUtil::StringToLong(const char *str)
{
    return ::atol(str);
}

inline ULong StringUtil::StringToULong(const char *str)
{
    return static_cast<ULong>(StringToLong(str));
}

inline Int64 StringUtil::StringToInt64(const char *str)
{
#ifndef _WIN32
    return ::atoll(str);
#else
    return ::_atoi64(str);
#endif
}

inline UInt64 StringUtil::StringToUInt64(const char *str)
{
    return static_cast<UInt64>(StringToInt64(str));
}

inline Double StringUtil::StringToDouble(const char *str)
{
#ifndef _WIN32
    return ::atof(str);
#else
    return ::atof(str);
#endif
}

inline FS_String StringUtil::ItoA(Int32 value, Int32 radix)
{
    return I64toA(value, radix);
}

inline FS_String StringUtil::UItoA(UInt32 value, Int32 radix)
{
    return UI64toA(value, radix);
}

#pragma endregion

template <>
inline FS_String StringUtil::Num2Str(Int64 val, Int32 radix)
{
    return I64toA(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(UInt64 val, Int32 radix)
{
    return UI64toA(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(Int32 val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(UInt32 val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(Int16 val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(UInt16 val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(Byte8 val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(U8 val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(Long val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(ULong val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline FS_String StringUtil::Num2Str(Double val, Int32 radix)
{
    char buf[64] = {0};

#ifndef _WIN32
    sprintf(buf, "%f", val);
#else // LLBC_TARGET_PLATFORM_WIN32
    sprintf_s(buf, sizeof(buf), "%f", val);
#endif // LLBC_TARGET_PLATFORM_NON_WIN32
    return buf;
}

template <>
inline FS_String StringUtil::Num2Str(Float val, Int32 radix)
{
    return Num2Str<Double>(val, radix);
}

template <typename ObjType>
inline FS_String StringUtil::Num2Str(ObjType val, Int32 radix)
{
    if(radix != 10 && radix != 16)
        radix = 10;

    FS_String str;
    if(radix == 16)
        str.GetRaw() += "0x";

    UInt64 ptrVal = 0;
    ::memcpy(&ptrVal, &val, sizeof(ObjType) > sizeof(UInt64) ? sizeof(UInt64) : sizeof(ObjType));
    return (str + Num2Str<UInt64>(ptrVal, radix));
}

FS_NAMESPACE_END

#endif
