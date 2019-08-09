#include "stdafx.h"
#include "TestSuit/TestSuit/TestInsts.h"

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
#include "TestSuit/TestSuit/TestInst/TestLogFile.h"
#include "TestSuit/TestSuit/TestInst/TestRandom.h"
#include "TestSuit/TestSuit/TestInst/TestAes.h"
#include "TestSuit/TestSuit/TestInst/TestUtf8.h"
#include "TestSuit/TestSuit/TestInst/TerstSmartVar.h"
#include "TestSuit/TestSuit/TestInst/TestTimeWheel.h"
#include "TestSuit/TestSuit/TestInst/TestSystemUtil.h"
#include "TestSuit/TestSuit/TestInst/TestLogModule.h"
#include "TestSuit/TestSuit/TestInst/TestXor.h"
#include "TestSuit/TestSuit/TestInst/TestCrashHandle.h"
#include "TestSuit/TestSuit/TestInst/TestMemoryPool.h"
#include "TestSuit/TestSuit/TestInst/TestObjPool.h"
#include "TestSuit/TestSuit/TestInst/TestMathUtil.h"
#include "TestSuit/TestSuit/TestInst/TestIocpExample.h"
#include "TestSuit/TestSuit/TestInst/TestEvent.h"

void TestInsts::Run()
{
    TestCrashHandle::Run();
}