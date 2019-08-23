#ifdef __Base_Common_Net_Defs_FS_NetBufferArray_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_NetBufferArray::FS_NetBufferArray(Int32 sizeBuffer)
    :_bufferSize(sizeBuffer)
{
    _buffers.push_back(new FS_NetBuffer(_bufferSize));
}

inline FS_NetBufferArray::~FS_NetBufferArray()
{
    STLUtil::DelListContainer<decltype(_buffers), AssistObjsDefs::SelfRelease>(_buffers);
}

inline char *FS_NetBufferArray::GetData()
{
    return _buffers.front()->GetData();
}

inline bool FS_NetBufferArray::Push(const char *data, Int32 len)
{
    if(!_buffers.back()->Push(data, len))
    {
        auto newBuffer = new FS_NetBuffer(_bufferSize);
        _buffers.push_back(newBuffer);
        return newBuffer->Push(data, len);
    }

    return true;
}

inline void FS_NetBufferArray::Pop(std::list<FS_NetBuffer *>::iterator &iterNode, Int32 len)
{
    auto buffer = *iterNode;
    buffer->Pop(len);
    if(_buffers.size() == 1)
        return;

    if(buffer->IsEmpty())
    {
        FS_Release(buffer);
        _buffers.erase(iterNode);
        iterNode = _buffers.end();
    }
}

inline Int32 FS_NetBufferArray::Write2socket(SOCKET sockfd)
{
    return _buffers.back()->Write2socket(sockfd);
}

inline Int32 FS_NetBufferArray::ReadFromSocket(SOCKET sockfd)
{
    return _buffers
}

FS_NAMESPACE_END

#endif
