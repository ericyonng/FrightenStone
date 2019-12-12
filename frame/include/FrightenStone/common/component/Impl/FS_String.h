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
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_FS_String_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_FS_String_H__


#pragma once

#include <vector>
#include <string>
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/DataType/DataType.h"
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
#include "FrightenStone/common/memorypool/memorypool.h"

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
    static const std::string _defStripChars;

public:
    FS_String();
    virtual ~FS_String();
    
    FS_String(char other);
    FS_String(const std::string &other);
    FS_String(const char *other);
    FS_String(const FS_String &other);
    FS_String &operator = (const Byte8 *str);
    FS_String &operator = (const std::string &str);
    FS_String &operator = (const FS_String &str);

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
    const char &operator [] (Int32 index) const;    // no out range detect
    char &operator [] (size_type index);    // no out range detect
    const char &operator [] (size_type index) const;    // no out range detect

    bool operator == (const FS_String &index) const;
    bool operator == (const char *str) const;
    bool operator != (const char *str) const;
    bool operator != (const FS_String &str) const;
    FS_String operator + (const FS_String &other) const; // 
    FS_String operator + (const char *other) const; // 
    FS_String operator + (const std::string &other) const; // 

    FS_String &operator += (const FS_String &other); // 
    FS_String &operator += (const char *other); // 
    FS_String &operator += (const std::string &other); // 

    FS_String &operator *=(size_t right);

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
    void CompressString();      // 去除字符串末尾的\0压缩空间只对0结尾的字符串有效
    const FS_String &RemoveZeroTail();      // 移除容器中尾部的0
    const FS_String &RemoveHeadZero();      // 移除容器中首部的0

    FS_String &AppendBitData(const char *bitData, Int64 dataSize);
    _These Split(char sep, size_type max_split = -1) const;    // only ascii max_split分割的次数，一次分割两块数据一共分割max_split+1块数据
    _These Split(const char *sep, size_type max_split = -1) const;    // only ascii
    _These Split(const FS_String &sep, std::string::size_type max_split = -1, bool onlyLikely = false) const;
    _These Split(const _These &seps, size_type max_split = -1) const;

    // strip operation: strip left. 去首部连续字符
    FS_String &lstrip(const FS_String &chars = FS_String());
    FS_String lstrip(const FS_String &chars = FS_String()) const;

    // strip operation: strip right. 去尾部连续字符
    FS_String &rstrip(const FS_String &chars = FS_String());
    FS_String rstrip(const FS_String &chars = FS_String()) const;

    // strip operation: 去除首尾字符
    FS_String &strip(const FS_String &chars = FS_String());
    FS_String strip(const FS_String &chars = FS_String()) const;

    // isalpha/isupper/islower 是否字母
    static bool isalpha(const char &c);
    static bool isalpha(const FS_String &s);
    bool isalpha() const;
    static bool islower(const char &c);
    static bool islower(const FS_String &s);
    bool islower() const;
    static bool isupper(const char &c);
    static bool isupper(const FS_String &s);
    bool isupper() const;

    // 是否数值
    static bool isdigit(const char &c);
    static bool isdigit(const FS_String &s);
    bool isdigit() const;

    // isspace: space[' '], carriage return['\r'], line feed['\n'], form feed['\f'], horizontal tab['\t'], vertical tab['\v']
    static bool isspace(const char &c);
    static bool isspace(const FS_String &s);
    bool isspace() const;

    // startswith/endswith
    bool IsStartsWith(const FS_String &s) const;
    bool IsEndsWith(const FS_String &s) const;

    // tolower/toupper operations. 请确保是英文字符串 isalpha
    FS_String tolower() const;
    FS_String toupper() const;

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
    FS_String &AppendFormat(const char *fmt, Args&&... rest);
    template< typename... Args>
    FS_String &Append(Args&&... rest);
    FS_String &AppendFormatCStyle(const char *fmt, ...);

    // 兼容格式字符串中没有占位符情况
    template<typename... Args>
    FS_String &FormatCompatibilityNoFmt(const std::string &fmt, const Args&... rest);

    // 计算占位符个数
    static UInt64 CalcPlaceHolderNum(const std::string &fmtStr);
    // 获取第一个占位符位置
    static UInt64 GetFirstValidPlaceHolderIndex(const std::string &fmt);
    // 讲格式符初始化给outStr，解析时必须做的 返回第一个占位符位置
    static UInt64 InitFmtToOutStrFirst(const std::string &fmt, std::string &outStr);
    // startIndex 必须为当前要格式化的%所在位置 NextPlaceHolderPos输出下一个未被格式化的位置outIndex 输出std::string::npos表示结束
    static std::string NextPlaceHolderPos(const std::string &strFmt, UInt64 startIndex, UInt64 &outIndex);
private:
    template<typename ObjType>
    void _AppendFormat(const std::string &fmtLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, ObjType &&val);
    template<typename ObjType, typename... Args>
    void _AppendFormat(const std::string &fmtLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, ObjType &&val, Args&&... rest);   //Args&...
    template<typename ObjType>
    void _DoAppendFormat(const std::string &fmtLeft, UInt64 &startPlaceHolderIndex, UInt64 &nextPlaceHolderIndex, FS_String &obj, ObjType &&val);
    template< typename ObjType>
    void _Append(ObjType &&arg);
    template<typename ObjType, typename... Args>
    void _Append(ObjType &&arg, Args&&... rest);
    #pragma endregion

private:
    template<typename ObjType>
    void _append_fmt_str(const Byte8 *fmtstr, const ObjType &val);
    template<typename ObjType>
    void _append_fmt_str(const Byte8 *fmtstr, Int16 bufferSize, const ObjType &val);
    void _append_fmt_str_cstyle(const Byte8 *fmtstr, ...);

private:
    std::string _buffer;
    BUFFER32 _cache;
};

FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/FS_StringImpl.h"

#endif // !__Frame_Include_FrightenStone_Common_Component_Impl_FS_String_H__
