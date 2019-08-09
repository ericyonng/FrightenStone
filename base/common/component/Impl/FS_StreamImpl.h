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
 * @file  : FS_StreamImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/9
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_FS_Stream_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_Stream::FS_Stream(Byte8 *data, Int32 size, bool needDelete)
    :_buff(data)
    , _size(size)
    , _needDelete(needDelete)
{
}

#pragma region assist
inline char *FS_Stream::GetData()
{
    return _buff;
}

Int32 FS_Stream::GetWrLength() const
{
    return _writePos;
}

inline bool FS_Stream::CanRead(Int32 len) const
{
    return _size - _readPos >= len;
}

inline bool FS_Stream::CanWrite(Int32 len) const
{
     return _size - _writePos >= len;
}

inline void FS_Stream::OffsetWrLenOnWrChange(Int32 len)
{
     _writePos += len;
}

inline void FS_Stream::OffsetOnReadChange(Int32 len)
{
    _readPos += len;
}

inline void FS_Stream::SetWritePos(Int32 n)
{
    _writePos = n;
}
#pragma endregion

#pragma region read
template<typename T>
inline bool FS_Stream::Read(T &n, bool isOffset)
{
    // 计算要读取数据的字节长度
    Int32 objSize = static_cast<Int32>(sizeof(T));

    // 判断能不能读
    if(CanRead(objSize))
    {
        // 将要读取的数据 拷贝出来
        memcpy(&n, _buff + _readPos, objSize);

        // 偏移已读数据位置
        if(isOffset)
            OffsetOnReadChange(objSize);

        return true;
    }

    g_Log->e<FS_Stream>(_LOGFMT_("error, Cant Read. struct name[%s]."), typeid(T).name());
    return false;
}

template<typename T>
inline UInt32 FS_Stream::ReadArray(T *arr, UInt32 len)
{
    UInt32 elementCnt = 0;

    // 读取数组元素个数,但不偏移读取位置
    Read(elementCnt, false);

    // 判断缓存数组能否放得下
    if(elementCnt <= len)
    {
        // 计算数组的字节长度
        Int32 bytes = static_cast<Int32>(elementCnt * sizeof(T));

        // 判断能不能读出
        if( CanRead(static_cast<Int32>(bytes + sizeof(UInt32))) )
        {
            // 计算已读位置+数组长度所占有空间
            OffsetOnReadChange(static_cast<Int32>(sizeof(UInt32)));

            // 将要读取的数据 拷贝出来
            memcpy(arr, _buff + _readPos, static_cast<size_t>(bytes));

            // 计算已读数据位置
            OffsetOnReadChange(static_cast<Int32>(bytes));
            return elementCnt;
        }
    }

    g_Log->e<FS_Stream>(_LOGFMT_("error,ReadArray failed. element name[%s]."), typeid(T).name());
    return 0;
}

template<typename T>
inline bool FS_Stream::ReadWithoutOffsetPos(T &n)
{
    return Read(n, false);
}

inline Byte8 FS_Stream::ReadInt8()
{
    Byte8 def = 0;
    Read(def);
    return def;
}

inline Int16 FS_Stream::ReadInt16()
{
    Int16 n = 0;
    Read(n);
    return n;
}


inline Int32 FS_Stream::ReadInt32()
{
    Int32 n = 0;
    Read(n);
    return n;
}

inline Int64 FS_Stream::ReadInt64()
{
    Int64 n = 0;
    Read(n);
    return n;
}

inline U8 FS_Stream::ReadUInt8()
{
    U8 def = 0;
    Read(def);
    return def;
}

inline UInt16 FS_Stream::ReadUInt16()
{
    UInt16 n = 0;
    Read(n);
    return n;
}

inline UInt32 FS_Stream::ReadUInt32()
{
    UInt32 n = 0;
    Read(n);
    return n;
}

inline UInt64 FS_Stream::ReadUInt64()
{
    UInt64 n = 0;
    Read(n);
    return n;
}

inline Float FS_Stream::ReadFloat()
{
    Float n = 0.0f;
    Read(n);
    return n;
}

inline Double FS_Stream::ReadDouble()
{
    Double n = 0.0;
    Read(n);
    return n;
}

inline bool FS_Stream::ReadString(FS_String &str)
{
    return ReadString(str.GetRaw());
}

#pragma endregion

#pragma region Write

template<typename T>
inline bool FS_Stream::Write(const T &n)
{
    // 计算要写入数据的字节长度
    Int32 len = static_cast<Int32>(sizeof(T));

    // 判断能不能写入
    if(CanWrite(len))
    {
        // 将要写入的数据 拷贝到缓冲区尾部
        memcpy(_buff + _writePos, &n, static_cast<size_t>(len));

        // 计算已写入数据尾部位置
        OffsetWrLenOnWrChange(len);
        return true;
    }

    g_Log->e<FS_Stream>(_LOGFMT_("Write failed. obj name[%s]"), typeid(T).name());
    return false;
}

template<typename T>
inline bool FS_Stream::WriteArray(T *data, UInt32 len)
{
    // 计算要写入数组的字节长度
    auto bytes = sizeof(T)*len;

    // 判断能不能写入
    if(CanWrite(bytes + sizeof(UInt32)))
    {
        // 先写入数组的元素长度
        Write(len);

        // 将要写入的数据 拷贝到缓冲区尾部
        memcpy(_buff + _writePos, data, bytes);

        // 计算数据尾部位置
        OffsetWrLenOnWrChange(bytes);
        return true;
    }

    g_Log->e<FS_Stream>(_LOGFMT_("WriteArray failed. obj name[%s]"), typeid(T).name());
    return false;
}

inline bool FS_Stream::WriteInt8(Byte8 n)
{
    return Write(n);
}

inline bool FS_Stream::WriteInt16(Int16 n)
{
    return Write(n);
}

inline bool FS_Stream::WriteInt32(Int32 n)
{
    return Write(n);
}

inline bool FS_Stream::WriteFloat(Float n)
{
    return Write(n);
}

inline bool FS_Stream::WriteDouble(Double n)
{
    return Write(n);
}
#pragma endregion

FS_NAMESPACE_END

#endif
