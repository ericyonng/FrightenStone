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

FS_NAMESPACE_BEGIN

// 字节流写入规则：先写入uint32的数据长度，再写入数据
// 字节流读取规则：先读取uint32的数据长度，再读取数据
class BASE_EXPORT FS_Stream
{
    OBJ_POOL_CREATE(FS_Stream, _objPoolHelper);
public:
    FS_Stream(Byte8 *data, Int32 size, bool needDelete = false);
    FS_Stream(Int32 size = 1024);
    virtual ~FS_Stream();

    /* 辅助 */
    #pragma region assist
    /*
    *   brief:
    *       1. - GetData 获取数据缓冲区
    *       2. - GetWrLength 已写入数据缓冲区的长度
    *       1. - CanRead 还能读出len字节的数据吗?
    *       2. - CanWrite 还能写入len字节的数据吗?
    *       3. - OffsetWrLenOnWrChange 已写入位置，写入位置_writePos 偏移len字节长度
    *       4. - OffsetOnReadChange 已读取位置，读取位置 _readPos 偏移len字节长度
    *       5. - SetWritePos 设置写入位置_writePos
    */
public:
    char *GetData();
    Int32 GetWrLength() const;
    bool CanRead(Int32 len) const;
    bool CanWrite(Int32 len) const;
    void OffsetWrLenOnWrChange(Int32 len);
    void OffsetOnReadChange(Int32 len);
    void SetWritePos(Int32 n);
    #pragma endregion

    /* 读字节流 */
    #pragma region Read
public:
    template<typename T>
    bool Read(T &n, bool isOffset = true);
    template<typename T>
    UInt32 ReadArray(T *arr, UInt32 len);
    template<typename T>
    bool ReadWithoutOffsetPos(T &n);

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
    template<typename T>
    bool Write(const T &n);
    template<typename T>
    bool WriteArray(T *data, UInt32 len);
    bool WriteInt8(Byte8 n);
    bool WriteInt16(Int16 n);
    bool WriteInt32(Int32 n);
    bool WriteFloat(Float n);
    bool WriteDouble(Double n);
    #pragma endregion

private:
    // 数据缓冲区
    char *_buff = NULL;
    // 缓冲区总的空间大小，字节长度
    Int32 _size = 0;
    // 已写入数据的尾部位置，已写入数据长度
    Int32 _writePos = 0;
    // 已读取数据的尾部位置
    Int32 _readPos = 0;
    // _buff是外部传入的数据块时是否应该被释放
    bool _needDelete = true;
    bool _isPoolCreate = false;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_StreamImpl.h"

#endif
