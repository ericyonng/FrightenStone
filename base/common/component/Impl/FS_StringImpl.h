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
 * @file  : FS_StringImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_FS_String__H__


#pragma once

#include "base/common/component/Impl/BufferAddapter.h"
#include "base/common/assist/assistobjs/assistobjs.h"
#include "base/common/component/Impl/FS_ArgsExpander.h"

FS_NAMESPACE_BEGIN

inline FS_String::FS_String()
{
    _cache[0] = 0;
}

inline FS_String::~FS_String()
{

}

inline FS_String::FS_String(char other)
{
    _buffer = other;
    _cache[0] = 0;
}

inline FS_String::FS_String(const std::string &other)
    :_buffer(other)
{
    _cache[0] = 0;
}

inline FS_String::FS_String(const char *other)
{
    _buffer = other;
    _cache[0] = 0;
}

inline FS_String::FS_String(const FS_String &other)
{
    _buffer = other._buffer;
    _cache[0] = 0;
}

#pragma region << 
inline FS_String &FS_String::operator = (const Byte8 *str)
{
    _buffer = str;
    return *this;
}

inline FS_String &FS_String::operator << (const FS_String &str)
{
    _buffer += str._buffer;
    return *this;
}

inline FS_String &FS_String::operator << (const std::string &str)
{
    _buffer += str;
    return *this;
}

inline FS_String &FS_String::operator << (const bool &val)
{
    _append_fmt_str("%d", val);
    return *this;
}

inline FS_String &FS_String::operator << (const Byte8 &val)
{
    _append_fmt_str("%c", val);
    return *this;
}

inline FS_String &FS_String::operator << (const U8 &val)
{
    _append_fmt_str("%hhu", val);
    return *this;
}

inline FS_String &FS_String::operator << (const Int16 &val)
{
    _append_fmt_str("%hd", val);
    return *this;
}

inline FS_String &FS_String::operator << (const UInt16 &val)
{
    _append_fmt_str("%hu", val);
    return *this;
}

inline FS_String &FS_String::operator << (const Int32 &val)
{
    _append_fmt_str("%d", val);
    return *this;
}

inline FS_String &FS_String::operator << (const UInt32 &val)
{
    _append_fmt_str("%u", val);
    return *this;
}

inline FS_String &FS_String::operator<<(const Long &val)
{
    _append_fmt_str("%ld", val);
    return *this;
}

inline FS_String &FS_String::operator<<(const ULong &val)
{
    _append_fmt_str("%lu", val);
    return *this;
}

inline FS_String &FS_String::operator<<(const Int64 &val)
{
    _append_fmt_str("%lld", val);
    return *this;
}

inline FS_String &FS_String::operator << (const UInt64 &val)
{
    _append_fmt_str("%llu", val);
    return *this;
}

inline FS_String &FS_String::operator << (const Float &val)
{
    _append_fmt_str_cstyle("%f", val);
    return *this;
}

inline FS_String &FS_String::operator << (const Double &val)
{
    _append_fmt_str_cstyle("%lf", val);
    return *this;
}

inline FS_String &FS_String::operator << (const Byte8 *val)
{
    if(UNLIKELY(!val))
        return *this;

    _buffer += val;
    return *this;
}

inline FS_String &FS_String::operator << (const void *addr)
{
    _append_fmt_str("%p", addr);
    return *this;
}

inline FS_String &FS_String::operator << (void *&&addr)
{
    _append_fmt_str("%p", addr);
    return *this;
}

inline char &FS_String::operator [] (Int32 index)
{
    return _buffer[index];
}

inline const char &FS_String::operator [] (Int32 index) const
{
    return _buffer[index];
}

inline char &FS_String::operator [] (size_type index)
{
    return _buffer[index];
}

inline const char &FS_String::operator [] (size_type index) const
{
    return _buffer[index];
}

inline bool FS_String::operator == (const FS_String &index) const
{
    return _buffer == index._buffer;
}

inline FS_String FS_String::operator + (const FS_String &other) const
{
    return _buffer + other._buffer;
}

inline FS_String FS_String::operator + (const char *other) const
{
    return _buffer + other;
}

inline FS_String FS_String::operator + (const std::string &other) const
{
    return _buffer + other;
}

inline FS_String &FS_String::operator += (const FS_String &other)
{
    _buffer += other._buffer;
    return *this;
}

inline FS_String &FS_String::operator += (const char *other)
{
    _buffer += other;
    return *this;
}

inline FS_String &FS_String::operator += (const std::string &other)
{
    _buffer += other;
    return *this;
}
#pragma endregion

#pragma region Inline
inline bool FS_String::operator <(const FS_String &right) const
{
    return _buffer < right._buffer;
}

inline FS_String::operator std::string &()
{
    return _buffer;
}

inline FS_String::operator const std::string &() const
{
    return _buffer;
}

inline size_t FS_String::CopyTo(char *destData, Int32 destSize, Int32 cntToCopy, Int32 srcOffset) const
{
    return _buffer._Copy_s(destData, destSize, cntToCopy, srcOffset);
}

inline const char *FS_String::c_str() const
{
    return _buffer.c_str();
}

inline bool FS_String::empty() const
{
    return _buffer.empty();
}

inline size_t FS_String::size() const
{
    return _buffer.size();
}

inline std::string &FS_String::GetRaw()
{
    return _buffer;
}

inline const std::string &FS_String::GetRaw() const
{
    return _buffer;
}

inline size_t FS_String::GetLength() const
{
    return _buffer.length();
}

inline void FS_String::Clear()
{
    _buffer.clear();
    _cache[0] = 0;
}

