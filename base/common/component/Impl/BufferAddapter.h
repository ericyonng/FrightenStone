#ifndef __Base_Common_Component_Impl_Buffer_Addapter__H__
#define __Base_Common_Component_Impl_Buffer_Addapter__H__
/**
* @file    BufferAddapter.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/03/01
* @brief
*/

#pragma once
#include "base/exportbase.h"
#include "base/common/basedefs/DataType/DataType.h"

FS_NAMESPACE_BEGIN

template<typename T>
struct GetBufferAddapterSize
{
    static Int16 GetBufferNeeded(const char *&val)
    {
        return strlen(const_cast<char *>(val));
    }

    static Int64 GetBufferNeeded(char *& data)
    {
        return strlen(reinterpret_cast<char *>(data));
    }

    static Int16 GetBufferNeeded(char *&&data)
    {
        return strlen(reinterpret_cast<char *>(data));
    }

    static Int16 GetBufferNeeded(char * const &&data)
    {
        return strlen(reinterpret_cast<char *>(data));
    }
};

FS_NAMESPACE_END

#include "base/common/component/Impl/BufferAddapterImpl.h"

#endif
