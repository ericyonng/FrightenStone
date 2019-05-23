#include "TestSuit/TestSuit/TestInst/TestDelegate.h"
#include "TestSuit/TestSuit/TestInst/TestString.h"
#include "TestSuit/TestSuit/TestInst/TestTime.h"
#include "TestSuit/TestSuit/TestInst/TestTrigger.h"
#include "TestSuit/TestSuit/TestInst/TestThreadPool.h"
#include "TestSuit/TestSuit/TestInst/TestFSDirectory.h"
#include "TestSuit/TestSuit/TestInst/TestFSFileUtil.h"
#include "TestSuit/TestSuit/TestInst/TestCpuUtil.h"
#include "TestSuit/TestSuit/TestInst/TestFile.h"
#include "TestSuit/TestSuit/TestInst/TestJson.h"

int main()
{
    TestJson::Run();
    std::cout << "main end" << std::endl;
    getchar();
    return 0;
}