#ifndef RECIEVER_H_
#define RECIEVER_H_

#pragma once

class CTaskBase;

class CReciever :public CTaskBase
{
	MEMPOOL_CREATE(m_MemObj);
public:
	CReciever() {}
	virtual ~CReciever() {}

	//任务执行体
	virtual int Run();

	//释放
	virtual int Release()
	{
		delete this;
		return 0;
	}
};


#endif

