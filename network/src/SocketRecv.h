#ifndef SOCKET_RECV_H_
#define SOCKET_RECV_H_

#pragma once

//线程不安全类 CSocketRecv的生命周期内只能绑定一次socket
class CSocketRecv
{
	NO_COPY(CSocketRecv);
	HEAP_CREATE_NEED_CLEANS_PUBLIC(CSocketRecv);
	MEMPOOL_CREATE(m_MemObj);

public:
	bool Recv();	//接收数据
	bool Bind(const SOCKET& sbind, const I64 nCacheSizeSet = 0);	//和socket绑定
	I64 GetCacheSize() const { return m_nCacheSizeMax; }	//获取缓冲区大小
	I64 GetCurDataSize() const { return m_nCurDataSize; }	//获取当前数据大小
	const char *GetCurData() const { return m_pBuffer; }	//获取当前数据
	void ResetCache();

protected:
	gtool::CLitUnmanagedPtr<SOCKET> m_ps = NULL;
	gtool::CSmartPtrC<char, char, DEL_WAY_DELETE_MULTI> m_pBuffer = NULL;
	I64			m_nCurDataSize = 0;		//当前数据大小
	I64			m_nCacheSizeMax = 0;	//最大缓冲大小
	I32			m_nLastErr = 0;			//socket错误
};

#endif

