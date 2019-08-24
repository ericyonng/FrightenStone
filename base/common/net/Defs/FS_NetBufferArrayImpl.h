#ifdef __Base_Common_Net_Defs_FS_NetBufferArray_H__
#pragma once

FS_NAMESPACE_BEGIN



inline FS_NetBufferArray::FS_NetBufferArray(Int32 sizeBuffer)
    :_bufferSize(sizeBuffer)
{
    auto newBuffer = new FS_NetBuffer(this, _bufferSize);
    _buffers.push_back(newBuffer);
    newBuffer->SetNode(--_buffers.end());
}

inline FS_NetBufferArray::~FS_NetBufferArray()
{
    STLUtil::DelListContainer<decltype(_buffers), AssistObjsDefs::SelfRelease>(_buffers);
}

inline void FS_NetBufferArray::Release()
{
    delete this;
}

inline char *FS_NetBufferArray::GetData()
{
    return _buffers.front()->GetData();
}

inline std::list<FS_NetBuffer *>::iterator FS_NetBufferArray::GetFrontNode()
{
    return _buffers.begin();
}

inline bool FS_NetBufferArray::Push(const char *data, Int32 len)
{
    if(!_buffers.back()->Push(data, len))
    {
        auto newBuffer = new FS_NetBuffer(this, _bufferSize);
        _buffers.push_back(newBuffer);
        newBuffer->SetNode(--_buffers.end());
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
        _buffers.erase(iterNode);
        iterNode = _buffers.end();
        FS_Release(buffer);
    }
}

inline Int32 FS_NetBufferArray::Write2socket(SOCKET sockfd)
{
    return _buffers.front()->Write2socket(sockfd);
}

inline Int32 FS_NetBufferArray::ReadFromSocket(SOCKET sockfd)
{
    return _buffers.back()->ReadFromSocket(sockfd);
}

inline bool FS_NetBufferArray::HasMsg() const
{
    return _buffers.front()->HasMsg();
}

inline bool FS_NetBufferArray::NeedWrite() const
{
    return _buffers.front()->NeedWrite();
}

inline IO_DATA_BASE *FS_NetBufferArray::MakeRecvIoData(SOCKET sockfd)
{
    return _buffers.back()->MakeRecvIoData(sockfd);
}

inline IO_DATA_BASE *FS_NetBufferArray::MakeSendIoData(SOCKET sockfd)
{// 从最早的数据发送
    return _buffers.front()->MakeSendIoData(sockfd);
}

inline bool FS_NetBufferArray::OnReadFromIocp(std::list<FS_NetBuffer *>::iterator &iterNode, int recvBytes)
{
    return (*iterNode)->OnReadFromIocp(recvBytes);
}

inline bool FS_NetBufferArray::OnWrite2Iocp(std::list<FS_NetBuffer *>::iterator &iterNode, int sendBytes)
{
    auto buffer = *iterNode;
    bool isWrite2Iocp = buffer->OnWrite2Iocp(sendBytes);
    if(!isWrite2Iocp)
        return false;
    if(_buffers.size() == 1)
        return isWrite2Iocp;

    if(buffer->IsEmpty())
    {
        _buffers.erase(iterNode);
        iterNode = _buffers.end();
        FS_Release(buffer);
    }

    return isWrite2Iocp;
}

FS_NAMESPACE_END

#endif
