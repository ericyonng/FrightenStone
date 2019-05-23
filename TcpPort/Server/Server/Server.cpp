//

#include "pch.h"
#include <iostream>

#include "TcpPort/Server/Impl/InitProcess.h"
#include "TcpPort/Server/Impl/TcpServer.h"

InitProcess g_InitProcess;

bool g_bRun = true;
void cmdThread()
{
    while(true)
    {
        char cmdBuf[256] = {};
        scanf("%s", cmdBuf);
        if(0 == strcmp(cmdBuf, "exit"))
        {
            g_bRun = false;
            printf("quit cmdThread \n");
            break;
        }
        else {
            printf("unsurpport。\n");
        }
    }
}

int main()
{
    TcpServer server;
    server.InitSocket();
    server.Bind(nullptr, 4567);
    server.Listen(200);

    std::thread t1(cmdThread);
    t1.detach();

    while(g_bRun)
    {
        server.OnRun();
    }
    server.Close();
    printf("exit。\n");
    getchar();
    return 0;
}
