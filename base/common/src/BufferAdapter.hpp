#ifndef BUFFER_ADAPTER_HPP_
#define BUFFER_ADAPTER_HPP_

#pragma once

#pragma  MEM_ALIGN_BEGIN(1)

NAMESPACE_BEGIN(gtool)

//导出实例化 减小其他模块实例化开销
// extern template struct BufferAdapter<float>;
// extern template struct BufferAdapter<double>;
// extern template struct BufferAdapter<void *>;

/////////////////////////////////////////////////////实例化类型

//根据数据类型buffer
template<typename T>
struct BufferAdapter
{

};

template<>
struct BufferAdapter<float>
{
	BUFFER64 Buffer;
	BufferAdapter() { memset(Buffer, 0, BUFFER_LEN64); }
};

template<>
struct BufferAdapter<double>
{
	BUFFER64 Buffer;
	BufferAdapter() { memset(Buffer, 0, BUFFER_LEN64); }
};

template<>
struct BufferAdapter<void *>
{
	BUFFER32 Buffer;
	BufferAdapter() { memset(Buffer, 0, BUFFER_LEN32); }
};


//定长缓冲buffer
template<BUFFER_LEN_TYPE BUFFER_TYPE= BUFFER_LEN_TYPE_1024>
struct BufferAdapterCustomLen
{

};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_32>
{
	BUFFER32 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN32); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_64>
{
	BUFFER64 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN64); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_128>
{
	BUFFER128 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN128); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_256>
{
	BUFFER256 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN256); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_512>
{
	BUFFER512 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN512); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_1024>
{
	BUFFER1024 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN1024); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_2048>
{
	BUFFER2048 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN2048); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_4096>
{
	BUFFER4096 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN4096); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_8192>
{
	BUFFER8192 Buffer;
	BufferAdapterCustomLen() { memset(Buffer, 0, BUFFER_LEN8192); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_9216>
{
	BUFFER9126 Buffer;
	BufferAdapterCustomLen() { ZERO_CLEAN_OBJ(Buffer); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_10240>
{
	BUFFER10240 Buffer;
	BufferAdapterCustomLen() { ZERO_CLEAN_OBJ(Buffer); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_64K>
{
	BUFFER64K Buffer;
	BufferAdapterCustomLen() { ZERO_CLEAN_OBJ(Buffer); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_128K>
{
	BUFFER128K Buffer;
	BufferAdapterCustomLen() { ZERO_CLEAN_OBJ(Buffer); }
};


template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_256K>
{
	BUFFER256K Buffer;
	BufferAdapterCustomLen() { ZERO_CLEAN_OBJ(Buffer); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_512K>
{
	BUFFER512K Buffer;
	BufferAdapterCustomLen() { ZERO_CLEAN_OBJ(Buffer); }
};

template<>
struct BufferAdapterCustomLen<BUFFER_LEN_TYPE_1M>
{
	BUFFER1M Buffer;
	BufferAdapterCustomLen() { ZERO_CLEAN_OBJ(Buffer); }
};


////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct GetBufferAddapterSize
{
	static I64 GetBufferNeeded(const char * const& rData)
	{
		return strlen((char *)rData);
	}

	static I64 GetBufferNeeded(char *& rData)
	{
		return strlen((char *)rData);
	}

	static I64 GetBufferNeeded(char *&& rData)
	{
		return strlen((char *)rData);
	}
};

/////////////////////////////////////////////////////////////////////////////////////// void

template<>
struct GetBufferAddapterSize<void*>
{
	static I64 GetBufferNeeded(const void*& )
	{
		return BUFFER_LEN32;
	}

	static I64 GetBufferNeeded(void* const& )
	{
		return BUFFER_LEN32;
	}

	static I64 GetBufferNeeded(void*& )
	{
		return BUFFER_LEN32;
	}

	static I64 GetBufferNeeded(void*&& )
	{
		return BUFFER_LEN32;
	}
};

template<>
struct GetBufferAddapterSize<const void *>
{
	static I64 GetBufferNeeded(const void *& )
	{
		return BUFFER_LEN32;
	}

	static I64 GetBufferNeeded(void* const& )
	{
		return BUFFER_LEN32;
	}

	static I64 GetBufferNeeded(void *& )
	{
		return BUFFER_LEN32;
	}

	static I64 GetBufferNeeded(void *&& )
	{
		return BUFFER_LEN32;
	}

};

///////////////////////////////////////////////////////////////////////////////////// 8

template<>
struct GetBufferAddapterSize<I8>
{
	static I64 GetBufferNeeded(const I8& )
	{
		return BUFFER_LEN8;
	}
};

template<>
struct GetBufferAddapterSize<const I8>
{
	static I64 GetBufferNeeded(const I8& )
	{
		return BUFFER_LEN8;
	}
};

template<>
struct GetBufferAddapterSize< U8>
{
	static I64 GetBufferNeeded(const U8& )
	{
		return BUFFER_LEN8;
	}
};

template<>
struct GetBufferAddapterSize<const U8>
{
	static I64 GetBufferNeeded(const U8& )
	{
		return BUFFER_LEN8;
	}
};

///////////////////////////////////////////////////////////////////////////////////// 16

template<>
struct GetBufferAddapterSize<U16>
{
	static I64 GetBufferNeeded(const U16& )
	{
		return BUFFER_LEN8;
	}
};

template<>
struct GetBufferAddapterSize<const U16>
{
	static I64 GetBufferNeeded(const U16& )
	{
		return BUFFER_LEN8;
	}
};


template<>
struct GetBufferAddapterSize<I16>
{
	static I64 GetBufferNeeded(const I16& )
	{
		return BUFFER_LEN8;
	}
};

template<>
struct GetBufferAddapterSize<const I16>
{
	static I64 GetBufferNeeded(const I16& )
	{
		return BUFFER_LEN8;
	}
};

///////////////////////////////////////////////////////////////////////////////////// 32

template<>
struct GetBufferAddapterSize<U32>
{
	static I64 GetBufferNeeded(const U32& )
	{
		return BUFFER_LEN32;
	}
};

template<>
struct GetBufferAddapterSize<const U32>
{
	static I64 GetBufferNeeded(const U32& )
	{
		return BUFFER_LEN32;
	}
};


template<>
struct GetBufferAddapterSize<I32>
{
	static I64 GetBufferNeeded(const I32& )
	{
		return BUFFER_LEN32;
	}
};

template<>
struct GetBufferAddapterSize<const I32>
{
	static I64 GetBufferNeeded(const I32& )
	{
		return BUFFER_LEN32;
	}
};

///////////////////////////////////////////////////////////////////////////////////// 64

template<>
struct GetBufferAddapterSize<U64>
{
	static I64 GetBufferNeeded(const U64& )
	{
		return BUFFER_LEN32;
	}
};

template<>
struct GetBufferAddapterSize<const U64>
{
	static I64 GetBufferNeeded(const U64& )
	{
		return BUFFER_LEN32;
	}
};


template<>
struct GetBufferAddapterSize<I64>
{
	static I64 GetBufferNeeded(const I64& )
	{
		return BUFFER_LEN32;
	}
};

template<>
struct GetBufferAddapterSize<const I64>
{
	static I64 GetBufferNeeded(const I64& )
	{
		return BUFFER_LEN32;
	}
};

///////////////////////////////////////////////////////////////////////////////////// LONG

template<>
struct GetBufferAddapterSize<MYLONG>
{
	static I64 GetBufferNeeded(const MYLONG& )
	{
		return BUFFER_LEN32;
	}
};

template<>
struct GetBufferAddapterSize<const MYLONG>
{
	static I64 GetBufferNeeded(const MYLONG& )
	{
		return BUFFER_LEN32;
	}
};


template<>
struct GetBufferAddapterSize<MYULONG>
{
	static I64 GetBufferNeeded(const MYULONG& )
	{
		return BUFFER_LEN32;
	}
};

template<>
struct GetBufferAddapterSize<const MYULONG>
{
	static I64 GetBufferNeeded(const MYULONG& )
	{
		return BUFFER_LEN32;
	}
};


///////////////////////////////////////////////////////////////////////////////////// LONGLONG
// 
// template<>
// struct GetBufferAddapterSize<MYLONGLONG>
// {
// 	static I64 GetBufferNeeded(const MYLONGLONG& rData)
// 	{
// 		return BUFFER_LEN64;
// 	}
// };

// template<>
// struct GetBufferAddapterSize<const MYLONGLONG>
// {
// 	static I64 GetBufferNeeded(const MYLONGLONG& rData)
// 	{
// 		return BUFFER_LEN64;
// 	}
// };


// template<>
// struct GetBufferAddapterSize<MYULONGLONG>
// {
// 	static I64 GetBufferNeeded(const MYULONGLONG& rData)
// 	{
// 		return BUFFER_LEN64;
// 	}
// };
// 
// template<>
// struct GetBufferAddapterSize<const MYULONGLONG>
// {
// 	static I64 GetBufferNeeded(const MYULONGLONG& rData)
// 	{
// 		return BUFFER_LEN64;
// 	}
// };


///////////////////////////////////////////////////////////////////////////////////// float
template<>
struct GetBufferAddapterSize<float>
{
	static I64 GetBufferNeeded(const float& )
	{
		return BUFFER_LEN256;
	}
};

template<>
struct GetBufferAddapterSize<float &>
{
	static I64 GetBufferNeeded(const float && )
	{
		return BUFFER_LEN256;
	}
};

template<>
struct GetBufferAddapterSize<const float>
{
	static I64 GetBufferNeeded(const float& )
	{
		return BUFFER_LEN256;
	}
};

template<>
struct GetBufferAddapterSize<const float&>
{
	static I64 GetBufferNeeded(const float&& )
	{
		return BUFFER_LEN256;
	}
};


///////////////////////////////////////////////////////////////////////////////////// double

template<>
struct GetBufferAddapterSize<double>
{
	static I64 GetBufferNeeded(const double &)
	{
		return BUFFER_LEN512;
	}
};

template<>
struct GetBufferAddapterSize<double &>
{
	static I64 GetBufferNeeded(const double&& )
	{
		return BUFFER_LEN512;
	}
};

template<>
struct GetBufferAddapterSize<const double>
{
	static I64 GetBufferNeeded(const double&)
	{
		return BUFFER_LEN512;
	}
};

template<>
struct GetBufferAddapterSize<const double&>
{
	static I64 GetBufferNeeded(const double&)
	{
		return BUFFER_LEN512;
	}
};

///////////////////////////////////////////////////////////////////////////////////// dumper 字符串buffer

template<DUMPER_BUFFER_TYPE>
struct DumperBuffer
{

};

template<>
struct DumperBuffer<DUMPER_BUFFER_TYPE_SMALLER>
{
	BUFFER512 Buffer;
};

template<>
struct DumperBuffer<DUMPER_BUFFER_TYPE_NORMAL>
{
	BUFFER2048 Buffer;
};

NAMESPACE_END(gtool)

#pragma MEM_ALIGN_END(1)

#endif