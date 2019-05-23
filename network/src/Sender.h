#ifndef SENDER_H_
#define SENDER_H_

#pragma once

#include <TDPoster/ThreadPostInterfaceClass.hpp>

class CSenderThread :public INetworkSenderThread
{
	MEMPOOL_CREATE(m_MemObj);
public:
	CSenderThread(boost::atomic_bool& bThreadWork) :m_bThreadWork(bThreadWork) {}
	virtual ~CSenderThread() {}

	//任务执行体
	virtual int Run();

	//释放
	virtual int Release();

	virtual bool HandleRecvMsg(const U64 u64ConnectionPtr, const void *pData, const I64 nSize);	 //pData 数据不包含尺寸 nSize数据尺寸

protected:
	boost::atomic_bool&		m_bThreadWork;		//与管理器m_bThreadWork关联
};


#endif

