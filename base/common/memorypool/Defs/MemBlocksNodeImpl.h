#ifdef __Base_Common_MemoryPool_Defs_MemBlocksNode_H__
#pragma once

FS_NAMESPACE_BEGIN

inline MemBlocksNode::MemBlocksNode(size_t nodeSize)
    :_memBuff(::malloc(nodeSize))
    ,_nodeSize(nodeSize)
    ,_next(NULL)
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
