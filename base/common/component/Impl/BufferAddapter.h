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
 * @file  : BufferAddapter.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_Buffer_Addapter_H__
#define __Base_Common_Component_Impl_Buffer_Addapter_H__


#pragma once
#include "base/exportbase.h"
#include "base/common/basedefs/DataType/DataType.h"

FS_NAMESPACE_BEGIN

template<typename ObjType>
struct GetBufferAddapterSize
{
    static Int16 GetBufferNeeded(const char *&val)
    {
        return strlen(val);
    }

    static Int64 GetBufferNeeded(char *& data)
    {
        return strlen(reinterpret_cast<const char *>(data));
    }

    static Int16 GetBufferNeeded(char *&&data)
    {
        return strlen(reinterpret_cast<const char *>(data));
    }

    static Int16 GetBufferNeeded(char * const &&data)
    {
        return strlen(reinterpret_cast<const char *>(data));
    }
};

FS_NAMESPACE_END

#include "base/common/component/Impl/BufferAddapterImpl.h"

#endif
