/**
* @file    FS_File.cpp
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/20
* @brief
*/

#include "stdafx.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/component/Impl/File/FS_File.h"
#include "base/common/assist/utils/utils.h"
#include "base/common/status/status.h"
#include <assert.h>

FS_NAMESPACE_BEGIN

FS_File::FS_File()
    :_fp(NULL)
    ,_useTimestampTailer(false)
    ,_openMode("ab+")
{
}

FS_File::~FS_File()
{
    _locker.Lock();
    if(_fp)
    {
        FS_FileUtil::FlushFile(*_fp);
        FS_FileUtil::CloseFile(*_fp);
    }
    _locker.Unlock();
}

bool FS_File::Open(const char *fileName, bool isCreate /*= false*/, const char *openMode /*= "ab+"*/, bool useTimestampTailer /*= false*/)
{
    if(UNLIKELY(_fp))
    {
        printf("always open");
        assert(!"always open");
        return false;
    }

    FS_String fileNameCache = fileName;
    _createFileTime = Time::Now();
    _modifyFileTime = _createFileTime;
    _useTimestampTailer = useTimestampTailer;
    _extensionName = FS_FileUtil::ExtractFileExtension(fileNameCache);
    if(isCreate && _useTimestampTailer)
        FS_FileUtil::InsertFileTime(_extensionName, _createFileTime, fileNameCache);

    _fp = FS_FileUtil::OpenFile(fileNameCache.c_str(), isCreate, openMode);
    if(!_fp)
    {
        // ´´½¨Ä¿Â¼
        const auto &dir = FS_DirectoryUtil::GetFileDirInPath(fileName);
        if(FS_DirectoryUtil::CreateDir(dir))
            _fp = FS_FileUtil::OpenFile(fileNameCache.c_str(), isCreate, openMode);
    }

    if(_fp)
    {
        _fileName = FS_DirectoryUtil::GetFileNameInPath(fileNameCache);
        _path = FS_DirectoryUtil::GetFileDirInPath(fileNameCache);
        _openMode = openMode;
    }

    return _fp != NULL;
}

bool FS_File::Reopen()
{
    if(UNLIKELY(!Close()))
    {
        printf("close file fail!");
        return false;
    }

    FS_String wholeName = _path + _fileName;
    return Open(wholeName.c_str(), true, _openMode.c_str(), _useTimestampTailer);
}

bool FS_File::Flush()
{
    if(UNLIKELY(!_fp))
    {
        printf("file not open");
        return false;
    }

    return FS_FileUtil::FlushFile(*_fp);
}

bool FS_File::Close()
{
    if(_fp)
    {
        FS_FileUtil::FlushFile(*_fp);
        FS_FileUtil::CloseFile(*_fp);
    }

    _fp = NULL;
    return true;
}

Int64 FS_File::Write(const void *buffer, UInt64 writeDataLen)
{
    if(UNLIKELY(!_fp))
        return StatusDefs::Error;

    auto wrLen = FS_FileUtil::WriteFile(*_fp, reinterpret_cast<const char *>(buffer), writeDataLen);
    if(wrLen != 0)
        _modifyFileTime.FlushTime();

    return static_cast<Int64>(wrLen);
}

Int64 FS_File::Read(FS_String &outBuffer)
{
    if(UNLIKELY(!_fp))
        return StatusDefs::Error;

    return FS_FileUtil::ReadFile(*_fp, outBuffer);
}

Int64 FS_File::Read(Int64 sizeLimit, FS_String &outBuffer)
{
    if(UNLIKELY(!_fp))
        return StatusDefs::Error;

    return FS_FileUtil::ReadFile(*_fp, outBuffer, sizeLimit);
}

Int64 FS_File::ReadOneLine(FS_String &outBuffer)
{
    if(UNLIKELY(!_fp))
        return StatusDefs::Error;

    return static_cast<Int64>(FS_FileUtil::ReadOneLine(*_fp, outBuffer));
}

FS_NAMESPACE_END
