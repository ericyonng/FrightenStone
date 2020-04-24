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
 * @file  : TestCrashHandle.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/07/15
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestCrashHandle_H__
#define __Test_TestCrashHandle_H__

#pragma once

#include "stdafx.h"
// 
// class TestCrashEasyGloal
// {
// public:
//     TestCrashEasyGloal() {  }
//     virtual ~TestCrashEasyGloal()
//     {}
// #pragma region init/finish
// public:
//     virtual Int32 Init()
//     {
//         _log = g_Log;
//         _log->InitModule("main");
//         return StatusDefs::Success;
//     }
//     virtual void Finish()
//     {
//         // 释放组件
//         Fs_SafeFree(_log);
//     }
// #pragma endregion
// 
//     // 获取资源
// public:
//     virtual fs::ILog *GetLog()
//     {
//         return _log;
//     }
// 
// private:
//     fs::ILog *_log;
// };
// 
// TestCrashEasyGloal g_Global;
// 
// class TestCrashHandle
// {
// public:
//     static void Run()
//     {
// //         g_EasyGlobal = new TestCrashEasyGloal;
// //         g_EasyGlobal->Init();
//         g_Global.Init();
//         Int32 i = 100;
//         while(i-- > 0)
//         {
//             SystemUtil::Sleep(1);
//         }
//         fs::CrashHandleUtil::InitCrashHandleParams();
// /*        std::cout << fs::CrashHandleUtil::FS_CaptureStackBackTrace() << std::endl;*/
//         Int32 *intPtr = NULL;
//         *intPtr = 89;
// //         __try
// //         {
// // 
// //         }
// //         __except(fs::CrashHandleUtil::RecordExceptionInfo(*GET_EXCEPTION_INFO()))
// //         {
// //         }
//     }
// };

#endif
