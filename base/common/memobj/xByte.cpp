#include <PoolObj/xByte.h>

MEMPOOL_CREATE_IMPLEMENTATION(CXByte, m_MemObj)

CXByte::CXByte()
{
}

CXByte::CXByte(const CXByte& other)
{
	data = other.data;
}

CXByte::CXByte(CXByte&& other)
{
	data = other.data;
	other.data = 0;
}

CXByte::CXByte(const char& other)
{
	data = other;
}

CXByte::CXByte(char&& other)
{
	data = other;
}

CXByte::CXByte(char& other)
{
	data = other;
}

CXByte::~CXByte()
{
}

CXByte::operator char()
{
	return data;
}

CXByte::operator const char()
{
	return data;
}

CXByte::operator CXByte()
{
	return *this;
}

CXByte::operator const CXByte()
{
	return *this;
}

CXByte CXByte::operator%(const CXByte& other)
{
	CXByte result;
	result.data = data % other.data;
	return result;
}

CXByte CXByte::operator%(CXByte& other)
{
	CXByte result;
	result.data = data % other.data;
	return result;
}

CXByte CXByte::operator%(const char& other)
{
	CXByte result;
	result.data = data % other;
	return result;
}

CXByte CXByte::operator%(char& other)
{
	CXByte result;
	result.data = data % other;
	return result;
}

CXByte CXByte::operator ~()
{
	CXByte result;
	result.data = ~data;
	return result;
}

CXByte& CXByte::operator%=(char& other)
{
	data %= other;
	return *this;
}

CXByte& CXByte::operator%=(const char& other)
{
	data %= other;
	return *this;
}

CXByte& CXByte::operator%=(CXByte& other)
{
	data %= other.data;
	return *this;
}

CXByte& CXByte::operator%=(const CXByte& other)
{
	data %= other.data;
	return *this;
}

CXByte& CXByte::operator|=(char& other)
{
	data |= other;
	return *this;
}

CXByte& CXByte::operator|=(const char& other)
{
	data |= other;
	return *this;
}

CXByte& CXByte::operator|=(CXByte& other)
{
	data |= other.data;
	return *this;
}

CXByte& CXByte::operator|=(const CXByte& other)
{
	data |= other.data;
	return *this;
}

CXByte& CXByte::operator&=(char& other)
{
	data &= other;
	return *this;
}

CXByte& CXByte::operator&=(const char& other)
{
	data &= other;
	return *this;
}

CXByte& CXByte::operator&=(CXByte& other)
{
	data &= other.data;
	return *this;
}

CXByte& CXByte::operator&=(const CXByte& other)
{
	data &= other.data;
	return *this;
}

CXByte& CXByte::operator/=(char& other)
{
	data /= other;
	return *this;
}

CXByte& CXByte::operator/=(CXByte& other)
{
	data /= other.data;
	return *this;
}

CXByte& CXByte::operator/=(const char& other)
{
	data /= other;
	return *this;
}

CXByte& CXByte::operator/=(const CXByte& other)
{
	data /= other.data;
	return *this;
}

CXByte CXByte::operator|(char& other)
{
	CXByte result;
	result.data = data | other;
	return result;
}

CXByte CXByte::operator|(const char& other)
{
	CXByte result;
	result.data = data | other;
	return result;
}

CXByte CXByte::operator|(CXByte& other)
{
	CXByte result;
	result.data = data | other.data;
	return result;
}

CXByte CXByte::operator|(const CXByte& other)
{
	CXByte result;
	result.data = data | other.data;
	return result;
}

CXByte CXByte::operator&(char& other)
{
	CXByte result;
	result.data = data & other;
	return result;
}

CXByte CXByte::operator&(const char& other)
{
	CXByte result;
	result.data = data & other;
	return result;
}

CXByte CXByte::operator&(CXByte& other)
{
	CXByte result;
	result.data = data & other.data;
	return result;
}

CXByte CXByte::operator&(const CXByte& other)
{
	CXByte result;
	result.data = data & other.data;
	return result;
}

CXByte& CXByte::operator*=(char& other)
{
	data *= other;
	return *this;
}

CXByte& CXByte::operator*=(const char& other)
{
	data *= other;
	return *this;
}

CXByte& CXByte::operator*=(CXByte& other)
{
	data *= other.data;
	return *this;
}

