#ifdef __Base_Common_Net_Impl_FS_Addr_H__

#pragma once
FS_NAMESPACE_BEGIN

inline FS_Addr::~FS_Addr()
{
}

inline const FS_String &FS_Addr::GetAddr() const
{
    return _strIp;
}

inline UInt16 FS_Addr::GetPort() const
{
    return _port;
}

FS_NAMESPACE_END

#endif
