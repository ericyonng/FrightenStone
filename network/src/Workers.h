#pragma once

class CTaskBase;
class Workers:public CTaskBase
{
public:
	virtual ~Workers();
	static Workers* CreateNew(std::atomic<I32>& rCurThreadNum){	return new Workers(rCurThreadNum); }
	virtual int Release() { Cleans(); delete this; return 0; } 
	virtual void Cleans();
public:
	Workers(std::atomic<I32>& rCurThreadNum);
	I32 Run();

protected:
	std::atomic<I32>& m_nCurThreadNum;
};

