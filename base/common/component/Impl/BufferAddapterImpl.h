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
 * @file  : BufferAddapterImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_Buffer_Addapter_H__

#pragma once

FS_NAMESPACE_BEGIN

template<>
struct GetBufferAddapterSize<void *>
{
    static Int16 GetBufferNeeded(const void * &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void *const &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &&)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<const void *>
{
    static Int16 GetBufferNeeded(const void * &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * const &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &&)
    {
        return BUFFER_LEN32;
    }
};


template<>
struct GetBufferAddapterSize<void *&>
{
    static Int16 GetBufferNeeded(const void * &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void *const &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &&)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<const void *&>
{
    static Int16 GetBufferNeeded(const void *&)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void *const &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &)
    {
        return BUFFER_LEN32;
    }

    static Int16 GetBufferNeeded(void * &&)
    {
        return BUFFER_LEN32;
    }
};
///////////////////////////////////////////////////////////////////////////////////// 8

template<>
struct GetBufferAddapterSize<Byte8>
{
    static Int16 GetBufferNeeded(const Byte8 &)
    {
        return BUFFER_LEN4;
    }
};

template<>
struct GetBufferAddapterSize<const Byte8>
{
    static Int16 GetBufferNeeded(const Byte8 &)
    {
        return BUFFER_LEN4;
    }
};

template<>
struct GetBufferAddapterSize<Byte8 &>
{
    static Int16 GetBufferNeeded(const Byte8 &)
    {
        return BUFFER_LEN4;
    }
};

template<>
struct GetBufferAddapterSize<const Byte8 &>
{
    static Int16 GetBufferNeeded(const Byte8 &)
    {
        return BUFFER_LEN4;
    }
};

template<>
struct GetBufferAddapterSize<U8>
{
    static Int16 GetBufferNeeded(const U8 &)
    {
        return BUFFER_LEN4;
    }
};

template<>
struct GetBufferAddapterSize<const U8>
{
    static Int16 GetBufferNeeded(const U8 &)
    {
        return BUFFER_LEN4;
    }
};

template<>
struct GetBufferAddapterSize<U8 &>
{
    static Int16 GetBufferNeeded(const U8 &)
    {
        return BUFFER_LEN4;
    }
};

template<>
struct GetBufferAddapterSize<const U8 &>
{
    static Int16 GetBufferNeeded(const U8 &)
    {
        return BUFFER_LEN4;
    }
};

///////////////////////////////////////////////////////////////////////////////////// 16

template<>
struct GetBufferAddapterSize<UInt16>
{
    static Int16 GetBufferNeeded(const UInt16 &)
    {
        return BUFFER_LEN8;
    }
};

template<>
struct GetBufferAddapterSize<const UInt16>
{
    static Int16 GetBufferNeeded(const UInt16 &)
    {
        return BUFFER_LEN8;
    }
};

template<>
struct GetBufferAddapterSize<UInt16 &>
{
    static Int16 GetBufferNeeded(const UInt16 &)
    {
        return BUFFER_LEN8;
    }
};

template<>
struct GetBufferAddapterSize<const UInt16 &>
{
    static Int16 GetBufferNeeded(const UInt16 &)
    {
        return BUFFER_LEN8;
    }
};

template<>
struct GetBufferAddapterSize<Int16>
{
    static Int16 GetBufferNeeded(const Int16 &)
    {
        return BUFFER_LEN8;
    }
};

template<>
struct GetBufferAddapterSize<const Int16>
{
    static Int16 GetBufferNeeded(const Int16 &)
    {
        return BUFFER_LEN8;
    }
};

template<>
struct GetBufferAddapterSize<Int16 &>
{
    static Int16 GetBufferNeeded(const Int16 &)
    {
        return BUFFER_LEN8;
    }
};

template<>
struct GetBufferAddapterSize<const Int16 &>
{
    static Int16 GetBufferNeeded(const Int16 &)
    {
        return BUFFER_LEN8;
    }
};

///////////////////////////////////////////////////////////////////////////////////// 32

template<>
struct GetBufferAddapterSize<UInt32>
{
    static Int16 GetBufferNeeded(const UInt32 &)
    {
        return BUFFER_LEN16;
    }
};

template<>
struct GetBufferAddapterSize<const UInt32>
{
    static Int16 GetBufferNeeded(const UInt32 &)
    {
        return BUFFER_LEN16;
    }
};

template<>
struct GetBufferAddapterSize<UInt32 &>
{
    static Int16 GetBufferNeeded(const UInt32 &)
    {
        return BUFFER_LEN16;
    }
};

template<>
struct GetBufferAddapterSize<const UInt32 &>
{
    static Int16 GetBufferNeeded(const UInt32 &)
    {
        return BUFFER_LEN16;
    }
};

template<>
struct GetBufferAddapterSize<Int32>
{
    static Int16 GetBufferNeeded(const Int32 &)
    {
        return BUFFER_LEN16;
    }
};

template<>
struct GetBufferAddapterSize<const Int32>
{
    static Int16 GetBufferNeeded(const Int32&)
    {
        return BUFFER_LEN16;
    }
};

template<>
struct GetBufferAddapterSize<Int32 &>
{
    static Int16 GetBufferNeeded(const Int32 &)
    {
        return BUFFER_LEN16;
    }
};

template<>
struct GetBufferAddapterSize<const Int32 &>
{
    static Int16 GetBufferNeeded(const Int32 &)
    {
        return BUFFER_LEN16;
    }
};

///////////////////////////////////////////////////////////////////////////////////// 64

template<>
struct GetBufferAddapterSize<UInt64>
{
    static Int16 GetBufferNeeded(const UInt64 &)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<const UInt64>
{
    static Int16 GetBufferNeeded(const UInt64 &)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<UInt64 &>
{
    static Int16 GetBufferNeeded(const UInt64 &)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<const UInt64 &>
{
    static Int16 GetBufferNeeded(const UInt64 &)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<Int64>
{
    static Int16 GetBufferNeeded(const Int64 &)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<const Int64>
{
    static Int16 GetBufferNeeded(const Int64 &)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<Int64 &>
{
    static Int16 GetBufferNeeded(const Int64 &)
    {
        return BUFFER_LEN32;
    }
};

template<>
struct GetBufferAddapterSize<const Int64 &>
{
    static Int16 GetBufferNeeded(const Int64 &)
    {
        return BUFFER_LEN32;
    }
};

///////////////////////////////////////////////////////////////////////////////////// float
template<>
struct GetBufferAddapterSize<float>
{
    static Int16 GetBufferNeeded(const float &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE / 2 + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};

template<>
struct GetBufferAddapterSize<float &>
{
    static Int16 GetBufferNeeded(const float &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE / 2 + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};

template<>
struct GetBufferAddapterSize<const float>
{
    static Int16 GetBufferNeeded(const float &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE/2 + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};

template<>
struct GetBufferAddapterSize<const float &>
{
    static Int16 GetBufferNeeded(const float &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE / 2 + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};


///////////////////////////////////////////////////////////////////////////////////// double

template<>
struct GetBufferAddapterSize<double>
{
    static Int16 GetBufferNeeded(const double &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};

template<>
struct GetBufferAddapterSize<double &>
{
    static Int16 GetBufferNeeded(const double &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};

template<>
struct GetBufferAddapterSize<const double>
{
    static Int16 GetBufferNeeded(const double &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};

template<>
struct GetBufferAddapterSize<const double&>
{
    static Int16 GetBufferNeeded(const double &val)
    {
        double ceil = 0, intPart = 0;
        ceil = modf(val, &intPart);

        Int64 integer = static_cast<Int64>(intPart);
        Int16 cnt = (integer > 0 ? 0 : 1) + MAX_CEIL_WIDE + 1;
        integer = abs(integer);
        if(integer == 0)
            return ++cnt;

        while(true)
        {
            integer /= 10;
            ++cnt;

            if(integer <= 0)
                break;
        }

        return cnt;
    }
};

FS_NAMESPACE_END

#endif
