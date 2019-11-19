#ifdef __Base_Common_MemoryPool_Defs_MemBlocksNode_H__
#pragma once

FS_NAMESPACE_BEGIN

inline MemBlocksNode::MemBlocksNode(size_t blockSize, size_t blockCnt)
    :_memBuff(::malloc(blockSize*blockCnt))
    ,_nodeSize(blockSize*blockCnt)
    ,_next(NULL)
    ,_blockCnt(blockCnt)
{
}

inline MemBlocksNode::~MemBlocksNode()
{
    if(_memBuff)
    {
        ::free(_memBuff);
        _memBuff = NULL;
    }
}

FS_NAMESPACE_END

#endif
