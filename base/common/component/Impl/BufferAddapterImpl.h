#ifdef __Base_Common_Component_Impl_Buffer_Addapter__H__
/**
* @file    BufferAddapterImpl.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/03/01
* @brief
*/
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
