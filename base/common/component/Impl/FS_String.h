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
 * @file  : FS_String.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_FS_String__H__
#define __Base_Common_Component_Impl_FS_String__H__


#pragma once

#include <vector>
#include <string>
#include "base/exportbase.h"
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN
class FS_String;
FS_NAMESPACE_END

extern BASE_EXPORT std::ostream &operator <<(std::ostream &stream, const fs::FS_String &);

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_String
{
public:
    typedef std::vector<FS_String> _These, FS_Strings;
    typedef std::string::size_type size_type;
    static const char *endl;

public:
    FS_String();
    virtual ~FS_String();
    
    FS_String(char other);
    FS_String(const std::string &other);
    FS_String(const char *other);
    FS_String &operator = (const Byte8 *str);
    FS_String &operator << (const FS_String &str);
    FS_String &operator << (const std::string &str);
    FS_String &operator << (const bool &val);
    FS_String &operator << (const Byte8 &val);
    FS_String &operator << (const U8 &val);
    FS_String &operator << (const Int16 &val);
    FS_String &operator << (const UInt16 &val);
    FS_String &operator << (const Int32 &val);
    FS_String &operator << (const UInt32 &val);
    FS_String &operator << (const Long &val);
    FS_String &operator << (const ULong &val);
    FS_String &operator << (const Int64 &val);
    FS_String &operator << (const UInt64 &val);
    FS_String &operator << (const Float &val);
    FS_String &operator << (const Double &val);
    FS_String &operator << (const Byte8 *val);
    FS_String &operator << (const void *addr);    // 
    FS_String &operator << (void *&&addr);    // 

    char &operator [] (Int32 index);    // no out range detect
    char operator [] (Int32 index) const;    // no out range detect

    bool operator == (const FS_String &index) const;
    FS_String operator + (const FS_String &other) const; // 
    FS_String operator + (const char *other) const; // 
    FS_String operator + (const std::string &other) const; // 

    bool operator <(const FS_String &right) const;
    explicit operator std::string &();
    explicit operator const std::string &() const;

    size_t CopyTo(char *destData, Int32 destSize, Int32 cntToCopy, Int32 srcOffset = 0) const;
    const char *c_str() const;
    bool empty() const;
    size_t size() const;
    std::string &GetRaw();
    const std::string &GetRaw() const;
    size_t GetLength() const;
    void Clear();
    void Swap(FS_String &&str);
    FS_String ToHexString() const;

    FS_String &AppendBitData(const char *bitData, Int64 dataSize);
    _These Split(char sep, size_type max_split = -1) const;    // only ascii
    _These Split(const char *sep, size_type max_split = -1) const;    // only ascii
    _These Split(const FS_String &sep, std::string::size_type max_split = -1, bool onlyLikely = false) const;
    _These Split(const _These &seps, size_type max_split = -1) const;

    #pragma region UTF8 Surport
    void add_utf8_bomb();
    std::string::size_type length_with_utf8() const;
    std::string substr_with_utf8(std::string::size_type pos = 0, std::string::size_type n = std::string::npos) const;
    void split_utf8_string(Long charIndex, _These &strs) const;
    void scatter_utf8_string(_These &chars, std::string::size_type scatterCount = 0) const;
    bool has_utf8_bomb() const;
    void remove_utf8_bomb();
private:
    std::string::size_type _next_utf8_char_pos(std::string::size_type &beginBytePos) const;
    #pragma endregion

    #pragma region format
public:
    template<typename... Args>
    FS_String &Format(const char *fmt, Args&&... rest);
    template< typename... Args>
    FS_String &Append(Args&&... rest);
    FS_String &FormatCStyle(const char *fmt, ...);

    static UInt64 CalcPlaceHolderNum(const std::string &fmtStr);
    static bool AppendFirstValidPlaceHolderString(const std::string &fmt, UInt64 &firstIndex, std::string &outStr);
    static std::string NextPlaceHolderPos(const std::string &strFmt, UInt64 startIndex, UInt64 &outIndex);
private:
    template<typename T>
    void _AppendFormat(const std::string &strLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, T &&val);
    template<typename T, typename... Args>
    void _AppendFormat(const std::string &strLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, T &&val, Args&&... rest);   //Args&...
    template<typename T>
    void _DoAppendFormat(const std::string &strLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, T &&val);
    template< typename T>
    void _Append(T &&arg);
    template<typename T, typename... Args>
    void _Append(T &&arg, Args&&... rest);
    #pragma endregion

private:
    template<typename T>
    void _append_fmt_str(const Byte8 *fmtstr, const T &val);
    template<typename T>
    void _append_fmt_str(const Byte8 *fmtstr, Int16 bufferSize, const T &val);
    void _append_fmt_str_cstyle(const Byte8 *fmtstr, ...);

private:
    std::string _buffer;
    BUFFER32 _cache;
};

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

inline char FS_String::operator [] (Int32 index) const
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
#pragma endregion

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_StringImpl.h"

#endif // !__Base_Common_Component_Impl_FS_String__H__
