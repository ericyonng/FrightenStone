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
 * @file  : FS_Stream.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/9
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_FS_Stream_H__
#define __Base_Common_Component_Impl_FS_Stream_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/objpool/objpool.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/log/Log.h"
#include "base/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN

// 字节流写入规则：先写入uint32的数据长度，再写入数据
// 字节流读取规则：先读取uint32的数据长度，再读取数据
// 若是由内存池创建的缓冲区，在写入时支持缓冲区自增长
class BASE_EXPORT FS_Stream
{
    OBJ_POOL_CREATE(FS_Stream, _objPoolHelper);
public:
    FS_Stream();
    FS_Stream(Byte8 *data, Int32 size, bool needDelete = false, bool isPoolCreate = false);
    FS_Stream(Int32 size);
    virtual ~FS_Stream();
    virtual void Release();

    /* 辅助 */
    #pragma region assist
    /*
    *   brief:
    *       1. - GetDataAddr 获取数据缓冲区变量的地址（由于数据缓冲区会自动扩展空间所以需要取得空间所在地址）
    *       2. - GetWrLength 已写入数据缓冲区的长度
    *       3. - CanRead 还能读出len字节的数据吗?
    *       4. - CanWrite 还能写入len字节的数据吗?
    *       5. - OffsetWrLenOnWrChange 已写入位置，写入位置_writePos 偏移len字节长度
    *       6. - OffsetOnReadChange 已读取位置，读取位置 _readPos 偏移len字节长度
    *       7. - SetWritePos 设置写入位置_writePos
    */
public:
    char **GetDataAddr();
    Int32 GetWrLength() const;
    bool CanRead(Int32 len) const;
    bool CanWrite(Int32 len) const;
    void OffsetWrLenOnWrChange(Int32 len);
    void OffsetOnReadChange(Int32 len);
    void SetWritePos(Int32 n);
    template<typename StreamObjType>
    StreamObjType *CastTo();
    template<typename StreamObjType>
    const StreamObjType *CastTo() const;

    // 序列化反序列化（ObjType 必须有bool SerializeTo(FS_Stream *) const, bool DeserializeFrom(FS_Stream *)接口）
    template<typename ObjType>
    bool SerializeFrom(const ObjType &obj);
    template<typename ObjType>
    bool DeserializeTo(ObjType &obj);

    // 字节流序列化反序列化
    bool SerializeTo(FS_String &str) const;
    bool DeserializeFrom(const FS_String &str);
    #pragma endregion

    /* 读字节流 */
    #pragma region Read
public:
    template<typename ObjType>
    bool Read(ObjType &n, bool isOffset = true);
    template<typename ObjType>
    UInt32 ReadArray(ObjType *arr, UInt32 len);
    template<typename ObjType>
    bool ReadWithoutOffsetPos(ObjType &n);

    Byte8 ReadInt8();
    Int16 ReadInt16();
    Int32 ReadInt32();
    Int64 ReadInt64();
    U8    ReadUInt8();
    UInt16 ReadUInt16();
    UInt32 ReadUInt32();
    UInt64 ReadUInt64();
    Float ReadFloat();
    Double ReadDouble();
    bool ReadString(std::string &str);
    bool ReadString(FS_String &str);
    #pragma endregion

    /* 写字节流 */
    #pragma region Write
public:
    template<typename ObjType>
    bool Write(const ObjType &n);
    template<typename ObjType>
    bool WriteArray(ObjType *data, UInt32 len);
    bool Write(const FS_String &str);
    bool Write(const std::string &str);
    bool WriteInt8(Byte8 n);
    bool WriteInt16(Int16 n);
    bool WriteInt32(Int32 n);
    bool WriteFloat(Float n);
    bool WriteDouble(Double n);
    #pragma endregion

private:
    void _Clear();

private:
    // 缓冲区总的空间大小，字节长度
    Int32 _size = 0;
    // 已写入数据的尾部位置，已写入数据长度
    Int32 _writePos = 0;
    // 已读取数据的尾部位置
    Int32 _readPos = 0;
    // _buff是外部传入的数据块时是否应该被释放
    bool _needDelete = true;
    bool _isPoolCreate = false;
    // 数据缓冲区
    char *_buff = NULL;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_StreamImpl.h"

#endif