inline void FS_String::Swap(FS_String &&str)
{
    _buffer.swap(str._buffer);
}

inline FS_String &FS_String::AppendBitData(const char *bitData, Int64 dataSize)
{
    _buffer.append(bitData, dataSize);
    return *this;
}

inline bool FS_String::isalpha(const char &c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

inline bool FS_String::isalpha() const
{
    return isalpha(*this);
}

inline bool FS_String::islower(const char &c)
{
    return 'a' <= c && c <= 'z';
}

inline bool FS_String::islower() const
{
    return islower(*this);
}

inline bool FS_String::isupper(const char &c)
{
    return 'A' <= c && c <= 'Z';
}

inline bool FS_String::isupper() const
{
    return isupper(*this);
}

inline bool FS_String::isdigit(const char &c)
{
    return '0' <= c && c <= '9';
}

inline bool FS_String::isdigit() const
{
    return isdigit(*this);
}

inline bool FS_String::isspace(const char &c)
{
    return  c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n' || c == '\f';
}

inline bool FS_String::isspace() const
{
    return isspace(*this);
}

#pragma endregion

template<typename T>
inline void FS_String::_append_fmt_str(const Byte8 *fmtstr, const T &val)
{
    auto len = sprintf(_cache, fmtstr, val);
    _cache[len] = 0;
    _buffer += _cache;
}

template<typename T>
void FS_String::_append_fmt_str(const Byte8 *fmtstr, Int16 bufferSize, const T &val)
{
    Byte8 *bufferCache = new Byte8[bufferSize];
    bufferCache[0] = 0;
    auto len = sprintf(bufferCache, fmtstr, val);
    bufferCache[len] = 0;
    _buffer += static_cast<char *>(bufferCache);
    delete[]bufferCache;
}

template<typename... Args>
inline FS_String &FS_String::Format(const char *fmt, Args&&... rest)
{
    return FormatCStyle(fmt, std::forward<Args>(rest)...);

//     UInt64 firstIndex = 0, nextIndex = 0;
//     AppendFirstValidPlaceHolderString(fmt, firstIndex, _buffer);
//     NextPlaceHolderPos(fmt, firstIndex, nextIndex);
// 
//     //
//     if(firstIndex != std::string::npos)
//     {
//         FS_String str;
//         _AppendFormat(std::move(fmt), firstIndex, nextIndex, str, std::forward<Args>(rest)...);
//         _buffer += str.c_str();
//     }
//     else
//     {
//         AppendNoFmtStr(std::forward<Args>(rest)...);
//     }

//    return *this;
}

template<typename... Args>
inline FS_String &FS_String::Append(Args&&... rest)
{
    char expandArray[] = {((*this) << rest, 0)...};
    return *this;
}

template<typename... Args>
inline FS_String &FS_String::FormatCompatibilityNoFmt(const std::string &fmt, const Args&... rest)
{
    UInt64 firstIndex = 0, nextIndex = 0;

    // 初始化格式字符串
    firstIndex = InitFmtToOutStrFirst(fmt, _buffer);
    NextPlaceHolderPos(fmt, firstIndex, nextIndex);

    // 判断需不需要解析
    if(firstIndex != std::string::npos)
    {
        //解析
        _AppendFormat(fmt, firstIndex, nextIndex, *this, rest...);
        return *this;
    }

    // 不用解析控制字符
    return Append(rest...);
}

template<typename T>
inline void FS_String::_AppendFormat(const std::string &fmtLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, T &&val)
{
    _DoAppendFormat(fmtLeft, startPlaceHolderIndex, nextPlaceHolderIndex, obj, val);
}

template<typename T, typename... Args>
inline void FS_String::_AppendFormat(const std::string &fmtLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, T &&val, Args&&... rest)
{
    _DoAppendFormat(fmtLeft, startPlaceHolderIndex, nextPlaceHolderIndex, obj, val);
    _AppendFormat(fmtLeft, startPlaceHolderIndex, nextPlaceHolderIndex, obj, std::forward<Args>(rest)...);
}

template<typename T>
inline void FS_String::_DoAppendFormat(const std::string &fmtLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, T &&val)
{
    if(startPlaceHolderIndex == std::string::npos)
    {
        obj << val;
        return;
    }

    const std::string &strCache = (nextPlaceHolderIndex != std::string::npos) ? \
        fmtLeft.substr(startPlaceHolderIndex, nextPlaceHolderIndex - startPlaceHolderIndex):\
        fmtLeft.substr(startPlaceHolderIndex);

    {
        UInt64 bufferSize = strCache.length() + 1 + GetBufferAddapterSize<T>::GetBufferNeeded(std::forward<T>(val)) + 1;
        fs::SmartPtr<Byte8, AssistObjsDefs::MultiDelete> bufferCache = new char[bufferSize];

        auto len = sprintf(static_cast<char *>(bufferCache), strCache.c_str(), val);
        bufferCache[len] = 0;
        _buffer += static_cast<char *>(bufferCache);
    }
     
    startPlaceHolderIndex = nextPlaceHolderIndex;   //
    NextPlaceHolderPos(fmtLeft, startPlaceHolderIndex, nextPlaceHolderIndex);
}

template< typename T>
inline void FS_String::_Append(T &&arg)
{
    (*this) << arg;
}
template<typename T, typename... Args>
inline void FS_String::_Append(T &&arg, Args&&... rest)
{
    (*this) << arg;
    _Append(std::forward<Args>(rest)...);
}

// template<typename T>
// inline void FS_String::_AppendNoFmt(T &&val)
// {
//     (*this) << val;
// }

FS_NAMESPACE_END

#endif
