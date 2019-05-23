#include <PoolObj/xUByte.h>

MEMPOOL_CREATE_IMPLEMENTATION(CUXByte, m_MemObj)

CUXByte::CUXByte()
{
}

CUXByte::CUXByte(const CUXByte& other)
{
	data = other.data;
}

CUXByte::CUXByte(CUXByte&& other)
{
	data = other.data;
	other.data = 0;
}

CUXByte::CUXByte(const unsigned char& other)
{
	data = other;
}

CUXByte::CUXByte(unsigned char&& other)
{
	data = other;
}

CUXByte::CUXByte(unsigned char& other)
{
	data = other;
}

CUXByte::~CUXByte()
{
}

CUXByte::operator unsigned char()
{
	return data;
}

CUXByte::operator const unsigned char()
{
	return data;
}

CUXByte::operator CUXByte()
{
	return *this;
}

CUXByte::operator const CUXByte()
{
	return *this;
}

CUXByte CUXByte::operator%(const CUXByte& other)
{
	CUXByte result;
	result.data = data % other.data;
	return result;
}

CUXByte CUXByte::operator%(CUXByte& other)
{
	CUXByte result;
	result.data = data % other.data;
	return result;
}

CUXByte CUXByte::operator%(const unsigned char& other)
{
	CUXByte result;
	result.data = data % other;
	return result;
}

CUXByte CUXByte::operator%(unsigned char & other)
{
	CUXByte result;
	result.data = data % other;
	return result;
}

CUXByte CUXByte::operator ~()
{
	CUXByte result;
	result.data = ~data;
	return result;
}

CUXByte& CUXByte::operator%=(unsigned char& other)
{
	data %= other;
	return *this;
}

CUXByte& CUXByte::operator%=(const unsigned char& other)
{
	data %= other;
	return *this;
}

CUXByte& CUXByte::operator%=(CUXByte& other)
{
	data %= other.data;
	return *this;
}

CUXByte& CUXByte::operator%=(const CUXByte& other)
{
	data %= other.data;
	return *this;
}

CUXByte& CUXByte::operator|=(unsigned char& other)
{
	data |= other;
	return *this;
}

CUXByte& CUXByte::operator|=(const unsigned char& other)
{
	data |= other;
	return *this;
}

CUXByte& CUXByte::operator|=(CUXByte& other)
{
	data |= other.data;
	return *this;
}

CUXByte& CUXByte::operator|=(const CUXByte& other)
{
	data |= other.data;
	return *this;
}

CUXByte& CUXByte::operator&=(unsigned char& other)
{
	data &= other;
	return *this;
}

CUXByte& CUXByte::operator&=(const unsigned char& other)
{
	data &= other;
	return *this;
}

CUXByte& CUXByte::operator&=(CUXByte& other)
{
	data &= other.data;
	return *this;
}

CUXByte& CUXByte::operator&=(const CUXByte& other)
{
	data &= other.data;
	return *this;
}

CUXByte& CUXByte::operator/=(unsigned char& other)
{
	data /= other;
	return *this;
}

CUXByte& CUXByte::operator/=(CUXByte& other)
{
	data /= other.data;
	return *this;
}

CUXByte& CUXByte::operator/=(const unsigned char& other)
{
	data /= other;
	return *this;
}

CUXByte& CUXByte::operator/=(const CUXByte& other)
{
	data /= other.data;
	return *this;
}

CUXByte CUXByte::operator|(unsigned char& other)
{
	CUXByte result;
	result.data = data | other;
	return result;
}

CUXByte CUXByte::operator|(const unsigned char& other)
{
	CUXByte result;
	result.data = data | other;
	return result;
}

CUXByte CUXByte::operator|(CUXByte& other)
{
	CUXByte result;
	result.data = data | other.data;
	return result;
}

CUXByte CUXByte::operator|(const CUXByte& other)
{
	CUXByte result;
	result.data = data | other.data;
	return result;
}

CUXByte CUXByte::operator&(unsigned char& other)
{
	CUXByte result;
	result.data = data & other;
	return result;
}

CUXByte CUXByte::operator&(const unsigned char& other)
{
	CUXByte result;
	result.data = data & other;
	return result;
}

CUXByte CUXByte::operator&(CUXByte& other)
{
	CUXByte result;
	result.data = data & other.data;
	return result;
}

CUXByte CUXByte::operator&(const CUXByte& other)
{
	CUXByte result;
	result.data = data & other.data;
	return result;
}

CUXByte& CUXByte::operator*=(unsigned char& other)
{
	data *= other;
	return *this;
}

CUXByte& CUXByte::operator*=(const unsigned char& other)
{
	data *= other;
	return *this;
}

CUXByte& CUXByte::operator*=(CUXByte& other)
{
	data *= other.data;
	return *this;
}

