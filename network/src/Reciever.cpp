#include <Net/Reciever.h>
#include <BaseCode/TaskBase.h>
#include <Net/TcpSocket.h>

MEMPOOL_CREATE_IMPLEMENTATION(CReciever, m_MemObj)

int CReciever::Run()
{
	ASSERTEX(TcpServerMgr.Recv());
	return 0;
}