CXByte& CXByte::operator*=(const CXByte& other)
{
	data *= other.data;
	return *this;
}

bool CXByte::operator!=(char& other)
{
	return data != other;
}

bool CXByte::operator!=(const char& other)
{
	return data != other;
}

bool CXByte::operator!=(CXByte& other)
{
	return data != other.data;
}

bool CXByte::operator!=(const CXByte& other)
{
	return data != other.data;
}

CXByte& CXByte::operator-=(char& other)
{
	data -= other;
	return *this;
}

CXByte& CXByte::operator-=(const char& other)
{
	data -= other;
	return *this;
}

CXByte& CXByte::operator-=(CXByte& other)
{
	data -= other.data;
	return *this;
}

CXByte& CXByte::operator-=(const CXByte& other)
{
	data -= other.data;
	return *this;
}

CXByte& CXByte::operator+=(char& other)
{
	data += other;
	return *this;
}

CXByte& CXByte::operator+=(const char& other)
{
	data += other;
	return *this;
}

CXByte& CXByte::operator+=(CXByte& other)
{
	data += other.data;
	return *this;
}

CXByte& CXByte::operator+=(const CXByte& other)
{
	data += other.data;
	return *this;
}

CXByte CXByte::operator--(int)
{
	CXByte add;
	add.data = data;
	--data;
	return add;
}

CXByte& CXByte::operator--()
{
	--data;
	return *this;
}

CXByte CXByte::operator++(int)
{
	CXByte add;
	add.data = data;
	++data;
	return add;
}

CXByte& CXByte::operator++()
{
	++data;
	return *this;
}

CXByte CXByte::operator/(char& other)
{
	CXByte add;
	add.data = this->data / other;
	return add;
}

CXByte CXByte::operator/(const char& other)
{
	CXByte add;
	add.data = this->data / other;
	return add;
}

CXByte CXByte::operator/(CXByte& other)
{
	CXByte add;
	add.data = this->data / other.data;
	return add;
}

CXByte CXByte::operator/(const CXByte& other)
{
	CXByte add;
	add.data = this->data / other.data;
	return add;
}

CXByte CXByte::operator*(char& other)
{
	CXByte add;
	add.data = this->data * other;
	return add;
}

CXByte CXByte::operator*(const char& other)
{
	CXByte add;
	add.data = this->data * other;
	return add;
}

CXByte CXByte::operator*(CXByte& other)
{
	CXByte add;
	add.data = this->data * other.data;
	return add;
}

CXByte CXByte::operator*(const CXByte& other)
{
	CXByte add;
	add.data = this->data * other.data;
	return add;
}

CXByte CXByte::operator-(char& other)
{
	CXByte add;
	add.data = this->data - other;
	return add;
}

CXByte CXByte::operator-(const char& other)
{
	CXByte add;
	add.data = this->data - other;
	return add;
}

CXByte CXByte::operator-(CXByte& other)
{
	CXByte add;
	add.data = this->data - other.data;
	return add;
}

CXByte CXByte::operator-(const CXByte& other)
{
	CXByte add;
	add.data = this->data - other.data;
	return add;
}

CXByte CXByte::operator+(char& other)
{
	CXByte add;
	add.data = this->data + other;
	return add;
}

CXByte CXByte::operator+(const char& other)
{
	CXByte add;
	add.data = this->data + other;
	return add;
}

CXByte CXByte::operator+(CXByte& other)
{
	CXByte add;
	add.data = this->data + other.data;
	return add;
}

CXByte CXByte::operator+(const CXByte& other)
{
	CXByte add;
	add.data = this->data + other.data;
	return add;
}

CXByte& CXByte::operator=(char& other)
{
	data = other;
	return *this;
}

CXByte& CXByte::operator=(const char& other)
{
	data = other;
	return *this;
}

CXByte& CXByte::operator=(CXByte& other)
{
	data = other.data;
	return *this;
}

CXByte& CXByte::operator=(const CXByte& other)
{
	data = other.data;
	return *this;
}

bool CXByte::operator==(char& other)
{
	return data == other;
}

bool CXByte::operator==(const char& other)
{
	return data == other;
}

bool CXByte::operator==(const CXByte& other)
{
	return data == other.data;
}

bool CXByte::operator ==(CXByte& other)
{
	return data == other.data;
}