CUXByte& CUXByte::operator*=(const CUXByte& other)
{
	data *= other.data;
	return *this;
}

bool CUXByte::operator!=(unsigned char& other)
{
	return data != other;
}

bool CUXByte::operator!=(const unsigned char& other)
{
	return data != other;
}

bool CUXByte::operator!=(CUXByte& other)
{
	return data != other.data;
}

bool CUXByte::operator!=(const CUXByte& other)
{
	return data != other.data;
}

CUXByte& CUXByte::operator-=(unsigned char& other)
{
	data -= other;
	return *this;
}

CUXByte& CUXByte::operator-=(const unsigned char& other)
{
	data -= other;
	return *this;
}

CUXByte& CUXByte::operator-=(CUXByte& other)
{
	data -= other.data;
	return *this;
}

CUXByte& CUXByte::operator-=(const CUXByte& other)
{
	data -= other.data;
	return *this;
}

CUXByte& CUXByte::operator+=(unsigned char& other)
{
	data += other;
	return *this;
}

CUXByte& CUXByte::operator+=(const unsigned char& other)
{
	data += other;
	return *this;
}

CUXByte& CUXByte::operator+=(CUXByte& other)
{
	data += other.data;
	return *this;
}

CUXByte& CUXByte::operator+=(const CUXByte& other)
{
	data += other.data;
	return *this;
}

CUXByte CUXByte::operator--(int)
{
	CUXByte add;
	add.data = data;
	--data;
	return add;
}

CUXByte& CUXByte::operator--()
{
	--data;
	return *this;
}

CUXByte CUXByte::operator++(int)
{
	CUXByte add;
	add.data = data;
	++data;
	return add;
}

CUXByte& CUXByte::operator++()
{
	++data;
	return *this;
}

CUXByte CUXByte::operator/(unsigned char& other)
{
	CUXByte add;
	add.data = this->data / other;
	return add;
}

CUXByte CUXByte::operator/(const unsigned char& other)
{
	CUXByte add;
	add.data = this->data / other;
	return add;
}

CUXByte CUXByte::operator/(CUXByte& other)
{
	CUXByte add;
	add.data = this->data / other.data;
	return add;
}

CUXByte CUXByte::operator/(const CUXByte& other)
{
	CUXByte add;
	add.data = this->data / other.data;
	return add;
}

CUXByte CUXByte::operator*(unsigned char& other)
{
	CUXByte add;
	add.data = this->data * other;
	return add;
}

CUXByte CUXByte::operator*(const unsigned char& other)
{
	CUXByte add;
	add.data = this->data * other;
	return add;
}

CUXByte CUXByte::operator*(CUXByte& other)
{
	CUXByte add;
	add.data = this->data * other.data;
	return add;
}

CUXByte CUXByte::operator*(const CUXByte& other)
{
	CUXByte add;
	add.data = this->data * other.data;
	return add;
}

CUXByte CUXByte::operator-(unsigned char& other)
{
	CUXByte add;
	add.data = this->data - other;
	return add;
}

CUXByte CUXByte::operator-(const unsigned char& other)
{
	CUXByte add;
	add.data = this->data - other;
	return add;
}

CUXByte CUXByte::operator-(CUXByte& other)
{
	CUXByte add;
	add.data = this->data - other.data;
	return add;
}

CUXByte CUXByte::operator-(const CUXByte& other)
{
	CUXByte add;
	add.data = this->data - other.data;
	return add;
}

CUXByte CUXByte::operator+(unsigned char& other)
{
	CUXByte add;
	add.data = this->data + other;
	return add;
}

CUXByte CUXByte::operator+(const unsigned char& other)
{
	CUXByte add;
	add.data = this->data + other;
	return add;
}

CUXByte CUXByte::operator+(CUXByte& other)
{
	CUXByte add;
	add.data = this->data + other.data;
	return add;
}

CUXByte CUXByte::operator+(const CUXByte& other)
{
	CUXByte add;
	add.data = this->data + other.data;
	return add;
}

CUXByte& CUXByte::operator=(unsigned char& other)
{
	data = other;
	return *this;
}

CUXByte& CUXByte::operator=(const unsigned char& other)
{
	data = other;
	return *this;
}

CUXByte& CUXByte::operator=(CUXByte& other)
{
	data = other.data;
	return *this;
}

CUXByte& CUXByte::operator=(const CUXByte& other)
{
	data = other.data;
	return *this;
}

bool CUXByte::operator==(unsigned char& other)
{
	return data == other;
}

bool CUXByte::operator==(const unsigned char& other)
{
	return data == other;
}

bool CUXByte::operator==(const CUXByte& other)
{
	return data == other.data;
}

bool CUXByte::operator ==(CUXByte& other)
{
	return data == other.data;
}