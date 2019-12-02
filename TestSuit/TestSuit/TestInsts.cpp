/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : TestInsts.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/08/10
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "TestSuit/TestSuit/TestInsts.h"

// #include "TestSuit/TestSuit/TestInst/TestDelegate.h"
// #include "TestSuit/TestSuit/TestInst/TestString.h"
// #include "TestSuit/TestSuit/TestInst/TestTime.h"
// #include "TestSuit/TestSuit/TestInst/TestTrigger.h"
// #include "TestSuit/TestSuit/TestInst/TestThreadPool.h"
// #include "TestSuit/TestSuit/TestInst/TestFSDirectory.h"
// #include "TestSuit/TestSuit/TestInst/TestFSFileUtil.h"
// #include "TestSuit/TestSuit/TestInst/TestCpuUtil.h"
// #include "TestSuit/TestSuit/TestInst/TestFile.h"
// #include "TestSuit/TestSuit/TestInst/TestJson.h"
#include "TestSuit/TestSuit/TestInst/TestLogFile.h"
// #include "TestSuit/TestSuit/TestInst/TestRandom.h"
// #include "TestSuit/TestSuit/TestInst/TestAes.h"
// #include "TestSuit/TestSuit/TestInst/TestUtf8.h"
// #include "TestSuit/TestSuit/TestInst/TerstSmartVar.h"
// #include "TestSuit/TestSuit/TestInst/TestTimeWheel.h"
// #include "TestSuit/TestSuit/TestInst/TestSystemUtil.h"
#include "TestSuit/TestSuit/TestInst/TestLogModule.h"
// #include "TestSuit/TestSuit/TestInst/TestXor.h"
// #include "TestSuit/TestSuit/TestInst/TestCrashHandle.h"
// #include "TestSuit/TestSuit/TestInst/TestMemoryPool.h"
// #include "TestSuit/TestSuit/TestInst/TestObjPool.h"
// #include "TestSuit/TestSuit/TestInst/TestMathUtil.h"
// #include "TestSuit/TestSuit/TestInst/TestIocpExample.h"
// #include "TestSuit/TestSuit/TestInst/TestEvent.h"
// #include "TestSuit/TestSuit/TestInst/TestFS_List.h"
// #include "TestSuit/TestSuit/TestInst/TestSingleton.h"
// #include "TestSuit/TestSuit/TestInst/TestFSTlsTable.h"
// #include "TestSuit/TestSuit/TestInst/TestRtti.h"
// #include "TestSuit/TestSuit/TestInst/TestInterfaceComps.h"
// #include "TestSuit/TestSuit/TestInst/TestMessageQueue.h"
// #include "TestSuit/TestSuit/TestInst/TestStream.h"

#include "TestSuit/TestSuit/TestInst/TestServer.h"
#include "TestSuit/TestSuit/TestInst/TestMemAlign.h"
#include "TestSuit/TestSuit/TestInst/TestIniFile.h"

void TestInsts::Run()
{
//     auto __testWeel = [](const fs::FS_ThreadPool *pool)->void
//     {
//         TestTimeWheel::Run(pool);
//     };
// 
//     auto deleg = fs::DelegatePlusFactory::Create<decltype(__testWeel), void, const fs::FS_ThreadPool *>(__testWeel);
//     fs::FS_ThreadPool * pool = new fs::FS_ThreadPool(0, 1);
//     pool->AddTask(deleg);
//     getchar();
//     pool->Clear();
    TestLogModule::Run();
    //TestServer::Run();
    getchar();
    // TestJson::Run();
}